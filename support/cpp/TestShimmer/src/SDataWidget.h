#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>
#include <QVarLengthArray>

class Application;

namespace Ui {
    class SDataWidget;
}

struct SData
{
    SData();
    int moteID;
    int num;
    int length;
    QString tor;
    QString tod;
};

class SDataWidget : public QWidget {
    Q_OBJECT
public:
    SDataWidget(QWidget *parent, Application &app);
    ~SDataWidget();

private:
    Ui::SDataWidget *ui;
    Application &application;

    void getLinkingRecords(int moteId, int num);

    QVarLengthArray<SData> records;

private slots:
    void on_itemSelectionChanged();
};

#endif // SDATAWIDGET_H
