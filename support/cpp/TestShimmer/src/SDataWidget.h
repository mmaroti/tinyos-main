#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>
#include <QVarLengthArray>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include "SDownloadWidget.h"

class Application;

namespace Ui {
    class SDataWidget;
}

class SDataWidget : public QWidget {
    Q_OBJECT
public:
    SDataWidget(QWidget *parent, Application &app, SDownloadWidget &sdata);
    ~SDataWidget();

private:
    Ui::SDataWidget *ui;
    Application &application;
    SDownloadWidget &sdownload;

    void initLeft();
    void initRight(QVarLengthArray<int> list);
    QVarLengthArray<int> getLinkingRecords(int moteId, int num);

    QTreeWidgetItem* createParentItem(int i, QTreeWidget* root);
    void createChildItem(int i, QTreeWidgetItem* parent);

private slots:
    void on_itemSelectionChanged();
    void on_toPlotButton_clicked();
};

#endif // SDATAWIDGET_H
