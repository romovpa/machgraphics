#ifndef NOISEPROCESSOR_H
#define NOISEPROCESSOR_H

#include "imageprocessor.h"

class NoiseProcessor : public ImageProcessor
{
    Q_OBJECT

public:
	NoiseProcessor(const QImage&, QRect, QObject *parent = 0);

	enum NoiseType {
		GAUSSIAN = 0,
		RANDOM = 1,
		SALTPEPER = 2
	};

	void setType(int);
	void setSigma(double);

protected:
	void process();

private:
	double sigma;
	int type;
};

#endif // NOISEPROCESSOR_H
