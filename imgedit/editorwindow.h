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

	QAction *autoContrastAct;

	QMenu *fileMenu;
	QMenu *procMenu;
};

#endif // EDITORWINDOW_H
