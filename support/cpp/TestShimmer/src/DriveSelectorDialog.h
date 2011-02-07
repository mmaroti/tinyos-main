#ifndef DRIVESELECTORDIALOG_H
#define DRIVESELECTORDIALOG_H

#include <QDialog>

namespace Ui{
    class DriveSelectorDialog;
}

class DriveSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    DriveSelectorDialog(QWidget*);

    const QString getDrive() const { return drive; }

private slots:
    void on_driveList_itemSelectionChanged();

private:
    Ui::DriveSelectorDialog *ui;
    QString drive;
};

#endif // DRIVESELECTORDIALOG_H
