#include "imageprocessor.h"

#include <QtGui>

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

void ImageProcessor::progress(double value)
{
	emit progressChanged(value);
}

void ImageProcessor::process()
{
}

void ImageProcessor::run()
{
	emit processStarted();
	process();
	emit processFinished();
}
