#ifndef PEDESTRIAN_H
#define PEDESTRIAN_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "linear/linear.h"

// Feature extractor
class FeatureExtractor {
public:
	FeatureExtractor(cv::Mat img);

	/* preprocess image for further feature extraction */
	void preprocess();

	/* extract the feature vector of window */
	void feature(int shift, cv::Mat &out);

	/* maximal shift */
	int maxShift();

	/* actual size of window */
	cv::Size2i getWindowSize();

	/* length of feature vector */
	int featureLength();

	/* RHOG parameters */
	static const cv::Size2i cellSize;
	static const cv::Size2i blockSize;
	static const int nbins;

	/* extractor parameters */
	static const int windowWidth;

private:
	cv::Mat img;
	cv::Mat orient;
	cv::Mat magnitude;

	cv::Size2i blocksNum;
	cv::Size2i cellsNum;
	cv::Size2i windowSize;

	int length;
};

// Model of pedestrian detector
class Model {
public:
	Model();
	~Model();

	void load(const char *filename);
	void save(const char *filename) const;

	struct model *cl_model;
};

// Learner class
class Learner {
public:
	Learner();

	const Model& getModel();

	/* numbers of negatives per one training */
	void setNegativesPerPositive(int value);

	/* load training set */
	void loadTrainSet(const char *prefix, const char *annotFilename);

	/* increase training set using bootstrapping technique */
	void bootstrapTrainSet();

	/* run cross-validation to select best C parameter */
	void selectModelCrossValidation(int nfolds);

	/* run initial training using loaded training set */
	void train();

	/* training parameters */
	static const float defaultC, Cbase;
	static const int minLogC, maxLogC;

private:
	Model model;
	int negativesPerPositive;

	int objectsNum;
	int featureLength;
	cv::Mat features;
	std::vector<int> labels;
	float paramC;

	int trainExamples;
	std::vector<std::string> trainNames;
	std::vector<int> trainShifts;
	std::string trainPrefix;

	void readTrainImage(int idx, cv::Mat &img);

	/* liblinear structures */
	struct problem *linear_problem;
	struct parameter *linear_parameter;

	void makeLiblinearStructures();
	void destroyLiblinearStructures();
};

/* Detector class */
class Detector {
public:
	Detector(const Model &model);

	void detect(const cv::Mat &img);
	int detected();
	int nextShift();

	void setScanStep(int value);

	static const int maxWidthIntersect;

private:
	Model model;
	int scanStep;
	std::vector<int> cleanShifts;
	int position;
};

#endif // PEDESTRIAN_H
