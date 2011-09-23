#ifndef HISTOGRAMPROCESSOR_H
#define HISTOGRAMPROCESSOR_H

#include "imageprocessor.h"

class HistogramProcessor : public ImageProcessor
{
    Q_OBJECT
public:
	explicit HistogramProcessor(const QImage &, QRect, QObject *parent = 0);

	enum HistogramTransform {
		RGB_LINEAR_STRETCH,
		LUMA_LINEAR_STRETCH,
		GREYWORLD_WHITE_BALANCE
	};

	void setType(HistogramTransform type);

protected:
	void process();

private:
	int type;

	void rgbStretch();
	void lumaStretch();
	void greyworld();
};

#endif // HISTOGRAMPROCESSOR_H
