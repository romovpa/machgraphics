#include "toolkitwidget.h"
#include "ui_toolkitwidget.h"

ToolkitWidget::ToolkitWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolkitWidget)
{
    ui->setupUi(this);
}

ToolkitWidget::~ToolkitWidget()
{
    delete ui;
}
