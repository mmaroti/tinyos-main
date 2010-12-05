#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>
#include <QVarLengthArray>
#include <QTreeWidgetItem>
#include <QTreeWidget>

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

    const SData& getSDataAt(int i) const { return records[i]; }
    int getRecordsSize() const { return records.size(); }

    void msleep(unsigned long msecs);

private:
    Ui::SDataWidget *ui;
    Application &application;

    void initLeft();
    void initRight(QVarLengthArray<int> list);
    QVarLengthArray<int> getLinkingRecords(int moteId, int num);

    QTreeWidgetItem* createParentItem(int i, QTreeWidget* root);
    void createChildItem(int i, QTreeWidgetItem* parent);

    void fillSData();
    void init();
    QVarLengthArray<SData> records;

signals:
    void downloadStarted();
    void downloadFinished();

private slots:
    void on_itemSelectionChanged();
    void on_toPlotButton_clicked();
    void on_downloadButton_clicked();
    void on_downloadStarted();
    void on_downloadFinished();
    void on_clearButton_clicked();
};

#endif // SDATAWIDGET_H
