#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>
#include <QImage>
#include <QRect>

class ImageProcessor : public QThread
{
    Q_OBJECT
public:
	explicit ImageProcessor(const QImage &image, QRect rect, QObject *parent = 0);
	QImage getImage();

signals:
	void processStarted();
	void progressChanged(double);
	void processFinished();

protected:
	virtual void process();

	void run();
	void progress(double);

	QImage image;
	QRect rect;

};

int double2int(double x);

#endif // IMAGEPROCESSOR_H
