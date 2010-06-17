#include "PeriodicalCalibrationModule.h"
#include "CalibrationModule.h"
#include "CalibrationWidget.h"
#include "LinearEquations.h"
#include "math.h"
#include "QtDebug"

PeriodicalCalibrationModule::PeriodicalCalibrationModule(Application &app, CalibrationModule &calMod ) :
        application(app),
        calibrationModule(calMod)
{
    int size = application.settings.beginReadArray("gyroAvgsData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);

        gyroMinAvgs[i] = application.settings.value("gyroAvgsData").toDouble();
    }
    application.settings.endArray();
}

PeriodicalCalibrationModule::~PeriodicalCalibrationModule()
{

}

IdleGyroWindow::IdleGyroWindow(){
    size = 2;
    numOfRotations = 0.0;
}

QString PeriodicalCalibrationModule::Calibrate(QString rotAxis)
{
    double Xint = 0.0; double Yint = 0.0; double Zint = 0.0;
    int xGyro, yGyro, zGyro;
    double xtemp1, ytemp1, ztemp1, xtemp2, ytemp2, ztemp2, avgtemp, alfa1, alfa2, idleAxis, alfa1test, alfa2test;
    double alfaSum = 0.0;
    double SqSum = 0.0;
    int pos;

    int actSize = 2;

    for(int i = 11; i < application.dataRecorder.size(); i++){
        if ( i == 17400)
            qDebug() << "y start";
        xtemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationModule.getCalibrationDataAt(0) + application.dataRecorder.at(i-1).yAccel * calibrationModule.getCalibrationDataAt(1) + application.dataRecorder.at(i-1).zAccel * calibrationModule.getCalibrationDataAt(2) + calibrationModule.getCalibrationDataAt(9) );
        xtemp2 = (application.dataRecorder.at(i).xAccel * calibrationModule.getCalibrationDataAt(0) + application.dataRecorder.at(i).yAccel * calibrationModule.getCalibrationDataAt(1) + application.dataRecorder.at(i).zAccel * calibrationModule.getCalibrationDataAt(2) + calibrationModule.getCalibrationDataAt(9) );
        ytemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationModule.getCalibrationDataAt(3) + application.dataRecorder.at(i-1).yAccel * calibrationModule.getCalibrationDataAt(4) + application.dataRecorder.at(i-1).zAccel * calibrationModule.getCalibrationDataAt(5) + calibrationModule.getCalibrationDataAt(10) );
        ytemp2 = (application.dataRecorder.at(i).xAccel * calibrationModule.getCalibrationDataAt(3) + application.dataRecorder.at(i).yAccel * calibrationModule.getCalibrationDataAt(4) + application.dataRecorder.at(i).zAccel * calibrationModule.getCalibrationDataAt(5) + calibrationModule.getCalibrationDataAt(10) );
        ztemp1 = (application.dataRecorder.at(i-1).xAccel * calibrationModule.getCalibrationDataAt(6) + application.dataRecorder.at(i-1).yAccel * calibrationModule.getCalibrationDataAt(7) + application.dataRecorder.at(i-1).zAccel * calibrationModule.getCalibrationDataAt(8) + calibrationModule.getCalibrationDataAt(11) );
        ztemp2 = (application.dataRecorder.at(i).xAccel * calibrationModule.getCalibrationDataAt(6) + application.dataRecorder.at(i).yAccel * calibrationModule.getCalibrationDataAt(7) + application.dataRecorder.at(i).zAccel * calibrationModule.getCalibrationDataAt(8) + calibrationModule.getCalibrationDataAt(11) );

        avgtemp = sqrt( pow(xtemp2, 2.0) + pow(ytemp2, 2.0) + pow(ztemp2, 2.0) );

        if(rotAxis == "x"){
            SqSum = pow(ytemp2,2.0) + pow(ztemp2,2.0);
            idleAxis = xtemp2;
        } else if(rotAxis == "y") {
            SqSum = pow(ztemp2,2.0) + pow(xtemp2,2.0);
            idleAxis = ytemp2;
        } else if(rotAxis == "z") {
            SqSum = pow(xtemp2,2.0) + pow(ytemp2,2.0);
            idleAxis = ztemp2;
        }

        if( (avgtemp < 1.5*GRAV) && (avgtemp > 0.5*GRAV) && (fabs(idleAxis) < 0.5*GRAV) && (SqSum > 0.25*pow(GRAV,2.0)) && (SqSum < 2*pow(GRAV,2.0)) ){
            if( actSize > 20 ){
                xtemp1 = (application.dataRecorder.at(i-11).xAccel * calibrationModule.getCalibrationDataAt(0) + application.dataRecorder.at(i-11).yAccel * calibrationModule.getCalibrationDataAt(1) + application.dataRecorder.at(i-11).zAccel * calibrationModule.getCalibrationDataAt(2) + calibrationModule.getCalibrationDataAt(9) );
                xtemp2 = (application.dataRecorder.at(i-10).xAccel * calibrationModule.getCalibrationDataAt(0) + application.dataRecorder.at(i-10).yAccel * calibrationModule.getCalibrationDataAt(1) + application.dataRecorder.at(i-10).zAccel * calibrationModule.getCalibrationDataAt(2) + calibrationModule.getCalibrationDataAt(9) );
                ytemp1 = (application.dataRecorder.at(i-11).xAccel * calibrationModule.getCalibrationDataAt(3) + application.dataRecorder.at(i-11).yAccel * calibrationModule.getCalibrationDataAt(4) + application.dataRecorder.at(i-11).zAccel * calibrationModule.getCalibrationDataAt(5) + calibrationModule.getCalibrationDataAt(10) );
                ytemp2 = (application.dataRecorder.at(i-10).xAccel * calibrationModule.getCalibrationDataAt(3) + application.dataRecorder.at(i-10).yAccel * calibrationModule.getCalibrationDataAt(4) + application.dataRecorder.at(i-10).zAccel * calibrationModule.getCalibrationDataAt(5) + calibrationModule.getCalibrationDataAt(10) );
                ztemp1 = (application.dataRecorder.at(i-11).xAccel * calibrationModule.getCalibrationDataAt(6) + application.dataRecorder.at(i-11).yAccel * calibrationModule.getCalibrationDataAt(7) + application.dataRecorder.at(i-11).zAccel * calibrationModule.getCalibrationDataAt(8) + calibrationModule.getCalibrationDataAt(11) );
                ztemp2 = (application.dataRecorder.at(i-10).xAccel * calibrationModule.getCalibrationDataAt(6) + application.dataRecorder.at(i-10).yAccel * calibrationModule.getCalibrationDataAt(7) + application.dataRecorder.at(i-10).zAccel * calibrationModule.getCalibrationDataAt(8) + calibrationModule.getCalibrationDataAt(11) );

                if(rotAxis == "x"){
                    alfa1 = calculateAngle(ytemp1, ztemp1);
                    alfa2 = calculateAngle(ytemp2, ztemp2);
                    pos = 0;
                } else if(rotAxis == "y"){
                    alfa1 = calculateAngle(ztemp1, xtemp1);
                    alfa2 = calculateAngle(ztemp2, xtemp2);
                    pos = 1;
                } else if(rotAxis == "z"){
                    alfa1 = calculateAngle(xtemp1, ytemp1);
                    alfa2 = calculateAngle(xtemp2, ytemp2);
                    pos = 2;
                }

                if( (alfa2 - alfa1) > M_PI ){
                    alfaSum += alfa2 - alfa1 - 2*M_PI;
                } else if( (alfa2 - alfa1) < -M_PI){
                    alfaSum += alfa2 - alfa1 + 2*M_PI;
                } else {
                    alfaSum += alfa2 - alfa1;
                }


                xGyro  = application.dataRecorder.at(i-10).xGyro;
                yGyro  = application.dataRecorder.at(i-10).yGyro;
                zGyro  = application.dataRecorder.at(i-10).zGyro;

                Xint += xGyro - gyroMinAvgs[0];
                Yint += yGyro - gyroMinAvgs[1];
                Zint += zGyro - gyroMinAvgs[2];
            }

            actSize++;

        } else if( actSize > 200 ){
            IdleGyroWindow idleGyroWindow;

            idleGyroWindow.numOfRotations = alfaSum / (2*M_PI);
            idleGyroWindow.alfaSum = alfaSum;
            idleGyroWindow.size = actSize;
            idleGyroWindow.Xint = Xint;
            idleGyroWindow.Yint = Yint;
            idleGyroWindow.Zint = Zint;

            if( idleGyroWindows.size() > pos){
                if( fabs(idleGyroWindows.at(pos).numOfRotations) < fabs(idleGyroWindow.numOfRotations) ){
                    idleGyroWindows.remove(pos);
                    idleGyroWindows.insert(pos, idleGyroWindow);
                }
            } else {
                idleGyroWindows.insert(pos, idleGyroWindow);
            }

            //qDebug() << idleGyroWindow.toString() << "AROUND " << rotAxis;
            //qDebug() << "From: " << i-actSize << " To: " << i << "\n";

            alfaSum = 0.0;
            actSize = 2;
            Xint = 0; Yint = 0; Zint = 0;
        } else {
            actSize = 2;
            alfaSum = 0.0;
            Xint = 0; Yint = 0; Zint = 0;
        }
    }
    qDebug() << application.dataRecorder.size() << "  -  ";
    qDebug() << idleGyroWindows.at(pos).toString() << "IdleGyroWindows Size: " << QString::number(idleGyroWindows.size());
    return "ok";
}

QString PeriodicalCalibrationModule::SVD()
{
    LinearEquations linearEquations;

    Equation* equation1 = linearEquations.createEquation();
    equation1->setConstant(idleGyroWindows.at(0).numOfRotations);
    equation1->setCoefficient("a11", idleGyroWindows.at(0).Xint);
    equation1->setCoefficient("a12", idleGyroWindows.at(0).Yint);
    equation1->setCoefficient("a13", idleGyroWindows.at(0).Zint);
    equation1->setCoefficient("a21", 0);
    equation1->setCoefficient("a22", 0);
    equation1->setCoefficient("a23", 0);
    equation1->setCoefficient("a31", 0);
    equation1->setCoefficient("a32", 0);
    equation1->setCoefficient("a33", 0);
    linearEquations.addEquation(equation1);

    Equation* equation2 = linearEquations.createEquation();
    equation2->setConstant(0);
    equation2->setCoefficient("a11", 0);
    equation2->setCoefficient("a12", 0);
    equation2->setCoefficient("a13", 0);
    equation2->setCoefficient("a21", idleGyroWindows.at(0).Xint);
    equation2->setCoefficient("a22", idleGyroWindows.at(0).Yint);
    equation2->setCoefficient("a23", idleGyroWindows.at(0).Zint);
    equation2->setCoefficient("a31", 0);
    equation2->setCoefficient("a32", 0);
    equation2->setCoefficient("a33", 0);
    linearEquations.addEquation(equation2);

    Equation* equation3 = linearEquations.createEquation();
    equation3->setConstant(0);
    equation3->setCoefficient("a11", 0);
    equation3->setCoefficient("a12", 0);
    equation3->setCoefficient("a13", 0);
    equation3->setCoefficient("a21", 0);
    equation3->setCoefficient("a22", 0);
    equation3->setCoefficient("a23", 0);
    equation3->setCoefficient("a31", idleGyroWindows.at(0).Xint);
    equation3->setCoefficient("a32", idleGyroWindows.at(0).Yint);
    equation3->setCoefficient("a33", idleGyroWindows.at(0).Zint);
    linearEquations.addEquation(equation3);

    Equation* equation4 = linearEquations.createEquation();
    equation4->setConstant(0);
    equation4->setCoefficient("a11", idleGyroWindows.at(1).Xint);
    equation4->setCoefficient("a12", idleGyroWindows.at(1).Yint);
    equation4->setCoefficient("a13", idleGyroWindows.at(1).Zint);
    equation4->setCoefficient("a21", 0);
    equation4->setCoefficient("a22", 0);
    equation4->setCoefficient("a23", 0);
    equation4->setCoefficient("a31", 0);
    equation4->setCoefficient("a32", 0);
    equation4->setCoefficient("a33", 0);
    linearEquations.addEquation(equation4);

    Equation* equation5 = linearEquations.createEquation();
    equation5->setConstant(idleGyroWindows.at(1).numOfRotations);
    equation5->setCoefficient("a11", 0);
    equation5->setCoefficient("a12", 0);
    equation5->setCoefficient("a13", 0);
    equation5->setCoefficient("a21", idleGyroWindows.at(1).Xint);
    equation5->setCoefficient("a22", idleGyroWindows.at(1).Yint);
    equation5->setCoefficient("a23", idleGyroWindows.at(1).Zint);
    equation5->setCoefficient("a31", 0);
    equation5->setCoefficient("a32", 0);
    equation5->setCoefficient("a33", 0);
    linearEquations.addEquation(equation5);

    Equation* equation6 = linearEquations.createEquation();
    equation6->setConstant(0);
    equation6->setCoefficient("a11", 0);
    equation6->setCoefficient("a12", 0);
    equation6->setCoefficient("a13", 0);
    equation6->setCoefficient("a21", 0);
    equation6->setCoefficient("a22", 0);
    equation6->setCoefficient("a23", 0);
    equation6->setCoefficient("a31", idleGyroWindows.at(1).Xint);
    equation6->setCoefficient("a32", idleGyroWindows.at(1).Yint);
    equation6->setCoefficient("a33", idleGyroWindows.at(1).Zint);
    linearEquations.addEquation(equation6);

    Equation* equation7 = linearEquations.createEquation();
    equation7->setConstant(0);
    equation7->setCoefficient("a11", idleGyroWindows.at(2).Xint);
    equation7->setCoefficient("a12", idleGyroWindows.at(2).Yint);
    equation7->setCoefficient("a13", idleGyroWindows.at(2).Zint);
    equation7->setCoefficient("a21", 0);
    equation7->setCoefficient("a22", 0);
    equation7->setCoefficient("a23", 0);
    equation7->setCoefficient("a31", 0);
    equation7->setCoefficient("a32", 0);
    equation7->setCoefficient("a33", 0);
    linearEquations.addEquation(equation7);

    Equation* equation8 = linearEquations.createEquation();
    equation8->setConstant(0);
    equation8->setCoefficient("a11", 0);
    equation8->setCoefficient("a12", 0);
    equation8->setCoefficient("a13", 0);
    equation8->setCoefficient("a21", idleGyroWindows.at(2).Xint);
    equation8->setCoefficient("a22", idleGyroWindows.at(2).Yint);
    equation8->setCoefficient("a23", idleGyroWindows.at(2).Zint);
    equation8->setCoefficient("a31", 0);
    equation8->setCoefficient("a32", 0);
    equation8->setCoefficient("a33", 0);
    linearEquations.addEquation(equation2);

    Equation* equation9 = linearEquations.createEquation();
    equation9->setConstant(idleGyroWindows.at(2).numOfRotations);
    equation9->setCoefficient("a11", 0);
    equation9->setCoefficient("a12", 0);
    equation9->setCoefficient("a13", 0);
    equation9->setCoefficient("a21", 0);
    equation9->setCoefficient("a22", 0);
    equation9->setCoefficient("a23", 0);
    equation9->setCoefficient("a31", idleGyroWindows.at(2).Xint);
    equation9->setCoefficient("a32", idleGyroWindows.at(2).Yint);
    equation9->setCoefficient("a33", idleGyroWindows.at(2).Zint);
    linearEquations.addEquation(equation9);

    Solution* solution = linearEquations.solveWithSVD(0.0);
    solution->print();

    QString returnMessage = "";

    if ( solution->getMaximumError() > 0.1 ) {
        returnMessage = "Maximum Error is too great! ( > 0.1 )  \n";
        return returnMessage;
    } else {
        application.settings.beginWriteArray("gyroCalibrationData");
        for (unsigned int i = 0; i < linearEquations.getVariableCount(); i++) {
            application.settings.setArrayIndex(i);
            application.settings.setValue("gyroCalibrationData", solution->getValueAt(i));
        }
        application.settings.endArray();

        application.settings.beginReadArray("gyroCalibrationData");
        returnMessage.append("\nGyro Calibration Data: \n");

        application.settings.setArrayIndex(0);
        returnMessage.append("a11 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[0] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(1);
        returnMessage.append("a12 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[1] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(2);
        returnMessage.append("a13 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[2] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(3);
        returnMessage.append("a21 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[3] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(4);
        returnMessage.append("a22 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[4] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(5);
        returnMessage.append("a23 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[5] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(6);
        returnMessage.append("a31 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[6] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(7);
        returnMessage.append("a32 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[7] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.setArrayIndex(8);
        returnMessage.append("a33 ");
        returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        gyroCalibrationData[8] = application.settings.value("gyroCalibrationData").toDouble();

        application.settings.endArray();

        return returnMessage;
    }
}

double PeriodicalCalibrationModule::calculateAngle(double accel1, double accel2) {
    double alfa;
    if( (pow(accel1, 2.0) + pow(accel2, 2.0)) < pow(GRAV/2, 2.0) )
        return 10.0;

    if( fabs(accel1) > fabs(accel2) ) {
        alfa = atan(accel2/accel1);
        if( accel1 < 0 )
            alfa += M_PI;
    }
    else {
        alfa = M_PI/2 - atan(accel1/accel2);
        if( accel2 < 0 )
            alfa += M_PI;
    }

    if ( alfa >= M_PI) alfa -= 2*M_PI;
    if ( alfa < -M_PI) alfa += 2*M_PI;

    return alfa;
}

QString IdleGyroWindow::toString() const
{
    QString s = "Idle Gyro Window: \n";

    s += "Size: " + QString::number(size) + "\n"
         + "  AlfaSum: " + QString::number(alfaSum)
         + ",  Number of Rotations: " + QString::number(numOfRotations) + "\n"
         + " Xint: " + QString::number(Xint) + " Yint: " + QString::number(Yint)
         + " Zint: " + QString::number(Zint);

        return s + "\n";
}

