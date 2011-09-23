#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtGui>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);
	const QImage& getImage();
	bool hasImage();
	float getScale();
	QRect getRect();

signals:
	void scaleChanged(float);

public slots:
	void setImage(const QImage &);
	void setScale(float);

protected:
	bool event(QEvent *);
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);

private:
	void updateSize();

	bool gestureEvent(QGestureEvent *);
	void pinchTriggered(QPinchGesture *);

	QImage image;
	QRect rect;

	float scaleFactor;
	float currentStepScaleFactor;

	bool selectingRect;
	QPoint selectingPoint;
	QPoint getImagePos(const QPoint &);
};

#endif // IMAGEWIDGET_H
