#ifndef PERIODICALCALIBRATIONMODULE_H
#define PERIODICALCALIBRATIONMODULE_H
#define MAXDIFF 20
#define NUMOFROT 10.0

#include <QObject>
#include "Application.h"

class Application;

struct IdleGyroWindow {
    IdleGyroWindow();
    QString toString() const;

    int size;
    float numOfRotations;
    double alfaSum;
    long Xint, Yint, Zint;
};

class PeriodicalCalibrationModule : public QObject {
    Q_OBJECT
public:
    PeriodicalCalibrationModule( Application &app, CalibrationModule &calMod );
    ~PeriodicalCalibrationModule();

    QString Calibrate(QString rotAxis);
    QString SVD();

    double getCalibratedData(int time, QString axis);
    double calculateAngle(double accel1, double accel2);

    QString printSettingsToConsole(QString settingsData);
    void ClearWindows();
private:
    Application &application;
    CalibrationModule &calibrationModule;

    double gyroMinAvgs[3];
    QVector<IdleGyroWindow> idleGyroWindows;
    double gyroCalibrationData[12];
};

#endif // PERIODICALCALIBRATIONMODULE_H
