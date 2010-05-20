#ifndef CALIBRATIONMODULE_H
#define CALIBRATIONMODULE_H
#define WINDOW 100  //data window size, for finding idle shimmer positions
#define MAXDIFF 20  //maximum difference between max and min values from an idle mote
#define xN   0.95*xMinAvg+0.05*xMaxAvg  //region borders for classifying idle windows
#define xZ_L 0.55*xMinAvg+0.45*xMaxAvg  //N = NEGATIVE_Gravity STATE
#define xZ_U 0.45*xMinAvg+0.55*xMaxAvg  //Z = ZERO_Gravity STATE
#define xP   0.05*xMinAvg+0.95*xMaxAvg  //P = POSITIVE_Gravity STATE
#define yN   0.95*yMinAvg+0.05*yMaxAvg  //L = lower border; U = upper border;
#define yZ_L 0.55*yMinAvg+0.45*yMaxAvg  //x,y,z : axis
#define yZ_U 0.45*yMinAvg+0.55*yMaxAvg
#define yP   0.05*yMinAvg+0.95*yMaxAvg
#define zN   0.95*zMinAvg+0.05*zMaxAvg
#define zZ_L 0.55*zMinAvg+0.45*zMaxAvg
#define zZ_U 0.45*zMinAvg+0.55*zMaxAvg
#define zP   0.05*zMinAvg+0.95*zMaxAvg
#define GRAV 9.81                       //gravitational acceleration

#include <QObject>
#include "Application.h"
#include <jama_qr.h>

class Application;

struct IdleWindow
{
        IdleWindow();
        QString toString() const;

        int xMin;
        int xMax;
        double xAvg;
        int yMin;
        int yMax;
        double yAvg;
        int zMin;
        int zMax;
        double zAvg;
};

class CalibrationModule : public QObject {
    Q_OBJECT
public:
    CalibrationModule( Application &app );
    ~CalibrationModule();

    QString Calibrate();
    QString Classify();
    QString LSF();
    void saveCalibratedData(QString);
    void printMatrix1D(TNT::Array1D<double>);
    void printMatrix2D(TNT::Array2D<double>);

    const QVarLengthArray<IdleWindow> & getIdleWindows() const {
            return idleWindows;
    }

    int size() const {
            return idleWindows.size();
    }

    const IdleWindow & at(int i) const {
            return idleWindows[i];
    }

    QString atToString(int i) const {
            return idleWindows[i].toString();
    }

    void clearWindows();

    const int & atIdleSides(int i) const {
            return idleSidesMins[i];
    }

    void clearIdleSides();

private:
    Application &application;

    int xMin, xMax, yMin, yMax, zMin, zMax, xDiff,yDiff,zDiff;
    float xAvg, yAvg, zAvg;
    float xMinAvg, xMaxAvg, yMinAvg, yMaxAvg, zMinAvg, zMaxAvg;
    long xSum, ySum, zSum;
    QVarLengthArray<IdleWindow> idleWindows;
    int idleSidesMins[6];
    double calibrationData[12];
};

#endif // CALIBRATIONMODULE_H
