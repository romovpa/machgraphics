#ifndef GEOMETRYPROCESSOR_H
#define GEOMETRYPROCESSOR_H

#include "imageprocessor.h"

#include <QRgb>

class GeometryProcessor : public ImageProcessor
{
public:
	GeometryProcessor(const QImage &image, QRect rect, QObject *parent = 0);

	enum FillStyle {
		FILL_BY_OLD_IMAGE = 0,
		FILL_BY_COLOR = 1
	};

public slots:
	void setScale(double);
	void setAngle(double);
	void setFillStyle(int);
	void setFillColor(QRgb);

protected:
	void process();

private:
	double scale, angle;
	int fillStyle;
	QRgb fillColor;

	QRgb interpolate(int i, int j, double dx, double dy);

};

#endif // GEOMETRYPROCESSOR_H
