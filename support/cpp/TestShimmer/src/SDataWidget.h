#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>

class Application;

namespace Ui {
    class SDataWidget;
}

class SDataWidget : public QWidget {
    Q_OBJECT
public:
    SDataWidget(QWidget *parent, Application &app);
    ~SDataWidget();

private:
    Ui::SDataWidget *ui;
    Application &application;

private slots:
    void on_itemSelectionChanged();
};

#endif // SDATAWIDGET_H
