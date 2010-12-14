#ifndef SDATAWIDGET_H
#define SDATAWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QVarLengthArray>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include "DownloadManager.hpp"

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

void fillSData(QVarLengthArray<SData>& records);

class SDataWidget : public QWidget {
    Q_OBJECT
public:
    SDataWidget(QWidget *parent, Application &app);
    ~SDataWidget();

    const SData& getSDataAt(int i) const { return records[i]; }
    int getRecordsSize() const { return records.size(); }

private:
    Ui::SDataWidget *ui;
    Application &application;

    QMessageBox* blockingBox;
    sdc::DownloadManager manager;
    QVarLengthArray<SData> records;
    QVarLengthArray<SData> filteredRecords;
    volatile bool downloadFailed;
    QString errorMsg;

    void initLeft();
    void initRight(QVarLengthArray<int> list);
    QVarLengthArray<int> getLinkingRecords(int moteId, int num);

    QTreeWidgetItem* createParentItem(int i, QTreeWidget* root);
    void createChildItem(int i, QTreeWidgetItem* parent);

    void init();

    void processBinaryFile(const QString& dialogCaption, const QString& startFromHere, const QString& blockTitle);
    void downloadFromDevice();
    void showBlockingBox(const QString& title, const QString& text);

    const QString selectWin32Device();
    const QString selectBinaryFile(const QString& caption, const QString& startFromHere);

    void printRecords();
    void filterRecords();

signals:

    void updateGUI();

private slots:

    void onItemSelectionChanged();
    void on_toPlotButton_clicked();
    void on_downloadButton_clicked();
    void onItemDoubleClicked(QTreeWidgetItem*,int);
    void on_fileButton_clicked();
    void onDownloadFinished(bool error, const QString& error_msg, const QVarLengthArray<SData>& data);
    void onUpdateGUI();
    void on_clearButton_clicked();
};

#endif // SDATAWIDGET_H
