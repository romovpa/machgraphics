#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent)
{
	// layout
	progressBar = new QProgressBar(this);
	cancelButton = new QPushButton(tr("Cancel"), this);
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(progressBar);
	layout->addWidget(cancelButton);
	setLayout(layout);

	// connections
	connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelRequested()));

	// window
	setWindowTitle( tr("Image Processing") );
	setWindowFlags(Qt::Dialog);
	setMinimumWidth(300);
	setModal(true);
	setVisible(false);
	// TODO: deny resizing
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
}

void ProgressDialog::setProgress(int value)
{
	progressBar->setValue(value);
}

void ProgressDialog::setProgress(double value)
{
	progressBar->setValue((int)(100*value+0.5));
}


