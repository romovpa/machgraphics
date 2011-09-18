#include "editorwindow.h"

#include <QtGui>

EditorWindow::EditorWindow(QWidget *parent) :
	QMainWindow(parent)
{
	imageWidget = new ImageWidget;
	imageWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageWidget->setBackgroundRole(QPalette::Base);

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setWidget(imageWidget);

	setCentralWidget(scrollArea);

	createActions();
	createMenus();

	setWindowTitle( tr("Image Editor") );
	resize(600, 400);
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::openImage()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), QDir::homePath());
	if (!fileName.isEmpty()) {
		QImage image(fileName);
		if (image.isNull()) {
			QMessageBox::warning(this, tr("Image Editor"),
				tr("Cannot load %1.").arg(fileName));
			return;
		}
		imageFile = fileName;
		imageWidget->setImage(image);
	}
}

void EditorWindow::saveImage()
{
	bool success = imageWidget->getImage().save(imageFile);
	if (!success) {
		QMessageBox::warning(this, tr("Image Editor"),
			tr("Cannot save image to %1.").arg(imageFile));
	}
}

void EditorWindow::saveImageAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Image"), QDir::homePath());
	if (!fileName.isEmpty())
		imageFile = fileName;
	saveImage();
}

void EditorWindow::createActions()
{
	openAct = new QAction(tr("&Open"), this);
	openAct->setShortcut(QKeySequence::Open);
	connect(openAct, SIGNAL(triggered()), this, SLOT(openImage()));

	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcut(QKeySequence::Save);
	connect(saveAct, SIGNAL(triggered()), this, SLOT(saveImage()));

	saveAsAct = new QAction(tr("Save As"), this);
	saveAsAct->setShortcut(QKeySequence::SaveAs);
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveImageAs()));

	quitAct = new QAction(tr("Quit"), this);
	quitAct->setShortcut(QKeySequence::Quit);
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void EditorWindow::updateActions()
{

}

void EditorWindow::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);

	menuBar()->addMenu(fileMenu);
}
