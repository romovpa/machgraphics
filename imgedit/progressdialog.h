#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QtGui>

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

public slots:
	void setProgress(int);
	void setProgress(double);

signals:
	void cancelRequested();

protected:
	void closeEvent(QCloseEvent *);

private:
	QProgressBar *progressBar;
	QPushButton *cancelButton;

};

#endif // PROGRESSDIALOG_H
