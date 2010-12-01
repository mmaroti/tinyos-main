#ifndef SDOWNLOADWIDGET_H
#define SDOWNLOADWIDGET_H

#include <QWidget>
#include <QVarLengthArray>

class Application;

namespace Ui {
    class SDownloadWidget;
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

class SDownloadWidget : public QWidget {
    Q_OBJECT
public:
    SDownloadWidget(QWidget *parent, Application &app);
    ~SDownloadWidget();

    const SData getSDataAt(int i) const { return records[i]; }
    int size() const { return records.size(); }

private:
    Ui::SDownloadWidget *ui;
    Application &application;

    void setFlatFileModel(const QString &filename);
    QWidget setupListTab();

    void fillSData();
    void init();
    QVarLengthArray<SData> records;


private slots:
    void on_downloadButton_clicked();
    void on_clearButton_clicked();
};

#endif // SDOWNLOADWIDGET_H
