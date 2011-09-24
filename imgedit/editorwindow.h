#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "imagewidget.h"
#include "progressdialog.h"
#include "imageprocessor.h"

#include <QtGui>

class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();

private slots:
	void openImage();
	void saveImage();
	void saveImageAs();

	void doAutoContrast();
	void doAutoLevels();
	void doWhiteBalance();
	void doGeometryTransform();
	void doFilter();
	void doConvolution();

private:
	void createActions();
	void updateActions();
	void createMenus();

	bool runProcessor(ImageProcessor &processor);

	QString imageFile;

	QScrollArea *scrollArea;
	ImageWidget *imageWidget;
	ProgressDialog *progressDialog;

	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *quitAct;

	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *zoomOriginalAct;
	QAction *zoomByWheelAct;

	QAction *autoContrastAct; // Linear stretch on L (luma)
	QAction *autoLevelsAct;   // Linear stretch on RGB
	QAction *whiteBalanceAct; // White balancing (grayworld)
	QAction *filterAct;       // Open filtration dialog
							   //    support: gauss (separable), sharpeness, median, arbitrary linear
	QAction *convolutionAct;  // Convolution with arbitrary kernel
	QAction *geometryAct;     // Open geometry transform dialog

	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *procMenu;
};

#endif // EDITORWINDOW_H
