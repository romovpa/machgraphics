#include "filterprocessor.h"

#include <QtGui>
#include <math.h>
#include <cstdio>

FilterProcessor::FilterProcessor(const QImage &image, QRect rect, QObject *parent) :
	ImageProcessor(image, rect, parent),
	type(NOFILTER)
{
}

void FilterProcessor::setType(int type)
{
	this->type = type;
}

void FilterProcessor::setSigma(double sigma)
{
	this->sigma = sigma;
}

void FilterProcessor::setKernelSize(int n)
{
	this->kerSize = n;
}

void FilterProcessor::setKernel(double *kernel, int n, int m)
{
	this->userKer = kernel;
	this->kerN = n;
	this->kerM = m;
}

void FilterProcessor::convolve(double *kernel, int n, int m)
{
	int nh = n/2, mh = m/2;
	QImage newImage(image);
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		QRgb *newLine = (QRgb*)newImage.scanLine(i);
		progress((double)(i-rect.top())/rect.height());
		for (int j = rect.left(); j <= rect.right(); ++j) {
			double sumR = 0, sumG = 0, sumB = 0;
			for (int p = 0; p < n; ++p)
				for (int q = 0; q < m; ++q) {
					int i1 = i+p-nh, j1 = j+q-mh;
					if (i1 < 0) i1 = 0;
					if (i1 >= image.height()) i1 = image.height()-1;
					if (j1 < 0) j1 = 0;
					if (j1 >= image.width()) j1 = image.width()-1;
					QRgb pixel = image.pixel(j1, i1);
					double multiplier = kernel[m*p+q];
					sumR += qRed(pixel)*multiplier;
					sumG += qGreen(pixel)*multiplier;
					sumB += qBlue(pixel)*multiplier;
				}
			newLine[j] = qRgb(sumR, sumG, sumB);
		}
	}
	image = newImage;
}

void FilterProcessor::normalizeKernel(double *kernel, int n)
{
	double sum = 0;
	for (int i = 0; i < n; ++i)
		sum += kernel[i];
	for (int i = 0; i < n; ++i)
		kernel[i] /= sum;
}

void FilterProcessor::gaussKernel1D(double *kernel, int n, double sigma)
{
	for (int i = 0; i < n; i++) {
		double x = (double)i - (n+1)/2;
		kernel[i] = exp(-x*x/(2*sigma*sigma));
	}
	normalizeKernel(kernel, n);
}

void FilterProcessor::gaussKernel2D(double *kernel, int n, double sigma)
{
	double *line = new double[n];
	gaussKernel1D(line, n, sigma);
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			kernel[i*n+j] = line[i]*line[j];
	delete line;
	normalizeKernel(kernel, n*n);
}

void FilterProcessor::unsharpKernel(double *kernel, int n, double sigma)
{
	qDebug() << n << ' ' << sigma;
	gaussKernel2D(kernel, n, sigma);
	for (int i = 0; i < n*n; ++i)
		kernel[i] *= -1;
	int c = (n+1)/2;
	kernel[c*n+c] += 1;
	normalizeKernel(kernel, n*n);
}

void FilterProcessor::process()
{
	double *kernel;
	switch (type) {
		case NOFILTER:
			break;

		case GAUSSIAN:
			kernel = new double[kerSize];
			gaussKernel2D(kernel, kerSize, sigma);
//			convolve(kernel, kerSize, 1);
//			convolve(kernel, 1, kerSize);
			convolve(kernel, kerSize, kerSize);
			delete kernel;
			break;

		case UNSHARP:
			kernel = new double[kerSize*kerSize];
			unsharpKernel(kernel, kerSize, sigma);
			convolve(kernel, kerSize, kerSize);
			delete kernel;
			break;

		case MEDIAN:
			qWarning() << "Median filter is not implemented";
			break;

		case CONVOLUTION:
			convolve(userKer, kerN, kerM);

		default:
			qWarning() << "FilterProcessor: invalid filter type";

	}
}
