#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "imagewidget.h"
#include "toolkitwidget.h"

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

private:
	void createActions();
	void updateActions();
	void createMenus();
	void createToolkit();

	QString imageFile;

	QScrollArea *scrollArea;
	ImageWidget *imageWidget;

	QDockWidget *toolkitDock;
	ToolkitWidget *toolkitWidget;

	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *quitAct;
	QAction *toggleToolkitAct;

	QMenu *fileMenu;
	QMenu *windowMenu;
};

#endif // EDITORWINDOW_H
