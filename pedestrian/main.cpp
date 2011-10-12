#include <QCoreApplication>
#include <QStringList>
#include <QtCore>

#include <opencv/cv.h>

#include <linear/linear.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	// parsing the settings
	QStringList args = QCoreApplication::arguments();
	for (int i = 0; i < args.size(); ++i) {

	}

	struct problem *p;
	p->bias = 1;

	cv::Mat m;

	return 0;
}
