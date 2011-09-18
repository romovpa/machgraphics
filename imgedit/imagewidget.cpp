#include "imagewidget.h"

#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent) :
	QWidget(parent),
	currentStepScaleFactor(1),
	scaleFactor(1)
{
	grabGesture(Qt::PinchGesture);
}

const QImage& ImageWidget::getImage()
{
	return image;
}

float ImageWidget::getScale()
{
	return scaleFactor * currentStepScaleFactor;
}

void ImageWidget::setImage(const QImage &newImage)
{
	image = newImage;
	updateSize();
	update();
}

void ImageWidget::setScale(float scale)
{
	scaleFactor = scale;
	currentStepScaleFactor = 1;
}

void ImageWidget::updateSize()
{
	int w = ceil(float(image.width()) * getScale());
	int h = ceil(float(image.height()) * getScale());
	setFixedSize(w, h);
}

bool ImageWidget::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture)
		return gestureEvent(static_cast<QGestureEvent*>(event));
	return QWidget::event(event);
}

void ImageWidget::paintEvent(QPaintEvent *)
{
	if (!image.isNull()) {
		QPainter p(this);
		p.scale(getScale(), getScale());
		p.drawImage(0, 0, image);
	}
}

bool ImageWidget::gestureEvent(QGestureEvent *event)
{
	if (QGesture *pinch = event->gesture(Qt::PinchGesture))
		pinchTriggered(static_cast<QPinchGesture*>(pinch));
	return true;
}

void ImageWidget::pinchTriggered(QPinchGesture *gesture)
{
	QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
	if (changeFlags & QPinchGesture::ScaleFactorChanged) {
		qreal value = gesture->property("scaleFactor").toReal();
		currentStepScaleFactor = value;
	}
	if (gesture->state() == Qt::GestureFinished)
		setScale(scaleFactor * currentStepScaleFactor);
	updateSize();
	update();
	emit scaleChanged(getScale());
}
