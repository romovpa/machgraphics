#include "filterprocessor.h"

#include <QtGui>
#include <math.h>
#include <cstdio>

FilterProcessor::FilterProcessor(const QImage &image, QRect rect, QObject *parent) :
	ImageProcessor(image, rect, parent),
	type(NOFILTER),
	alpha(0.2)
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

void FilterProcessor::setAlpha(double alpha)
{
	this->alpha = alpha;
}

void FilterProcessor::setRadius(int radius)
{
	this->radius = radius;
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
			newLine[j] = qRgb(double2int(sumR), double2int(sumG), double2int(sumB));
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

void writeKernel(double *ker, int n, int m, const char *filename)
{
	FILE *out = fopen(filename, "wt");
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j)
			fprintf(out, "%lf ", ker[i*m+j]);
		fprintf(out, "\n");
	}
	fclose(out);
}

void FilterProcessor::unsharpKernel(double *kernel, int n, double sigma, double alpha)
{
	gaussKernel2D(kernel, n, sigma);
	for (int i = 0; i < n*n; ++i)
		kernel[i] *= -alpha;
	int c = (n+1)/2;
	kernel[c*n+c] += 1+alpha;
	normalizeKernel(kernel, n*n);
}

void FilterProcessor::median()
{
	int maxPixels = (2*radius+1)*(2*radius+1);
	quint8 *rList = new quint8[maxPixels];
	quint8 *gList = new quint8[maxPixels];
	quint8 *bList = new quint8[maxPixels];

	QImage newImage(image);
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		progress((double)(i-rect.top())/rect.height());
		QRgb *newLine = (QRgb*)newImage.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); ++j) {
			int len = collectNeighbors(i, j, radius, rList, gList, bList);
			newLine[j] = qRgb(findMedian(rList, len), findMedian(gList, len), findMedian(bList, len));
		}
	}
	image = newImage;
}

void FilterProcessor::glass()
{
	QImage newImage(image);
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		progress((double)(i-rect.top())/rect.height());
		QRgb *newLine = (QRgb*)newImage.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); ++j) {
			int i1 = i + rand()%(2*radius+1) - radius;
			int j1 = j + rand()%(2*radius+1) - radius;
			if (i1 < 0) i1 = 0;
			if (i1 >= image.height()) i1 = image.height()-1;
			if (j1 < 0) j1 = 0;
			if (j1 >= image.width()) j1 = image.width()-1;
			newLine[j] = image.pixel(j1, i1);
		}
	}
	image = newImage;
}

int FilterProcessor::collectNeighbors(int i, int j, int radius,
									  quint8 *rList, quint8 *gList, quint8 *bList)
{
	int end = 0;
	int radius2 = radius*radius;
	for (int p = -radius; p <= radius; ++p)
		for (int q = -radius; q <= radius; ++q) {
			if (i+p < 0 || i+p >= image.height()) continue;
			if (j+q < 0 || j+q >= image.width()) continue;
			if (p*p + q*q > radius2) continue;
			QRgb pixel = image.pixel(j+q, i+p);
			rList[end] = qRed(pixel);
			gList[end] = qGreen(pixel);
			bList[end] = qBlue(pixel);
			++end;
		}
	return end;
}

int FilterProcessor::findMedian(quint8 *arr, int n)
{
	int k = n/2;
	int l = 0, r = n;
	while (l+1 < r) {
		int q = makePartition(arr, l, r);
		if (q <= k)
			l = q;
		else
			r = q;
	}
	return arr[l];
}

int FilterProcessor::makePartition(quint8 *arr, int l, int r)
{
	int x = arr[r-1]; // pivot value
	int i = l-1;
	for (int j = l; j < r-1;  ++j)
		if (arr[j] <= x) {
			i++;
			quint8 t = arr[j];
			arr[j] = arr[i];
			arr[i] = t;
		}
	quint8 t = arr[i+1];
	arr[i+1] = arr[r-1];
	arr[r-1] = t;
	return i+1;
}

void FilterProcessor::process()
{
	double *kernel;
	switch (type) {
		case NOFILTER:
			break;

		case GAUSSIAN:
			kernel = new double[kerSize];
			gaussKernel1D(kernel, kerSize, sigma);
			convolve(kernel, kerSize, 1);
			convolve(kernel, 1, kerSize);
			delete kernel;
			break;

		case UNSHARP:
			kernel = new double[kerSize*kerSize];
			unsharpKernel(kernel, kerSize, sigma, alpha);
			convolve(kernel, kerSize, kerSize);
			delete kernel;
			break;

		case MEDIAN:
			median();
			break;

		case GLASS_EFFECT:
			glass();
			break;

		case CONVOLUTION:
			convolve(userKer, kerN, kerM);

		default:
			qWarning() << "FilterProcessor: invalid filter type";

	}
}
