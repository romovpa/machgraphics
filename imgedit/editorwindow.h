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

private:
	void createActions();
	void updateActions();
	void createMenus();

	bool runProcessor(ImageProcessor *processor);

	QString imageFile;

	QScrollArea *scrollArea;
	ImageWidget *imageWidget;
	ProgressDialog *progressDialog;

	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *quitAct;

	QAction *autoContrastAct; // Linear stretch on L (luma)
	QAction *rgbStretchAct;   // Linear stretch on RGB
	QAction *whiteBalanceAct; // White balancing (gray world)
	QAction *filterAct;       // Open filtration dialog
							   //    support: gauss (separable), sharpeness, median, arbitrary linear
	QAction *affineAct;       // Open affine transform dialog
	QAction *wavesAct;        // Waves effect
	QAction *glassAct;        // Glass effect

	QMenu *fileMenu;
	QMenu *procMenu;
};

#endif // EDITORWINDOW_H
