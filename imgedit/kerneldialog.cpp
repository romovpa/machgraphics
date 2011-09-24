#include "kerneldialog.h"
#include "ui_kerneldialog.h"

#include <QtGui>
#include <cstdio>

KernelDialog::KernelDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::KernelDialog),
	kernel(0)
{
    ui->setupUi(this);
	kernel = new double[ui->colsBox->maximum() * ui->rowsBox->maximum()];
	changeMatrixSize();
}

KernelDialog::~KernelDialog()
{
    delete ui;
}

double* KernelDialog::getKernel()
{
	return kernel;
}

int KernelDialog::getKernelRows()
{
	return ui->rowsBox->value();
}

int KernelDialog::getKernelCols()
{
	return ui->colsBox->value();
}

void KernelDialog::changeMatrixSize()
{
	cols = ui->colsBox->value();
	rows = ui->rowsBox->value();
	ui->kernelTable->setColumnCount(cols);
	ui->kernelTable->setRowCount(rows);
	updateMatrix();
}

void KernelDialog::changeNormalizeFactor()
{
	updateMatrix();
	double sum = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			QTableWidgetItem *item = ui->kernelTable->item(i, j);
			double value = 0;
			if (item) {
				bool correct;
				value = item->text().toDouble(&correct);
				if (correct)
					sum += value;
			}
		}
	}
	if (abs(sum) > 1e-5)
		ui->scaleFactorBox->setValue(1.0/sum);
	else
		ui->scaleFactorBox->setValue(1);
	updateMatrix();
}

void KernelDialog::updateMatrix()
{
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			QTableWidgetItem *item = ui->kernelTable->item(i, j);
			double value = 0;
			if (item) {
				bool correct;
				value = item->text().toDouble(&correct);
				if (!correct)
					value = 0;
			}
			kernel[cols*(rows-1-i) + (cols-1-j)] = value * ui->scaleFactorBox->value();
		}
	}
}
