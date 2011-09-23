#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

namespace Ui {
    class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = 0);
    ~FilterDialog();

	int getType();
	int getKernelSize();
	double getSigma();

private slots:
	void changeType(int);

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H
