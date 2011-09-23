#include "histogramprocessor.h"

#include <QtGui>

HistogramProcessor::HistogramProcessor(const QImage &image, QRect rect, QObject *parent) :
	ImageProcessor(image, rect, parent)
{
}

void HistogramProcessor::setType(HistogramTransform type)
{
	this->type = type;
}

void HistogramProcessor::process()
{
	switch (type) {
		case RGB_LINEAR_STRETCH:
			rgbStretch();
			break;
		case LUMA_LINEAR_STRETCH:
			lumaStretch();
			break;
		default:
			qWarning() << "Invalid histogram transformation: " << type;
	}
}

void HistogramProcessor::rgbStretch()
{
	double minR = 255, minG = 255, minB = 255;
	double maxR = 0, maxG = 0, maxB = 0;
	// finding min and max
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		QRgb *line = (QRgb*)image.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); ++j) {
			QRgb cur = line[j];
			double r = qRed(cur), g = qGreen(cur), b = qBlue(cur);
			if (r < minR) minR = r;
			if (g < minG) minG = g;
			if (b < minB) minB = b;
			if (r > maxR) maxR = r;
			if (g > maxG) maxG = g;
			if (b > maxB) maxB = b;
		}
	}
	// fixing zero-interval stretching
	if (maxR <= minR) minR = 0, maxR = 255;
	if (maxG <= minG) minG = 0, maxG = 255;
	if (maxB <= minB) minB = 0, maxB = 255;
	// stretching
	for (int i = rect.top(); i <= rect.bottom(); i++) {
		QRgb *line = (QRgb*)image.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); j++) {
			QRgb cur = line[j];
			line[j] = qRgb( (int) (qRed(cur)-minR)/(maxR-minR)*255.0+0.5,
							(int) (qGreen(cur)-minG)/(maxG-minG)*255.0+0.5,
							(int) (qBlue(cur)-minB)/(maxB-minB)*255.0+0.5 );
		}
	}
}

void HistogramProcessor::lumaStretch()
{
	double minY = 255, maxY = 0;
	// finding min and max
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		QRgb *line = (QRgb*)image.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); ++j) {
			QRgb cur = line[j];
			double r = qRed(cur), g = qGreen(cur), b = qBlue(cur);
			double y = 0.2125*r + 0.7154*g + 0.0721*b;
			if (y < minY) minY = y;
			if (y > maxY) maxY = y;
		}
	}
	// fixing zero-interval stretching
	if (maxY <= minY) minY = 0, maxY = 255;
	// stretching
	for (int i = rect.top(); i <= rect.bottom(); ++i) {
		QRgb *line = (QRgb*)image.scanLine(i);
		for (int j = rect.left(); j <= rect.right(); ++j) {
			QRgb cur = line[j];
			double r = qRed(cur), g = qGreen(cur), b = qBlue(cur);
			double y = 0.2125*r + 0.7154*g + 0.0721*b;
			double ynew = (y - minY)/(maxY - minY)*255;
			double alpha = ynew/y;

			r = r*alpha, g = g*alpha, b = b*alpha;
			if (r < 0) r = 0;
			if (r > 255) r = 255;
			if (g < 0) g = 0;
			if (g > 255) g = 255;
			if (b < 0) b = 0;
			if (b > 255) b = 255;

			line[j] = qRgb((int)(r+0.5), (int)(g+0.5), (int)(b+0.5));
		}
	}
}
