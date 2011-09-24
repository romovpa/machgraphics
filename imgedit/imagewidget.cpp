#include "imagewidget.h"

#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent) :
	QWidget(parent),
	currentStepScaleFactor(1),
	scaleFactor(1),
	wheelZoom(false)
{
	grabGesture(Qt::PinchGesture);
}

const QImage& ImageWidget::getImage()
{
	return image;
}

QRect ImageWidget::getRect()
{
	if (rect.isValid())
		return rect;
	else
		return image.rect();
}

float ImageWidget::getScale()
{
	return scaleFactor * currentStepScaleFactor;
}

void ImageWidget::setImage(const QImage &newImage)
{
	image = newImage;
	rect.setSize(QSize(0, 0));
	updateSize();
	update();
}

bool ImageWidget::hasImage()
{
	return !image.isNull();
}

void ImageWidget::setScale(float scale)
{
	scaleFactor = scale;
	currentStepScaleFactor = 1;
	updateSize();
	update();
	emit zoomOutAvailableChanged(scaleFactor - ZOOM_SCALE_STEP > ZOOM_MIN_SCALE);
}

void ImageWidget::setWheelZoom(bool value)
{
	this->wheelZoom = value;
}

void ImageWidget::zoomIn()
{
	setScale(getScale() + ZOOM_SCALE_STEP);
}

void ImageWidget::zoomOut()
{
	if (getScale() - ZOOM_SCALE_STEP > ZOOM_MIN_SCALE)
		setScale(getScale() - ZOOM_SCALE_STEP);
}

void ImageWidget::zoomOriginal()
{
	setScale(1);
}

void ImageWidget::updateSize()
{
	int w = ceil(float(image.width()) * getScale());
	int h = ceil(float(image.height()) * getScale());
	setFixedSize(w, h);
}

QPoint ImageWidget::getImagePos(const QPoint &p)
{
	return QPoint(p.x()/getScale(), p.y()/getScale());
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
		if (rect.isValid()) {
			p.setPen(Qt::SolidLine);
			p.setPen(Qt::red);
			p.drawRect(rect.left(), rect.top(), rect.width()-1, rect.height()-1);
		}
	}
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
	if (wheelZoom) {
		float scale = getScale();
		scale += event->delta() * ZOOM_SCALE_WHEEL_STEP;
		if (scale <	ZOOM_MIN_SCALE)
			scale = ZOOM_MIN_SCALE;
		setScale(scale);
	}
	else
		QWidget::wheelEvent(event);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
	selectingRect = true;
	selectingPoint = getImagePos(event->pos());
	rect.setSize(QSize(0, 0));
	update();
}

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (selectingRect) {
		QPoint p1 = selectingPoint, p2 = getImagePos(event->pos());
		// set selecton rect
		rect.setCoords(min(p1.x(), p2.x()),
					   min(p1.y(), p2.y()),
					   max(p1.x(), p2.x()),
					   max(p1.y(), p2.y()));
		// correcting rect
		rect.setBottom( min(image.height(), rect.bottom()) );
		rect.setTop( max(0, rect.top()) );
		rect.setLeft( max(0, rect.left()) );
		rect.setRight( min(image.width(), rect.right()) );

		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	selectingRect = false;
	update();
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
