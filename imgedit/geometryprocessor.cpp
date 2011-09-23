#include "geometryprocessor.h"

#include <QtGui>
#include <math.h>

GeometryProcessor::GeometryProcessor(const QImage &image, QRect rect, QObject *parent) :
	ImageProcessor(image, rect, parent),
	scale(1),
	angle(0),
	fillStyle(FILL_BY_OLD_IMAGE)
{
}

void GeometryProcessor::setAngle(double value)
{
	this->angle = value;
}

void GeometryProcessor::setScale(double value)
{
	this->scale = value;
}

void GeometryProcessor::setFillStyle(int style)
{
	this->fillStyle = style;
}

void GeometryProcessor::setFillColor(QRgb color)
{
	this->fillColor = color;
}

double fituint8(double x)
{
	if (x < 0) x = 0;
	if (x > 255) x = 255;
	return x;
}

QRgb GeometryProcessor::interpolate(int i, int j, double dx, double dy)
{
	double cr, cg, cb;
	if (j+1 >= image.width()) {
		if (i+1 >= image.height())
			return qRgb(255, 255, 255);
		cr = (double)(qRed(image.pixel(j,i+1))-qRed(image.pixel(j,i))) * dy + qRed(image.pixel(j,i));
		cg = (double)(qGreen(image.pixel(j,i+1))-qGreen(image.pixel(j,i))) * dy + qGreen(image.pixel(j,i));
		cb = (double)(qBlue(image.pixel(j,i+1))-qBlue(image.pixel(j,i))) * dy + qBlue(image.pixel(j,i));
	}
	else if (i+1 >= image.height()) {
		cr = (double)(qRed(image.pixel(j+1,i))-qRed(image.pixel(j,i))) * dx + qRed(image.pixel(j,i));
		cg = (double)(qGreen(image.pixel(j+1,i))-qGreen(image.pixel(j,i))) * dx + qGreen(image.pixel(j,i));
		cb = (double)(qBlue(image.pixel(j+1,i))-qBlue(image.pixel(j,i))) * dx + qBlue(image.pixel(j,i));
	}
	else {
		double ar = (double)(qRed(image.pixel(j+1,i))-qRed(image.pixel(j,i))) * dx + qRed(image.pixel(j,i));
		double ag = (double)(qGreen(image.pixel(j+1,i))-qGreen(image.pixel(j,i))) * dx + qGreen(image.pixel(j,i));
		double ab = (double)(qBlue(image.pixel(j+1,i))-qBlue(image.pixel(j,i))) * dx + qBlue(image.pixel(j,i));
		double br = (double)(qRed(image.pixel(j+1,i+1))-qRed(image.pixel(j,i+1))) * dx + qRed(image.pixel(j,i+1));
		double bg = (double)(qGreen(image.pixel(j+1,i+1))-qGreen(image.pixel(j,i+1))) * dx + qGreen(image.pixel(j,i+1));
		double bb = (double)(qBlue(image.pixel(j+1,i+1))-qBlue(image.pixel(j,i+1))) * dx + qBlue(image.pixel(j,i+1));
		cr = (br - ar) * dy + ar;
		cg = (bg - ag) * dy + ag;
		cb = (bb - ab) * dy + ab;
	}
	cr = fituint8(cr);
	cg = fituint8(cg);
	cb = fituint8(cb);
	return qRgb((int)(cr+0.5), (int)(cg+0.5), (int)(cb+0.5));
}

void GeometryProcessor::process()
{
	int n = image.height();
	int m = image.width();
	double theta = angle / 180 * 3.141592653589793;
	double dx = 1.0/(m-1);
	double dy = 1.0/(n-1);
	double x0 = ((double)rect.left() + (double)rect.width()/2) / (double)image.width();
	double y0 = ((double)rect.top() + (double)rect.height()/2) / (double)image.height();

	QImage newImg(image.size(), QImage::Format_RGB32);
	for (int i = 0; i < n; ++i) {
		QRgb *line = (QRgb*)image.scanLine(i);
		QRgb *newLine = (QRgb*)newImg.scanLine(i);
		for (int j = 0; j < m; ++j) {

			double x = j*dx;
			double y = i*dy;
			double x1 = x0 + 1/scale * (cos(theta)*(x-x0)   + sin(theta)*(y-y0));
			double y1 = y0 + 1/scale * (- sin(theta)*(x-x0) + cos(theta)*(y-y0));

			if (x1/dx >= rect.left() && x1/dx < rect.right()+1 &&
					y1/dy >= rect.top() && y1/dy < rect.bottom()+1) {
				// point from rotated rectangle
				// interpolate them, using bilinear interpolation
				int j2 = (int)(x1/dx);
				int i2 = (int)(y1/dy);
				newLine[j] = interpolate(i2, j2, (x1-j2*dx)/dx, (y1-i2*dy)/dy);
			}
			else if (rect.contains(j, i)) {
				// point from void
				switch (fillStyle) {
					case FILL_BY_OLD_IMAGE:
						newLine[j] = line[j];
						break;
					case FILL_BY_COLOR:
						newLine[j] = fillColor;
						break;
				}

			}
			else
				newLine[j] = line[j];

		}
	}

	image = newImg;
}
