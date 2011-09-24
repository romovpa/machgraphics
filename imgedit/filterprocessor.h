#ifndef FILTERPROCESSOR_H
#define FILTERPROCESSOR_H

#include "imageprocessor.h"

class FilterProcessor : public ImageProcessor
{
public:
	FilterProcessor(const QImage &, QRect, QObject *parent = 0);

	enum FilterTypes {
		NOFILTER = -1,
		GAUSSIAN = 0,
		UNSHARP,
		MEDIAN,
		GLASS_EFFECT,
		CONVOLUTION
	};

	void setType(int);
	void setSigma(double);
	void setAlpha(double);
	void setRadius(int);
	void setKernelSize(int);
	void setKernel(double *kernel, int n, int m);

protected:
	void process();

private:
	void convolve(double *kernel, int n, int m);

	void normalizeKernel(double *kernel, int n);
	void gaussKernel1D(double *kernel, int n, double sigma);
	void gaussKernel2D(double *kernel, int n, double sigma);
	void unsharpKernel(double *kernel, int n, double sigma, double alpha);
	void median();
	void glass();

	int collectNeighbors(int, int, int, quint8 *, quint8 *, quint8 *);
	int findMedian(quint8 *arr, int n);
	int makePartition(quint8 *arr, int l, int r);

	int type;
	double sigma, alpha;
	int kerSize, radius;
	double *userKer;
	int kerN, kerM;
};

#endif // FILTERPROCESSOR_H
