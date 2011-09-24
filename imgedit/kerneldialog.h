#ifndef KERNELDIALOG_H
#define KERNELDIALOG_H

#include <QDialog>

namespace Ui {
    class KernelDialog;
}

class KernelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KernelDialog(QWidget *parent = 0);
    ~KernelDialog();

	double* getKernel();
	int getKernelRows();
	int getKernelCols();

public slots:
	void changeMatrixSize();
	void changeNormalizeFactor();
	void updateMatrix();

private:
    Ui::KernelDialog *ui;


	int rows, cols;
	double *kernel;
};

#endif // KERNELDIALOG_H
