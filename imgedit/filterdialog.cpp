#include "filterdialog.h"
#include "ui_filterdialog.h"

#include "filterprocessor.h"

#include <QtGui>

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
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

int FilterDialog::getType()
{
	return ui->typeBox->currentIndex();
}

void FilterDialog::changeType(int type)
{
	switch (type) {
		case FilterProcessor::GAUSSIAN:
		case FilterProcessor::UNSHARP:
			ui->sizeBox->setEnabled(true);
			ui->sigmaBox->setEnabled(true);
			break;
		case FilterProcessor::MEDIAN:
			ui->sizeBox->setEnabled(true);
			ui->sigmaBox->setEnabled(false);
		case FilterProcessor::CONVOLUTION:
			ui->sizeBox->setEnabled(false);
			ui->sigmaBox->setEnabled(false);
		default:
			qWarning() << "FilterDialog: invalid filter type";

	}
}
