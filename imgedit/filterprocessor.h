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
		CONVOLUTION
	};

	void setType(int type);
	void setSigma(double sigma);
	void setKernelSize(int n);
	void setKernel(double *kernel, int n, int m);

protected:
	void process();

private:
	void convolve(double *kernel, int n, int m);

	void normalizeKernel(double *kernel, int n);
	void gaussKernel1D(double *kernel, int n, double sigma);
	void gaussKernel2D(double *kernel, int n, double sigma);
	void unsharpKernel(double *kernel, int n, double sigma);


	int type;
	double sigma;
	int kerSize;
	double *userKer;
	int kerN, kerM;
};

#endif // FILTERPROCESSOR_H
