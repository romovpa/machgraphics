#include "editorwindow.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	(new EditorWindow())->show();

    return a.exec();
}
