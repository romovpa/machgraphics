#include "imageprocessor.h"

#include <QtGui>
#include <math.h>

ImageProcessor::ImageProcessor(const QImage &image, QRect rect, QObject *parent) :
	QThread(parent),
	image(image),
	rect(rect)
{
}

QImage ImageProcessor::getImage()
{
	return image;
}

void ImageProcessor::run()
{
	emit busyChanged(true);
	process();
	emit busyChanged(false);
}

void ImageProcessor::process()
{
}

void ImageProcessor::updateImage()
{
	emit imageChanged(image);
}

void ImageProcessor::setProgress(int value)
{
	emit progressChanged(value);
}

void ImageProcessor::setProgress(int volume, int done)
{
	if (volume > 0)
		setProgress(round( (double)done/(double)volume ));
}

void ImageProcessor::setProgress(double value)
{
	setProgress(round(100*value));
}
