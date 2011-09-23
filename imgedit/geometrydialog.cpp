#include "geometrydialog.h"
#include "ui_geometrydialog.h"

#include "geometryprocessor.h"

#include <QColorDialog>

GeometryDialog::GeometryDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::GeometryDialog),
	fillStyle(GeometryProcessor::FILL_BY_OLD_IMAGE)
{
    ui->setupUi(this);
	ui->scaleBox->setRange(0.1, 1000);
}

GeometryDialog::~GeometryDialog()
{
    delete ui;
}

double GeometryDialog::getScale()
{
	return ui->scaleBox->value();
}

double GeometryDialog::getAngle()
{
	return ui->angleBox->value();
}

int GeometryDialog::getFillStyle()
{
	return fillStyle;
}

QRgb GeometryDialog::getFillColor()
{
	return fillColor;
}

void GeometryDialog::changeFillStyle(int style)
{
	fillStyle = style;
	if (fillStyle == GeometryProcessor::FILL_BY_COLOR)
		fillColor = QColorDialog::getColor(fillColor, this, "Image Editor").rgb();
}
