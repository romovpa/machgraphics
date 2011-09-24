#include "editorwindow.h"
#include "geometrydialog.h"
#include "filterdialog.h"
#include "kerneldialog.h"

#include "imageprocessor.h"
#include "histogramprocessor.h"
#include "geometryprocessor.h"
#include "filterprocessor.h"

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
	updateActions();

	progressDialog = new ProgressDialog(this);

	setWindowTitle( tr("Image Editor") );
	resize(600, 500);
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
		image = image.convertToFormat(QImage::Format_RGB32);
		imageFile = fileName;
		imageWidget->setImage(image);
		updateActions();
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

	zoomInAct = new QAction(tr("Zoom In"), this);
	zoomInAct->setShortcut(QKeySequence::ZoomIn);
	connect(zoomInAct, SIGNAL(triggered()), imageWidget, SLOT(zoomIn()));

	zoomOutAct = new QAction(tr("Zoom Out"), this);
	zoomOutAct->setShortcut(QKeySequence::ZoomOut);
	connect(zoomOutAct, SIGNAL(triggered()), imageWidget, SLOT(zoomOut()));
	connect(imageWidget, SIGNAL(zoomOutAvailableChanged(bool)), zoomOutAct, SLOT(setEnabled(bool)));

	zoomOriginalAct = new QAction(tr("Actual Size"), this);
	connect(zoomOriginalAct, SIGNAL(triggered()), imageWidget, SLOT(zoomOriginal()));

	zoomByWheelAct = new QAction(tr("Zoom by Mouse Wheel"), this);
	zoomByWheelAct->setCheckable(true);
	zoomByWheelAct->setChecked(false);
	connect(zoomByWheelAct, SIGNAL(toggled(bool)), imageWidget, SLOT(setWheelZoom(bool)));

	autoContrastAct = new QAction(tr("Auto Contrast"), this);
	autoContrastAct->setToolTip(tr("Apply luminance histogram stretching"));
	connect(autoContrastAct, SIGNAL(triggered()), this, SLOT(doAutoContrast()));

	autoLevelsAct = new QAction(tr("Auto Levels"), this);
	autoLevelsAct->setToolTip(tr("Apply channel-wise RGB histogram stretching"));
	connect(autoLevelsAct, SIGNAL(triggered()), this, SLOT(doAutoLevels()));

	whiteBalanceAct = new QAction(tr("Correct White Balance"), this);
	whiteBalanceAct->setToolTip(tr("Apply white balance correction using greyworld model"));
	connect(whiteBalanceAct, SIGNAL(triggered()), this, SLOT(doWhiteBalance()));

	geometryAct = new QAction(tr("Scale/Rotate"), this);
	geometryAct->setToolTip(tr("Scale and rotate image relative to the center"));
	connect(geometryAct, SIGNAL(triggered()), this, SLOT(doGeometryTransform()));

	filterAct = new QAction(tr("Filters && Effects"), this);
	filterAct->setToolTip(tr("Open filtration dialog"));
	connect(filterAct, SIGNAL(triggered()), this, SLOT(doFilter()));

	convolutionAct = new QAction(tr("Convolution"), this);
	convolutionAct->setToolTip(tr("Apply convolution with arbitrary kernel"));
	connect(convolutionAct, SIGNAL(triggered()), this, SLOT(doConvolution()));
}

void EditorWindow::updateActions()
{
	bool hasImg = imageWidget->hasImage();
	autoContrastAct->setEnabled(hasImg);
	autoLevelsAct->setEnabled(hasImg);
	whiteBalanceAct->setEnabled(hasImg);
	geometryAct->setEnabled(hasImg);
	filterAct->setEnabled(hasImg);
	convolutionAct->setEnabled(hasImg);
	zoomInAct->setEnabled(hasImg);
	zoomOutAct->setEnabled(hasImg);
	zoomOriginalAct->setEnabled(hasImg);
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

	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(zoomOriginalAct);
	viewMenu->addSeparator();
	viewMenu->addAction(zoomByWheelAct);
	menuBar()->addMenu(viewMenu);

	procMenu = new QMenu(tr("&Processing"), this);
	procMenu->addAction(autoContrastAct);
	procMenu->addAction(autoLevelsAct);
	procMenu->addAction(whiteBalanceAct);
	procMenu->addSeparator();
	procMenu->addAction(geometryAct);
	procMenu->addAction(filterAct);
	procMenu->addAction(convolutionAct);
	menuBar()->addMenu(procMenu);
}

bool EditorWindow::runProcessor(ImageProcessor &processor)
{
	// TODO: show progress window after 0.5 seconds the processor started
	connect(&processor, SIGNAL(progressChanged(double)), progressDialog, SLOT(setProgress(double)));
	connect(progressDialog, SIGNAL(cancelRequested()), &processor, SLOT(terminate()));
	connect(&processor, SIGNAL(terminated()), progressDialog, SLOT(reject()));
	connect(&processor, SIGNAL(processFinished()), progressDialog, SLOT(accept()));
	processor.start();
	if (progressDialog->exec() == QDialog::Accepted) {
		imageWidget->setImage(processor.getImage());
		return true;
	}
	else
		return false;
}

void EditorWindow::doAutoContrast()
{
	HistogramProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
	processor.setType(HistogramProcessor::LUMA_LINEAR_STRETCH);
	runProcessor(processor);
}

void EditorWindow::doAutoLevels()
{
	HistogramProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
	processor.setType(HistogramProcessor::RGB_LINEAR_STRETCH);
	runProcessor(processor);
}

void EditorWindow::doWhiteBalance()
{
	HistogramProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
	processor.setType(HistogramProcessor::GREYWORLD_WHITE_BALANCE);
	runProcessor(processor);
}

void EditorWindow::doGeometryTransform()
{
	GeometryDialog dlg;
	int res = dlg.exec();
	if (res == QDialog::Accepted) {
		GeometryProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
		processor.setScale(dlg.getScale());
		processor.setAngle(dlg.getAngle());
		processor.setFillStyle(dlg.getFillStyle());
		processor.setFillColor(dlg.getFillColor());
		runProcessor(processor);
	}
}

void EditorWindow::doFilter()
{
	FilterDialog dlg;
	int res = dlg.exec();
	if (res == QDialog::Accepted) {
		FilterProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
		processor.setType(dlg.getType());
		processor.setKernelSize(dlg.getKernelSize());
		processor.setSigma(dlg.getSigma());
		processor.setRadius(dlg.getRadius());
		runProcessor(processor);
	}
}

void EditorWindow::doConvolution()
{
	KernelDialog dlg;
	int res = dlg.exec();
	if (res == QDialog::Accepted) {
		FilterProcessor processor(imageWidget->getImage(), imageWidget->getRect(), this);
		processor.setType(FilterProcessor::CONVOLUTION);
		processor.setKernel(dlg.getKernel(), dlg.getKernelRows(), dlg.getKernelCols());
		runProcessor(processor);
	}
}
