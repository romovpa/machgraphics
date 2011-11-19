#include "pedestrian.h"

#include "linear/linear.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <math.h>

#include "debug.h"

using namespace cv;
using namespace std;

//
// Auxiliary Functions
//

const float eps = 1e-3;

void gradient2anglebin(const Mat &grX, const Mat &grY, Mat &angle, Mat &magnitude, int nbins)
{
	CV_Assert((grX.size == grY.size) && (grX.dims == 2));
	CV_Assert((grX.type() == CV_32F) && (grY.type() == CV_32F));

	int rows = grX.rows, cols = grX.cols;
	float binLen = CV_PI/nbins;

	angle.create(rows, cols, CV_8U);
	magnitude.create(rows, cols, CV_32F);

	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j) {
			float x = grY.at<float>(i, j);
			float y = grX.at<float>(i, j);
			float k = y/(x+eps);
			float alpha = atan(k)+CV_PI/2;
			angle.at<uchar>(i, j) = (uchar)(alpha/binLen);
			magnitude.at<float>(i, j) = sqrt(x*x+y*y);
		}
}

void softhist(const Mat &bin, const Mat &magnitude, Mat &hist, int nbins)
{
	CV_Assert(bin.dims == 2);
	CV_Assert((bin.type() == CV_8U) && (magnitude.type() == CV_32F));
	CV_Assert(bin.size == magnitude.size);

	for (int b = 0; b < nbins; ++b)
		hist.at<float>(b) = 0;
	for (int i = 0; i < bin.rows; ++i)
		for (int j = 0; j < bin.cols; ++j)
			hist.at<float>(0, bin.at<uchar>(i, j)) += magnitude.at<float>(i, j);
}

//
// Feature Extractor
//

const Size2i FeatureExtractor::cellSize(8, 8);
const Size2i FeatureExtractor::blockSize(1, 1);
const int FeatureExtractor::nbins = 8;
const int FeatureExtractor::windowWidth = 80;

FeatureExtractor::FeatureExtractor(Mat sourceImg) :
	img(sourceImg)
{
	preprocess();
}

Size2i FeatureExtractor::getWindowSize()
{
	return windowSize;
}

int FeatureExtractor::featureLength()
{
	return length;
}

int FeatureExtractor::maxShift()
{
	return img.cols - windowSize.width;
}

void FeatureExtractor::preprocess()
{
	// convert RGB to Y if needed
	if (img.type() == CV_8UC3)
		cvtColor(img, img, CV_RGB2GRAY);

	// computing gradients
	Mat grX, grY;

	float ker[] = {-1, 0, 1};
	Mat kernel(1, 3, CV_32F, ker);

	filter2D(img, grX, CV_32F, kernel);
	filter2D(img, grY, CV_32F, kernel.t());

	// computing angles
	gradient2anglebin(grX, grY, orient, magnitude, nbins);

	// computing grid
	cellsNum.height = img.rows / cellSize.height;
	cellsNum.width = windowWidth / cellSize.width;
	blocksNum.height = cellsNum.height / blockSize.height;
	blocksNum.width = cellsNum.width / blockSize.width;
	windowSize.height = blocksNum.height * blockSize.height * cellSize.height;
	windowSize.width = blocksNum.width * blockSize.width * cellSize.width;
	length = (blocksNum.width * blockSize.width) * (blocksNum.height * blockSize.height) * nbins;
}

void FeatureExtractor::feature(int shift, Mat &out)
{
	CV_Assert(out.type() == CV_32F);
	CV_Assert((out.rows == 1) && (out.cols == length));

	int place = 0;
	for (int blockRow = 0; blockRow < blocksNum.height; ++blockRow)
	for (int blockCol = 0; blockCol < blocksNum.width; ++blockCol) {
		int blockPlace = place;

		for (int cellRow = blockRow*blockSize.height; cellRow < (blockRow+1)*blockSize.height; ++cellRow)
		for (int cellCol = blockCol*blockSize.width; cellCol < (blockCol+1)*blockSize.width; ++cellCol) {
			Range rows(cellRow*cellSize.height, (cellRow+1)*cellSize.height);
			Range cols(shift+cellCol*cellSize.width, shift+(cellCol+1)*cellSize.width);
			Range outCols(place, place+nbins);

			Mat localOrient = orient(rows, cols);
			Mat localMagnitude = magnitude(rows, cols);
			Mat outLine = out.colRange(outCols);
			softhist(localOrient, localMagnitude, outLine, nbins);
			place += nbins;
		}

		// block normalization
		Mat block = out.colRange(Range(blockPlace, place));
		Mat normSqr = block*block.t();
		float norm = sqrtf(normSqr.at<float>(0, 0) + eps);
		block = block / norm;
	}

	CV_Assert(place == length);
}


//
// Model Class
//

Model::Model()
{
	cl_model = 0;
}

Model::~Model()
{
	//free_and_destroy_model(&cl_model);
}

void Model::load(const char *filename)
{
	cl_model = load_model(filename);
}

void Model::save(const char *filename) const
{
	save_model(filename, cl_model);
}


//
// Learner Class
//

const float Learner::defaultC = 1;
const float Learner::Cbase = 10;
const int Learner::minLogC = -4;
const int Learner::maxLogC = 4;

Learner::Learner() :
	negativesPerPositive(2)
{

}

const Model& Learner::getModel()
{
	return model;
}

void Learner::setNegativesPerPositive(int value)
{
	negativesPerPositive = value;
}

void Learner::readTrainImage(int idx, Mat &img)
{
	string filename = trainPrefix + trainNames[idx];
	img = imread(filename, 0);
	if (img.empty()) {
		cerr << "Cannot load image from file '" << filename << "'" << endl;
		exit(1);
	}
}

void Learner::loadTrainSet(const char *prefix, const char *annotFilename)
{
	printf("Loading train set from '%s'\n", prefix);

	trainPrefix = string(prefix);
	paramC = defaultC;

	// reading annotation file
	printf("Reading annotation: '%s' ", annotFilename);
	trainExamples = 0;
	trainNames.clear();
	trainShifts.clear();
	ifstream f(annotFilename);
	while (!f.eof()) {
		string name;
		int y0, x0, y1, x1;
		f >> name >> y0 >> x0 >> y1 >> x1;
		if (f.eof())
			break;
		trainNames.push_back(name+".png");
		trainShifts.push_back(x0-1);
		trainExamples++;
	}
	f.close();
	printf("(%d positives)\n", trainExamples);

	// loading training examples from train set
	Mat img;
	readTrainImage(0, img);
	FeatureExtractor extractor(img);
	printf("Extracting objects: %d\n", trainNames.size());

	objectsNum = trainExamples*(1+negativesPerPositive);
	featureLength = extractor.featureLength();
	int cur = 0;

	labels.clear();
	features.create(objectsNum, featureLength, CV_32F);
	for (int i = 0; i < trainExamples; ++i) {
		readTrainImage(i, img);
		extractor = FeatureExtractor(img);

		// positive example
		Mat f = features.row(cur);
		extractor.feature(trainShifts[i], f);
		labels.push_back(+1);
		cur++;

		// negative examples
		for (int t = 0; t < negativesPerPositive; ++t) {
			do {
				int shift = rand() % extractor.maxShift();
				if (shift > trainShifts[i]+extractor.windowWidth ||
					  shift+extractor.windowWidth < trainShifts[i]) {

					Mat f = features.row(cur);
					extractor.feature(shift, f);
					labels.push_back(-1);
					cur++;

					break;
				}
			} while (1);
		}
	}

	// initialize parameter C
	paramC = defaultC;
}

void Learner::bootstrapTrainSet()
{
	const int frameWidth = 80;
	const int maxWidthIntersect = 20;
	const int maxNewObjects = 100;

	Detector detector(model);

	Mat featuresAdd;
	featuresAdd.create(maxNewObjects, features.cols, features.type());

	Mat img;
	int objectsAdd = 0;

	for (int t = 0; t < trainExamples; ++t) {
		readTrainImage(t, img);
		detector.detect(img);
		FeatureExtractor extractor(img);

		int shift;
		int shiftGT = trainShifts[t];
		while ((shift = detector.nextShift()) >= 0)
			if (shift < shiftGT-frameWidth+maxWidthIntersect ||
				shift > shiftGT+frameWidth-maxWidthIntersect) {
				// false positive
				Mat f = featuresAdd.row(objectsAdd);
				extractor.feature(shift, f);
				labels.push_back(-1);
				if (++objectsAdd == maxNewObjects)
					break;
			}

		if (objectsAdd == maxNewObjects)
			break;
	}

	Mat featuresOld = features;
	features.create(objectsNum + objectsAdd, featureLength, CV_32F);
	Mat featuresOldArea = features(Range(0, objectsNum), Range::all());
	Mat featuresNewArea = features(Range(objectsNum, objectsNum+objectsAdd), Range::all());
	featuresOld.copyTo(featuresOldArea);
	featuresAdd.copyTo(featuresNewArea);
	objectsNum += objectsAdd;

	printf("New objects added: %d\n", objectsAdd);
}

void Learner::train()
{
	makeLiblinearStructures();
	model.cl_model = ::train(linear_problem, linear_parameter);
	destroyLiblinearStructures();
}

void Learner::selectModelCrossValidation(int nfolds)
{
	printf("Model selection:\n");

	int bestPrecision = -1;
	float bestC;
	int *result = new int[objectsNum];

	printf("Objects: %d\n", objectsNum);
	printf("Features: %d\n", featureLength);

	for (int logC = minLogC; logC <= maxLogC; ++logC) {
		paramC = powf(Cbase, (float)logC);
		printf("C=%f: ", paramC);

		// running liblinear
		makeLiblinearStructures();
		cross_validation(linear_problem, linear_parameter, nfolds, result);
		destroyLiblinearStructures();

		// computing result
		int precision = 0;
		for (int i = 0; i < objectsNum; ++i) {
			if (result[i] == labels[i])
				precision++;
		}
		printf("%d/%d correctly classified\n", precision, objectsNum);
		if (precision > bestPrecision) {
			bestC = paramC;
			bestPrecision = precision;
		}
	}

	printf("Best C=%f\n", bestC);
}

void Learner::makeLiblinearStructures()
{
	// problem structure
	linear_problem = (struct problem*)malloc(sizeof(struct problem));
	linear_problem->bias = -1;
	linear_problem->l = objectsNum;
	linear_problem->n = featureLength;
	linear_problem->y = (int*)malloc(objectsNum*sizeof(int));
	linear_problem->x = (struct feature_node**)malloc(objectsNum*sizeof(struct feature_node *));
	for (int i = 0; i < objectsNum; ++i) {
		linear_problem->y[i] = labels[i];
		linear_problem->x[i] = (struct feature_node*)malloc((featureLength+1)*sizeof(struct feature_node));
		for (int j = 0; j < featureLength; ++j) {
			//cout << (double)features.at<float>(i, j) << ' ';
			linear_problem->x[i][j].index = j;
			linear_problem->x[i][j].value = (double)features.at<float>(i, j);
		}
		linear_problem->x[i][featureLength].index = -1;
		//cout << endl;
	}

	// parameter structure
	linear_parameter = (struct parameter*)malloc(sizeof(struct parameter));
	linear_parameter->solver_type = L2R_L2LOSS_SVC_DUAL;
	linear_parameter->C = (double)paramC;
	linear_parameter->eps = 1e-4;
	linear_parameter->nr_weight = 0;
	linear_parameter->weight_label = 0;
	linear_parameter->weight = 0;
}

void Learner::destroyLiblinearStructures()
{
	free(linear_parameter);
	for (int i = 0; i < objectsNum; ++i) {
		free(linear_problem->x[i]);
	}
	free(linear_problem->y);
	free(linear_problem);
}



//
// Detector Class
//

const int Detector::maxWidthIntersect = 20;

Detector::Detector(const Model &model) :
	model(model), scanStep(10)
{
}

void Detector::setScanStep(int value)
{
	scanStep = value;
}

void Detector::detect(const cv::Mat &img)
{
	FeatureExtractor extractor(img);
	int featureLen = extractor.featureLength();
	struct feature_node *x = (struct feature_node *)malloc(sizeof(struct feature_node)*(featureLen+1));

	vector<int> shifts;
	vector<double> confidence;
	vector<bool> used;

	Mat f;
	f.create(1, featureLen, CV_32F);
	for (int shift = 0; shift < extractor.maxShift(); shift += scanStep) {
		extractor.feature(shift, f);

		// make liblinear feature vector
		for (int i = 0; i < featureLen; ++i) {
			x[i].index = i;
			x[i].value = (double)f.at<float>(0, i);
		}
		x[featureLen].index = -1;

		// run liblinear predict
		double conf[20];
		int decision = predict_values(model.cl_model, x, conf);

		if (decision > 0) {
			shifts.push_back(shift);
			confidence.push_back(conf[0]);
			used.push_back(false);
		}
	}

	// waste reduction
	cleanShifts.clear();
	position = 0;
	while (true) {
		double maxConf = 0;
		int maxIdx = -1;

		// looking for maximum
		for (int i = 0; i < shifts.size(); ++i)
			if (!used[i] && confidence[i]>maxConf)
				maxIdx = i, maxConf = confidence[i];

		if (maxIdx < 0) break;

		// adding detected pedestrian
		cleanShifts.push_back(shifts[maxIdx]);

		// removing non-maximums
		for (int i = 0; i < shifts.size(); ++i) {
			if (shifts[i] > shifts[maxIdx]-extractor.windowWidth+maxWidthIntersect &&
					shifts[i] < shifts[maxIdx]+extractor.windowWidth-maxWidthIntersect)
				used[i] = true;
		}
	}

	free(x);
}

int Detector::detected()
{
	return cleanShifts.size();
}

int Detector::nextShift()
{
	if (position >= cleanShifts.size())
		return -1;
	return cleanShifts[position++];
}

