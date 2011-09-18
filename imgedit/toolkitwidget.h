#ifndef TOOLKITWIDGET_H
#define TOOLKITWIDGET_H

#include <QWidget>

namespace Ui {
    class ToolkitWidget;
}

class ToolkitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolkitWidget(QWidget *parent = 0);
    ~ToolkitWidget();

private:
    Ui::ToolkitWidget *ui;
};

#endif // TOOLKITWIDGET_H
