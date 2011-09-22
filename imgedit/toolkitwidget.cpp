#include "toolkitwidget.h"
#include "ui_toolkitwidget.h"
#include "editorwindow.h"

#include "noiseprocessor.h"

#include <QtGui>

ToolkitWidget::ToolkitWidget(EditorWindow *editor, QWidget *parent) :
    QWidget(parent),
	ui(new Ui::ToolkitWidget),
	editor(editor)
{
    ui->setupUi(this);

	// Init tools
	initNoiseTool();
}

ToolkitWidget::~ToolkitWidget()
{
    delete ui;
}

void ToolkitWidget::startProcessor(ImageProcessor *processor)
{
	connect(processor, SIGNAL(imageChanged(QImage)), editor, SLOT(changeImage(QImage)));
	connect(processor, SIGNAL(busyChanged(bool)), ui->toolBox, SLOT(setDisabled(bool)));
	processor->start();
}

// ------------------- NOISE TOOL -------------------

void ToolkitWidget::initNoiseTool() {
	changeNoiseType(NoiseProcessor::GAUSSIAN);
}

double ToolkitWidget::getNoiseSigma()
{
	return (double)(ui->noiseSigmaSlider->value() - ui->noiseSigmaSlider->minimum()) /
			(double)(ui->noiseSigmaSlider->maximum() - ui->noiseSigmaSlider->minimum()) *
			(noiseSigmaMax - noiseSigmaMin) + noiseSigmaMin;
}

void ToolkitWidget::changeNoiseType(int type)
{
	this->noiseType = type;
	switch (type) {

		case NoiseProcessor::GAUSSIAN:
			noiseSigmaMin = 0.1;
			noiseSigmaMax = 80;
			ui->noiseSigmaLabel->setText( tr("Sigma:") );
			break;

		case NoiseProcessor::RANDOM:
		case NoiseProcessor::SALTPEPER:
			noiseSigmaMin = 0;
			noiseSigmaMax = 1;
			ui->noiseSigmaLabel->setText( tr("Density:") );
			break;

	}
	emit changeNoiseSigma();
}

void ToolkitWidget::changeNoiseSigma()
{
	QString string;
	ui->noiseSigmaValue->setText( string.sprintf("%.2f", getNoiseSigma()) );
}

void ToolkitWidget::startAddingNoise()
{
	NoiseProcessor *processor = new NoiseProcessor(editor->getImage(), editor->getRect(), this);
	processor->setSigma(getNoiseSigma());
	processor->setType(noiseType);
	startProcessor(processor);
}


