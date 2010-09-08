/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include <stdexcept>
#include "PeriodicalCalibrationModule.h"
#include "StationaryCalibrationModule.h"
#include "CalibrationWidget.h"
#include "LinearEquations.h"
#include "math.h"
#include "QtDebug"

PeriodicalCalibrationModule::PeriodicalCalibrationModule(Application &app, StationaryCalibrationModule &calMod ) :
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
    ClearWindows();
}

IdleGyroWindow::IdleGyroWindow(){
    size = 2;
    numOfRotations = 0.0;
}

QString PeriodicalCalibrationModule::Calibrate(QString rotAxis)
{
    double Xint = 0.0; double Yint = 0.0; double Zint = 0.0;
    int xGyro, yGyro, zGyro;
    double xtemp1, ytemp1, ztemp1, xtemp2, ytemp2, ztemp2, avgtemp, alfa1, alfa2, idleAxis;
    double alfaSum = 0.0;
    double SqSum = 0.0;
    int pos;

    int actSize = 2;

    for(int i = 11; i < application.dataRecorder.size(); i++){
        xtemp1 = getCalibratedData(i-1,"x");
        xtemp2 = getCalibratedData(i, "x");
        ytemp1 = getCalibratedData(i-1, "y");
        ytemp2 = getCalibratedData(i, "y");
        ztemp1 = getCalibratedData(i-1, "z");
        ztemp2 = getCalibratedData(i, "z");

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
                xtemp1 = getCalibratedData(i-11, "x");
                xtemp2 = getCalibratedData(i-10, "x");
                ytemp1 = getCalibratedData(i-11, "y");
                ytemp2 = getCalibratedData(i-10, "y");
                ztemp1 = getCalibratedData(i-11, "z");
                ztemp2 = getCalibratedData(i-10, "z");

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
    //qDebug() << application.dataRecorder.size() << "  -  ";
    //qDebug() << idleGyroWindows.at(pos).toString() << "IdleGyroWindows Size: " << QString::number(idleGyroWindows.size());
    return "ok";
}

QString PeriodicalCalibrationModule::SVD()
{
    LinearEquations linearEquations;

    linearEquations.getVariable("a11");
    linearEquations.getVariable("a12");
    linearEquations.getVariable("a13");
    linearEquations.getVariable("a21");
    linearEquations.getVariable("a22");
    linearEquations.getVariable("a23");
    linearEquations.getVariable("a31");
    linearEquations.getVariable("a32");
    linearEquations.getVariable("a33");
    linearEquations.getVariable("b1");
    linearEquations.getVariable("b2");
    linearEquations.getVariable("b3");

    Equation* equation1 = linearEquations.createEquation();
    equation1->setConstant(idleGyroWindows.at(0).numOfRotations*2*M_PI*C_HZ);
    equation1->setCoefficient("a11", idleGyroWindows.at(0).Xint);
    equation1->setCoefficient("a12", idleGyroWindows.at(0).Yint);
    equation1->setCoefficient("a13", idleGyroWindows.at(0).Zint);
    equation1->setCoefficient("b1", 1);
    linearEquations.addEquation(equation1);

    Equation* equation2 = linearEquations.createEquation();
    equation2->setConstant(0);
    equation2->setCoefficient("a21", idleGyroWindows.at(0).Xint);
    equation2->setCoefficient("a22", idleGyroWindows.at(0).Yint);
    equation2->setCoefficient("a23", idleGyroWindows.at(0).Zint);
    equation2->setCoefficient("b2", 1);
    linearEquations.addEquation(equation2);

    Equation* equation3 = linearEquations.createEquation();
    equation3->setConstant(0);
    equation3->setCoefficient("a31", idleGyroWindows.at(0).Xint);
    equation3->setCoefficient("a32", idleGyroWindows.at(0).Yint);
    equation3->setCoefficient("a33", idleGyroWindows.at(0).Zint);
    equation3->setCoefficient("b3", 1);
    linearEquations.addEquation(equation3);

    Equation* equation4 = linearEquations.createEquation();
    equation4->setConstant(0);
    equation4->setCoefficient("a11", idleGyroWindows.at(1).Xint);
    equation4->setCoefficient("a12", idleGyroWindows.at(1).Yint);
    equation4->setCoefficient("a13", idleGyroWindows.at(1).Zint);
    equation4->setCoefficient("b1", 1);
    linearEquations.addEquation(equation4);

    Equation* equation5 = linearEquations.createEquation();
    equation5->setConstant(idleGyroWindows.at(1).numOfRotations*2*M_PI*C_HZ);
    equation5->setCoefficient("a21", idleGyroWindows.at(1).Xint);
    equation5->setCoefficient("a22", idleGyroWindows.at(1).Yint);
    equation5->setCoefficient("a23", idleGyroWindows.at(1).Zint);
    equation5->setCoefficient("b2", 1);
    linearEquations.addEquation(equation5);

    Equation* equation6 = linearEquations.createEquation();
    equation6->setConstant(0);
    equation6->setCoefficient("a31", idleGyroWindows.at(1).Xint);
    equation6->setCoefficient("a32", idleGyroWindows.at(1).Yint);
    equation6->setCoefficient("a33", idleGyroWindows.at(1).Zint);
    equation6->setCoefficient("b3", 1);
    linearEquations.addEquation(equation6);

    Equation* equation7 = linearEquations.createEquation();
    equation7->setConstant(0);
    equation7->setCoefficient("a11", idleGyroWindows.at(2).Xint);
    equation7->setCoefficient("a12", idleGyroWindows.at(2).Yint);
    equation7->setCoefficient("a13", idleGyroWindows.at(2).Zint);
    equation7->setCoefficient("b1", 1);
    linearEquations.addEquation(equation7);

    Equation* equation8 = linearEquations.createEquation();
    equation8->setConstant(0);
    equation8->setCoefficient("a21", idleGyroWindows.at(2).Xint);
    equation8->setCoefficient("a22", idleGyroWindows.at(2).Yint);
    equation8->setCoefficient("a23", idleGyroWindows.at(2).Zint);
    equation8->setCoefficient("b2", 1);
    linearEquations.addEquation(equation8);

    Equation* equation9 = linearEquations.createEquation();
    equation9->setConstant(idleGyroWindows.at(2).numOfRotations*2*M_PI*C_HZ);
    equation9->setCoefficient("a31", idleGyroWindows.at(2).Xint);
    equation9->setCoefficient("a32", idleGyroWindows.at(2).Yint);
    equation9->setCoefficient("a33", idleGyroWindows.at(2).Zint);
    equation9->setCoefficient("b3", 1);
    linearEquations.addEquation(equation9);

    Equation* equation10 = linearEquations.createEquation();
    equation10->setConstant(0);
    equation10->setCoefficient("a11", gyroMinAvgs[0]);
    equation10->setCoefficient("a12", gyroMinAvgs[1]);
    equation10->setCoefficient("a13", gyroMinAvgs[2]);
    equation10->setCoefficient("b1", 1);
    linearEquations.addEquation(equation10);

    Equation* equation11 = linearEquations.createEquation();
    equation11->setConstant(0);
    equation11->setCoefficient("a21", gyroMinAvgs[0]);
    equation11->setCoefficient("a22", gyroMinAvgs[1]);
    equation11->setCoefficient("a23", gyroMinAvgs[2]);
    equation11->setCoefficient("b2", 1);
    linearEquations.addEquation(equation11);

    Equation* equation12 = linearEquations.createEquation();
    equation12->setConstant(0);
    equation12->setCoefficient("a31", gyroMinAvgs[0]);
    equation12->setCoefficient("a32", gyroMinAvgs[1]);
    equation12->setCoefficient("a33", gyroMinAvgs[2]);
    equation12->setCoefficient("b3", 1);
    linearEquations.addEquation(equation12);

    linearEquations.printStatistics();

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
            gyroCalibrationData[i] = solution->getValueAt(i);
        }
        application.settings.endArray();

        application.settings.beginReadArray("gyroCalibrationData");
        returnMessage.append("\nGyro Calibration Data: \n");

        QMap<QString, unsigned int> variables = linearEquations.getVariables();
        for(int i = 0; i < variables.size(); i++){
            application.settings.setArrayIndex(i);
            returnMessage.append(variables.key(i) + "\t");
            returnMessage.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
        }
        application.settings.endArray();

        return returnMessage;
    }
}

double PeriodicalCalibrationModule::getCalibratedData(int time, QString axis)
{
    if( axis == "x" ){
        return ( application.dataRecorder.at(time).xAccel * application.dataRecorder.getAccelCalibration()[0] + application.dataRecorder.at(time).yAccel * application.dataRecorder.getAccelCalibration()[1] + application.dataRecorder.at(time).zAccel * application.dataRecorder.getAccelCalibration()[2] + application.dataRecorder.getAccelCalibration()[9] );
    }else if( axis == "y" ){
        return ( application.dataRecorder.at(time).xAccel * application.dataRecorder.getAccelCalibration()[3] + application.dataRecorder.at(time).yAccel * application.dataRecorder.getAccelCalibration()[4] + application.dataRecorder.at(time).zAccel * application.dataRecorder.getAccelCalibration()[5] + application.dataRecorder.getAccelCalibration()[10] );
    }else if ( axis == "z" ){
        return ( application.dataRecorder.at(time).xAccel * application.dataRecorder.getAccelCalibration()[6] + application.dataRecorder.at(time).yAccel * application.dataRecorder.getAccelCalibration()[7] + application.dataRecorder.at(time).zAccel * application.dataRecorder.getAccelCalibration()[8] + application.dataRecorder.getAccelCalibration()[11] );
    }

    


    throw std::logic_error("Incorrect axis!");
	return 0;

}

double PeriodicalCalibrationModule::calculateAngle(double accel1, double accel2)
{
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

void PeriodicalCalibrationModule::ClearWindows()
{
        idleGyroWindows.clear();
}

