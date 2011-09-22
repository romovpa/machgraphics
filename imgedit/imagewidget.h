#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtGui>

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);

	const QImage& getImage();
	QRect getRect();
	float getScale();

signals:
	void scaleChanged(float);

public slots:
	void setImage(const QImage &);
	void setScale(float);

protected:
	bool event(QEvent *);
	void paintEvent(QPaintEvent *);

private:
	void updateSize();

	bool gestureEvent(QGestureEvent *);
	void pinchTriggered(QPinchGesture *);

	QImage image;

	float scaleFactor;
	float currentStepScaleFactor;
};

#endif // IMAGEWIDGET_H
