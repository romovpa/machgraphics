#ifndef TOOLKITWIDGET_H
#define TOOLKITWIDGET_H

#include "imageprocessor.h"

#include <QtGui>

namespace Ui {
    class ToolkitWidget;
}

class EditorWindow;

class ToolkitWidget : public QWidget
{
    Q_OBJECT

public:
	explicit ToolkitWidget(EditorWindow*, QWidget *parent = 0);
    ~ToolkitWidget();

private:
	void startProcessor(ImageProcessor*);

	Ui::ToolkitWidget *ui;
	EditorWindow *editor;

// --- Noise tool
private:
	void initNoiseTool();
	double getNoiseSigma();

	int noiseType;
	double noiseSigmaMin, noiseSigmaMax;

private slots:
	void changeNoiseType(int);
	void changeNoiseSigma();
	void startAddingNoise();



};

#endif // TOOLKITWIDGET_H
