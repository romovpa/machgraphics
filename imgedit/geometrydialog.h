#ifndef GEOMETRYDIALOG_H
#define GEOMETRYDIALOG_H

#include <QDialog>
#include <QRgb>

namespace Ui {
    class GeometryDialog;
}

class GeometryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GeometryDialog(QWidget *parent = 0);
    ~GeometryDialog();

	double getScale();
	double getAngle();
	int getFillStyle();
	QRgb getFillColor();

private slots:
	void changeFillStyle(int);

private:
    Ui::GeometryDialog *ui;

	int fillStyle;
	QRgb fillColor;
};

#endif // GEOMETRYDIALOG_H
