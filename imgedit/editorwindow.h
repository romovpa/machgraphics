#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "imagewidget.h"

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

	QString imageFile;

	QScrollArea *scrollArea;
	ImageWidget *imageWidget;
	QDockWidget *dock;

	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	QAction *quitAct;

	QMenu *fileMenu;
};

#endif // EDITORWINDOW_H
