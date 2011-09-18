#include <QtGui/QApplication>
#include "editorwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	(new EditorWindow())->show();

    return a.exec();
}
