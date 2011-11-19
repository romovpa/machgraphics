#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "pedestrian.h"

using namespace std;
using namespace cv;

const int DEFAULT_NEGNUM = 5;
const int DEFAULT_BOOTSTRAP = 0;
char * const DEFAULT_MODEL_FILE = "model.txt";
char * const DEFAULT_TRAIN_PREFIX = "train/";
char * const DEFAULT_TRAIN_GT = "train/train-processed.idl";
char * const DEFAULT_TEST_PREFIX = "test/";
char * const DEFAULT_TEST_GT = "test/test-processed.idl";

void help()
{
	printf("Usage: pedestrian train <options> \n");
	printf("       pedestrian test  <options> \n");
	printf("       pedestrian detect <files> <options> \n");
	printf("\n");
	printf("  --model FILE    specify model file (default: %s) \n", DEFAULT_MODEL_FILE);
	printf("\n");
	printf("Training keys: \n");
	printf("  --prefix <dir>  prefix of the path to training images (default: %s)\n", DEFAULT_TRAIN_PREFIX);
	printf("  --gt <file>     ground-truth annotation file (default: %s)\n", DEFAULT_TRAIN_GT);
	printf("  --negnum N      number of negative examples per one positive (default %d) \n", DEFAULT_NEGNUM);
	printf("  --crossval N    learn SVM using N-fold cross validation \n");
	printf("  --bootstrap N   improve classifier using N iters of bootstrapping (default: %d)\n", DEFAULT_BOOTSTRAP);
	printf("  --chi2          use chi2 approximate kernel mapping\n");
	printf("\n");
	printf("Testing keys: \n");
	printf("  --prefix <dir>  prefix of the path to testing images (default: %s)\n", DEFAULT_TEST_PREFIX);
	printf("  --gt <file>     ground-truth annotation file (default: %s)\n", DEFAULT_TEST_GT);
	printf("  --verbose       verbose mode\n");
	printf("\n");
	printf("Detector keys: \n");
	printf("  --vis           view source images with pedestrians boxed\n");
	exit(1);
}

void cmd_train(int argc, char *argv[])
{
	char *modelfile = DEFAULT_MODEL_FILE;
	char *prefix = DEFAULT_TRAIN_PREFIX;
	char *gt = DEFAULT_TRAIN_GT;
	int negnum = DEFAULT_NEGNUM;
	int bootstrap = DEFAULT_BOOTSTRAP;
	int crossval = 0;
	bool chi2 = false;

	for (int argi = 0; argi < argc; ++argi) {
		char *cmd = argv[argi];
		char *param = 0;
		if (argi+1 < argc)
			param = argv[argi+1];

		if (!strcmp(cmd, "--model") && param)
			modelfile = param, ++argi;
		else if (!strcmp(cmd, "--negnum") && param)
			negnum = atoi(param), ++argi;
		else if (!strcmp(cmd, "--crossval") && param)
			crossval = atoi(param), ++argi;
		else if (!strcmp(cmd, "--prefix") && param)
			prefix = param, ++argi;
		else if (!strcmp(cmd, "--gt") && param)
			gt = param, ++argi;
		else if (!strcmp(cmd, "--bootstrap") && param)
			bootstrap = atoi(param), ++argi;
		else {
			fprintf(stderr, "Invalid key '%s'\n", cmd);
			help();
		}
	}

	Learner learner;
	learner.setNegativesPerPositive(negnum);
	learner.loadTrainSet(prefix, gt);

	if (crossval)
		learner.selectModelCrossValidation(crossval);
	learner.train();

	for (int iter = 0; iter < bootstrap; ++iter) {
		printf("Bootstrap training set #%d\n", iter);
		learner.bootstrapTrainSet();
		printf("Learning again...\n");
		if (crossval)
			learner.selectModelCrossValidation(crossval);
		learner.train();
	}

	Model model = learner.getModel();

	model.save(modelfile);
}

void cmd_detect(int argc, char *argv[])
{
	char *modelfile = DEFAULT_MODEL_FILE;
	bool vis = false;
	vector<string> files;

	for (int argi = 0; argi < argc; ++argi) {
		char *cmd = argv[argi];
		char *param;
		if (argi+1 < argc)
			param = argv[argi+1];

		if (!strcmp(cmd, "--vis"))
			vis = true;
		if (!strcmp(cmd, "--model") && param)
			modelfile = param, ++argi;
		else
			files.push_back(cmd);
	}

	Model model;
	model.load(modelfile);

	if (vis)
		namedWindow("Pedestrians");

	printf("Detecting pedestrians: \n");
	Detector detector(model);
	for (int i = 0; i < files.size(); ++i) {
		printf("  %s: ", files[i].c_str());
		Mat img = imread(files[i], 1);
		if (img.empty()) {
			printf("no such file\n");
			continue;
		}

		// printing result
		detector.detect(img);
		printf("%d detected (", detector.detected());
		for (int j = 0; j < detector.detected(); ++j) {
			int shift = detector.nextShift();
			printf("%d", shift);
			if (j != detector.detected()-1)
				printf(", ");
			if (vis) {
				rectangle(img, Rect_<int>(shift, 1, 80, img.rows-1), CV_RGB(0, 255, 0), 2);
			}
		}
		printf(")\n");

		// visualization
		imshow("Pedestrians", img);
		waitKey();

	}

	if (vis)
		destroyWindow("Pedestrians");
}

void cmd_test(int argc, char *argv[])
{
	char *modelfile = DEFAULT_MODEL_FILE;
	char *prefix = DEFAULT_TEST_PREFIX;
	char *gt = DEFAULT_TEST_GT;
	bool verbose = false;

	for (int argi = 0; argi < argc; ++argi) {
		char *cmd = argv[argi];
		char *param;
		if (argi+1 < argc)
			param = argv[argi+1];

		if (!strcmp(cmd, "--model") && param)
			modelfile = param, ++argi;
		else if (!strcmp(cmd, "--prefix") && param)
			prefix = param, ++argi;
		else if (!strcmp(cmd, "--gt") && param)
			gt = param, ++argi;
		else if (!strcmp(cmd, "--verbose"))
			verbose = true;
		else {
			fprintf(stderr, "Invalid key '%s'\n", cmd);
			help();
		}
	}

	typedef map<string, vector<int> > ShiftsList;
	ShiftsList tests;

	// reading test list
	ifstream f(gt);
	if (!f) {
		fprintf(stderr, "Cannot open file '%s'\n", gt);
		exit(1);
	}
	while (!f.eof()) {
		string name;
		int y0, x0, y1, x1;
		f >> name >> y0 >> x0 >> y1 >> x1;
		if (f.eof())
			break;
		if (!tests.count(name))
			tests[name] = vector<int>();
		tests[name].push_back(x0);
	}
	f.close();

	// loading detector
	Model model;
	model.load(modelfile);
	Detector detector(model);

	// testing
	int numTP = 0, numDET = 0, numGT = 0;
	for (ShiftsList::iterator it = tests.begin(); it != tests.end(); ++it) {
		string name = (*it).first;
		vector<int> shiftsGT = (*it).second;

		if (verbose)
			printf("  %s: ", name.c_str());

		Mat img = imread(string(prefix)+name+string(".png"), 1);
		detector.detect(img);

		numGT += shiftsGT.size();
		numDET += detector.detected();
		// computing TPs
		int hereTP = 0;
		const int frameWidth = 80;
		const int minWidthIntersect = 40;
		int shift;
		while ((shift = detector.nextShift()) >= 0)
			for (int i = 0; i < shiftsGT.size(); ++i)
				if (shiftsGT[i] >= 0 &&
						shift > shiftsGT[i]-frameWidth+minWidthIntersect &&
						shift < shiftsGT[i]+frameWidth-minWidthIntersect) {
					// true positive found
					shiftsGT[i] = -1;
					++hereTP;
				}
		numTP += hereTP;

		if (verbose)
			printf("recognized %d/%d, falses %d\n", hereTP, (int)shiftsGT.size(), (int)detector.detected()-hereTP);
	}

	printf("Total\n");
	printf("  TP:  %d\n", numTP);
	printf("  DET: %d\n", numDET);
	printf("  GT:  %d\n", numGT);
	printf("  -----\n");
	printf("  Precision: %.4f%%\n", (float)numTP/numDET*100);
	printf("  Recall:    %.4f%%\n", (float)numTP/numGT*100);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		help();

	if (!strcmp(argv[1], "train"))
		cmd_train(argc-2, argv+2);
	else if (!strcmp(argv[1], "detect"))
		cmd_detect(argc-2, argv+2);
	else if (!strcmp(argv[1], "test"))
		cmd_test(argc-2, argv+2);
	else
		help();

	return 0;
}
