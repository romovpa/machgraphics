#include "noiseprocessor.h"

#include <QtGui>
#include <math.h>

NoiseProcessor::NoiseProcessor(const QImage &image, QRect rect, QObject *parent) :
	ImageProcessor(image, rect, parent)
{
}

float rnd()
{
	return (float)rand() / (float)RAND_MAX;
}

float normrnd(float sigma)
{
	float r1 = rnd();
	float r2 = rnd();
	float rn = sqrtf(-2*log(r1))*cosf(2*3.1415926f*r2);
	return rn*sigma;
}

int normnoise(int old, float sigma)
{
	float delta = normrnd(sigma);
	float x = (float)old + delta;
	x = (x > 0 ? x : 0);
	x = (x < 255 ? x : 255);
	return round(x);
}

void NoiseProcessor::setSigma(double val)
{
	this->sigma = val;
}

void NoiseProcessor::setType(int type)
{
	this->type = type;
}

void NoiseProcessor::process()
{
	int volume = rect.width() * rect.height();
	int done = 0;
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
//		setProgress(volume, done);
		for (int j = rect.left(); j <= rect.right(); j++, done++) {
			QRgb color = image.pixel(j, i);
			float deltaR, deltaG, deltaB;
			switch (type) {
				case GAUSSIAN:
					deltaR = normrnd(sigma);
					deltaG = normrnd(sigma);
					deltaB = normrnd(sigma);
					color = qRgb(normnoise(qRed(color), sigma),
								 normnoise(qGreen(color), sigma),
								 normnoise(qBlue(color), sigma));
					break;
				case RANDOM:
					if (rnd() < sigma)
						color = qRgb(roundf(255*rnd()), roundf(255*rnd()), roundf(255*rnd()));
					break;
				case SALTPEPER:
					if (rnd() < sigma) {
						if (rnd() < 0.5)
							color = qRgb(0, 0, 0);
						else
							color = qRgb(255, 255, 255);
					}
					break;
			}
			image.setPixel(j, i, color);
		}
	}
	updateImage();
}
