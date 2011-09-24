#include "filterdialog.h"
#include "ui_filterdialog.h"

#include "filterprocessor.h"

#include <QtGui>

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
	changeType(ui->typeBox->currentIndex());
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

int FilterDialog::getKernelSize()
{
	return ui->sizeBox->value();
}

double FilterDialog::getSigma()
{
	return ui->sigmaBox->value();
}

int FilterDialog::getRadius()
{
	return ui->radiusBox->value();
}

int FilterDialog::getType()
{
	return ui->typeBox->currentIndex();
}

void FilterDialog::changeType(int type)
{
	bool sigma = false, alpha = false, size = false, radius = false;
	bool matrix = false;
	switch (type) {
		case FilterProcessor::UNSHARP:
			alpha = true;
		case FilterProcessor::GAUSSIAN:
			size = true, sigma = true;
			break;
		case FilterProcessor::MEDIAN:
			radius = true;
			break;
		case FilterProcessor::GLASS_EFFECT:
			radius = true;
		case FilterProcessor::CONVOLUTION:
			matrix = true;
			break;
		default:
			qWarning() << "FilterDialog: invalid filter type";
	}

	ui->sigmaBox->setVisible(sigma);
	ui->sigmaLabel->setVisible(sigma);
	ui->alphaBox->setVisible(alpha);
	ui->alphaLabel->setVisible(alpha);
	ui->sizeBox->setVisible(size);
	ui->sizeLabel->setVisible(size);
	ui->radiusBox->setVisible(radius);
	ui->radiusLabel->setVisible(radius);
}
