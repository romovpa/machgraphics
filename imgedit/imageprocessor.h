#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>
#include <QImage>
#include <QRect>

class ImageProcessor : public QThread
{
    Q_OBJECT
public:
	ImageProcessor(const QImage &image, QRect rect, QObject *parent = 0);
	QImage getImage();

protected:
	virtual void process();

	void run();
	void updateImage();

	void setProgress(int);
	void setProgress(int volume, int done);
	void setProgress(double);

	QImage image;
	QRect rect;

signals:
	void busyChanged(bool);
	void progressChanged(int);
	void imageChanged(QImage);

};

#endif // IMAGEPROCESSOR_H
