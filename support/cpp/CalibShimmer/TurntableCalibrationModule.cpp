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

#include "TurntableCalibrationModule.h"
#include <QtDebug>
#include <qfile.h>
#include "LinearEquations.h"
#include "MoteData.h"
#include "tnt_math_utils.h"
#include <cmath>


using namespace std;

TurntableCalibrationModule::TurntableCalibrationModule(Application& app) : application(app)
{
    xGyroMin = 9999; yGyroMin = 9999, zGyroMin = 9999;
    xGyroMax = 0; yGyroMax = 0; zGyroMax = 0;
    xGyroDiff = 0; yGyroDiff = 0; zGyroDiff = 0;
    xGyroAvg = 0.0; yGyroAvg = 0.0; zGyroAvg = 0.0;
    xMinGyroAvg = 9999.99; xMaxGyroAvg = 0.0; yMinGyroAvg = 9999.99; yMaxGyroAvg = 0.0; zMinGyroAvg = 9999.99; zMaxGyroAvg = 0.0;
    xGyroSum = 0; yGyroSum = 0; zGyroSum = 0;

}

TurntableCalibrationModule::~TurntableCalibrationModule()
{
    clearWindows();
    clearTurntableSides();
}

TurntableWindow::TurntableWindow()
{
    xGyroMin = -1;
    xGyroMax = -1;
    yGyroMin = -1;
    yGyroMax = -1;
    zGyroMin = -1;
    zGyroMax = -1;
    xGyroAvg = -1;
    yGyroAvg = -1;
    zGyroAvg = -1;
}

QString TurntableCalibrationModule::Calibrate(MoteData* moteData, int rpm)
{

    if( rpm == 33 ){
        constant = ((100/3) * 2*M_PI) / 60;
    } else if ( rpm == 45){
        constant = (45 * 2*M_PI) / 60;
    } else {
        constant = -1;
    }

    int xGyro, yGyro, zGyro;

    for(int j = 0; j < moteData->samplesSize()-GYROWINDOW; j+=10) {
        for(int i = j; i < j + GYROWINDOW; i++){
            xGyro  = moteData->sampleAt(i).xGyro;
            yGyro  = moteData->sampleAt(i).yGyro;
            zGyro  = moteData->sampleAt(i).zGyro;

            if(xGyro > xGyroMax) xGyroMax = xGyro; if(xGyro < xGyroMin) xGyroMin = xGyro;
            if(yGyro > yGyroMax) yGyroMax = yGyro; if(yGyro < yGyroMin) yGyroMin = yGyro;
            if(zGyro > zGyroMax) zGyroMax = zGyro; if(zGyro < zGyroMin) zGyroMin = zGyro;

            xGyroSum += xGyro;
            yGyroSum += yGyro;
            zGyroSum += zGyro;
        }

        xGyroAvg = (float) xGyroSum/GYROWINDOW; yGyroAvg = (float) yGyroSum/GYROWINDOW; zGyroAvg = (float) zGyroSum/GYROWINDOW;

        if(xGyroAvg > xMaxGyroAvg) xMaxGyroAvg = xGyroAvg; if(xGyroAvg < xMinGyroAvg) xMinGyroAvg = xGyroAvg;
        if(yGyroAvg > yMaxGyroAvg) yMaxGyroAvg = yGyroAvg; if(yGyroAvg < yMinGyroAvg) yMinGyroAvg = yGyroAvg;
        if(zGyroAvg > zMaxGyroAvg) zMaxGyroAvg = zGyroAvg; if(zGyroAvg < zMinGyroAvg) zMinGyroAvg = zGyroAvg;

        xGyroDiff = xGyroMax - xGyroMin;
        yGyroDiff = yGyroMax - yGyroMin;
        zGyroDiff = zGyroMax - zGyroMin;

        if( (xGyroDiff < GYROMAXDIFF) && (yGyroDiff < GYROMAXDIFF) && (zGyroDiff < GYROMAXDIFF) ){
            TurntableWindow turntableWindow;

            turntableWindow.time = moteData->sampleAt(j).mote_time - GYROWINDOW;
            turntableWindow.xGyroMax = xGyroMax; turntableWindow.xGyroMin = xGyroMin;
            turntableWindow.yGyroMax = yGyroMax; turntableWindow.yGyroMin = yGyroMin;
            turntableWindow.zGyroMax = zGyroMax; turntableWindow.zGyroMin = zGyroMin;
            turntableWindow.xGyroAvg = xGyroAvg; turntableWindow.yGyroAvg = yGyroAvg; turntableWindow.zGyroAvg = zGyroAvg;
            turntableWindow.start = j;
            turntableWindows.append(turntableWindow);
        }

        xGyroMin = 9999; yGyroMin = 9999, zGyroMin = 9999;
        xGyroMax = 0; yGyroMax = 0; zGyroMax = 0;
        xGyroDiff = 0; yGyroDiff = 0; zGyroDiff = 0;
        xGyroAvg = 0.0; yGyroAvg = 0.0; zGyroAvg = 0.0;
        xGyroSum = 0; yGyroSum = 0; zGyroSum = 0;
    }

    if (turntableWindows.size() < 6){
        //msgBox.setText("Calibration Error! See console for details...");
        //msgBox.exec();
        return "Please load a data record with the mote being idle on each side for at least a 2 seconds!";
    } else {
        /*for(int i = 0; i < turntableWindows.size(); i++){
            qDebug() << turntableWindows[i].toString();
        }*/

        return Classify();
    }
}

QString TurntableCalibrationModule::Classify()
{
    QString errormsg;
    double xTemp,yTemp,zTemp;
    int xCLWMinDiff = 3*GYROMAXDIFF; int xCCLWMinDiff = 3*GYROMAXDIFF;
    int yCLWMinDiff = 3*GYROMAXDIFF; int yCCLWMinDiff = 3*GYROMAXDIFF;
    int zCLWMinDiff = 3*GYROMAXDIFF; int zCCLWMinDiff = 3*GYROMAXDIFF;
    int IMinDiff = 3*GYROMAXDIFF;

    for (int i = 0; i < 7; i++) {
        turntableSidesMins[i] = -1;
    }

    for (int i=0; i < turntableWindows.size(); i++) {
        xTemp = turntableWindows[i].xGyroAvg; yTemp = turntableWindows[i].yGyroAvg; zTemp = turntableWindows[i].zGyroAvg;
        int actDiff = (turntableWindows[i].xGyroMax-turntableWindows[i].xGyroMin)+(turntableWindows[i].yGyroMax-turntableWindows[i].yGyroMin)+(turntableWindows[i].zGyroMax-turntableWindows[i].zGyroMin);
        if ( (xTemp >= xCLW) && (xTemp <= xMaxGyroAvg) && (yTemp >= yI_L) && (yTemp <= yI_U ) && (zTemp >= zI_L) && (zTemp <= zI_U ) ) {
            //x rotating ClockWise
            if ( xCLWMinDiff > actDiff ) {
                xCLWMinDiff = actDiff;
                turntableSidesMins[0] = i;
            }
        } else if ( (xTemp >= xMinGyroAvg) && (xTemp <= xCCLW) && (yTemp >= yI_L) && (yTemp <= yI_U ) && (zTemp >= zI_L) && (zTemp <= zI_U ) ) {
            //x rotating counterClockWise
            if ( xCCLWMinDiff > actDiff ) {
                xCCLWMinDiff = actDiff;
                turntableSidesMins[1] = i;
            }
        } else if ( (xTemp >= xI_L) && (xTemp <= xI_U ) && (yTemp >= yCLW) && (yTemp <= yMaxGyroAvg) && (zTemp >= zI_L) && (zTemp <= zI_U ) ) {
            //y rotating ClockWise
            if ( yCLWMinDiff > actDiff ) {
                yCLWMinDiff = actDiff;
                turntableSidesMins[2] = i;
            }
        } else if ( (xTemp >= xI_L) && (xTemp <= xI_U ) && (yTemp >= yMinGyroAvg) && (yTemp <= yCCLW) && (zTemp >= zI_L) && (zTemp <= zI_U ) ) {
            //y rotating counterClockWise
            if ( yCCLWMinDiff > actDiff ) {
                yCCLWMinDiff = actDiff;
                turntableSidesMins[3] = i;
            }
        } else if ( (xTemp >= xI_L) && (xTemp <= xI_U ) && (yTemp >= yI_L) && (yTemp <= yI_U ) && (zTemp >= zCLW) && (zTemp <= zMaxGyroAvg) ) {
            //z rotating ClockWise
            if ( zCLWMinDiff > actDiff ) {
                zCLWMinDiff = actDiff;
                turntableSidesMins[4] = i;
            }
        } else if ( (xTemp >= xI_L) && (xTemp <= xI_U ) && (yTemp >= yI_L) && (yTemp <= yI_U ) && (zTemp >= zMinGyroAvg) && (zTemp <= zCCLW) ) {
            //z rotating counterClockWise
            if ( zCCLWMinDiff > actDiff ) {
                zCCLWMinDiff = actDiff;
                turntableSidesMins[5] = i;
            }
        } else if ( (xTemp >= xI_L) && (xTemp <= xI_U ) && (yTemp >= yI_L) && (yTemp <= yI_U ) && (zTemp >= zI_L) && (zTemp <= zI_U ) ){
            //not rotating
            if ( IMinDiff > actDiff ) {
                IMinDiff = actDiff;
                turntableSidesMins[6] = i;
            }
        } else {
            errormsg.append("Idle window outside boundaries! \n");
            errormsg.append(turntableWindows[i].toString());
            errormsg.append("Actual Difference: " + QString::number(actDiff) + "\n");
            errormsg.append("xCLWMinDiff: " + QString::number(xCLWMinDiff) + "\n");
            errormsg.append("xCCLWMinDiff: " + QString::number(xCCLWMinDiff) + "\n");
            errormsg.append("yCLWMinDiff: " + QString::number(xCLWMinDiff) + "\n");
            errormsg.append("yCCLWMinDiff: " + QString::number(xCCLWMinDiff) + "\n");
            errormsg.append("zCLWMinDiff: " + QString::number(xCLWMinDiff) + "\n");
            errormsg.append("zCCLWMinDiff: " + QString::number(xCCLWMinDiff) + "\n");
            errormsg.append("IMinDiff: " + QString::number(IMinDiff) + "\n");
            errormsg.append("X AVGs: " + QString::number(xMinGyroAvg) + " - " + QString::number(xMaxGyroAvg) + "\n");
            errormsg.append("Y AVGs: " + QString::number(yMinGyroAvg) + " - " + QString::number(yMaxGyroAvg) + "\n");
            errormsg.append("Z AVGs: " + QString::number(zMinGyroAvg) + " - " + QString::number(zMaxGyroAvg) + "\n");
            errormsg.append("X regions: " + QString::number(xMinGyroAvg) + " - " + QString::number(xCCLW) + " ; " + QString::number(xI_L) + " - " + QString::number(xI_U) + " ; " + QString::number(xCLW) + " - " + QString::number(xMaxGyroAvg) + "\n");
            errormsg.append("Y regions: " + QString::number(yMinGyroAvg) + " - " + QString::number(yCCLW) + " ; " + QString::number(yI_L) + " - " + QString::number(yI_U) + " ; " + QString::number(yCLW) + " - " + QString::number(yMaxGyroAvg) + "\n");
            errormsg.append("Z regions: " + QString::number(zMinGyroAvg) + " - " + QString::number(zCCLW) + " ; " + QString::number(zI_L) + " - " + QString::number(zI_U) + " ; " + QString::number(zCLW) + " - " + QString::number(zMaxGyroAvg) + "\n");

            //msgBox.setText("Calibration Error! See console for details...");
            //msgBox.exec();

            //application.dataRecorder.setGyroIdleWindowStart(0, turntableWindows[i].start);
            return errormsg;
        }
    }
    xMinGyroAvg = 9999.99; xMaxGyroAvg = 0.0; yMinGyroAvg = 9999.99; yMaxGyroAvg = 0.0; zMinGyroAvg = 9999.99; zMaxGyroAvg = 0.0;

    for ( int i = 0; i < 7; i++ ) {
        if ( turntableSidesMins[i] == -1 ) { // FIXME What is going on here?
            //msgBox.setText("Calibration Error! See console for details...");
            //msgBox.exec();
            //application.dataRecorder.setGyroIdleWindowStart(i, turntableWindows[i].start);
            return "Calibration is missing the mote being idle on one of its sides! (Please load a record with the mote being idle on each side for at least 2 seconds!)";
        } else {
            //application.dataRecorder.setGyroIdleWindowStart(i, turntableWindows[i].start);
            //qDebug() << turntableWindows[turntableSidesMins[i]].toString();
        }
    }

    return LSF();
}

QString TurntableCalibrationModule::LSF() {

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

    //X rotating CLW
    Equation* equation1 = linearEquations.createEquation();
    equation1->setConstant(constant);
    equation1->setCoefficient("a11", turntableWindows[turntableSidesMins[0]].xGyroAvg);
    equation1->setCoefficient("a12", turntableWindows[turntableSidesMins[0]].yGyroAvg);
    equation1->setCoefficient("a13", turntableWindows[turntableSidesMins[0]].zGyroAvg);
    equation1->setCoefficient("b1", 1);
    linearEquations.addEquation(equation1);

    Equation* equation2 = linearEquations.createEquation();
    equation2->setConstant(0.0);
    equation2->setCoefficient("a21", turntableWindows[turntableSidesMins[0]].xGyroAvg);
    equation2->setCoefficient("a22", turntableWindows[turntableSidesMins[0]].yGyroAvg);
    equation2->setCoefficient("a23", turntableWindows[turntableSidesMins[0]].zGyroAvg);
    equation2->setCoefficient("b2", 1);
    linearEquations.addEquation(equation2);

    Equation* equation3 = linearEquations.createEquation();
    equation3->setConstant(0.0);
    equation3->setCoefficient("a31", turntableWindows[turntableSidesMins[0]].xGyroAvg);
    equation3->setCoefficient("a32", turntableWindows[turntableSidesMins[0]].yGyroAvg);
    equation3->setCoefficient("a33", turntableWindows[turntableSidesMins[0]].zGyroAvg);
    equation3->setCoefficient("b3", 1);
    linearEquations.addEquation(equation3);

    //X rotating CCLW
    Equation* equation4 = linearEquations.createEquation();
    equation4->setConstant(-constant);
    equation4->setCoefficient("a11", turntableWindows[turntableSidesMins[1]].xGyroAvg);
    equation4->setCoefficient("a12", turntableWindows[turntableSidesMins[1]].yGyroAvg);
    equation4->setCoefficient("a13", turntableWindows[turntableSidesMins[1]].zGyroAvg);
    equation4->setCoefficient("b1", 1);
    linearEquations.addEquation(equation4);

    Equation* equation5 = linearEquations.createEquation();
    equation5->setConstant(0.0);
    equation5->setCoefficient("a21", turntableWindows[turntableSidesMins[1]].xGyroAvg);
    equation5->setCoefficient("a22", turntableWindows[turntableSidesMins[1]].yGyroAvg);
    equation5->setCoefficient("a23", turntableWindows[turntableSidesMins[1]].zGyroAvg);
    equation5->setCoefficient("b2", 1);
    linearEquations.addEquation(equation5);

    Equation* equation6 = linearEquations.createEquation();
    equation6->setConstant(0.0);
    equation6->setCoefficient("a31", turntableWindows[turntableSidesMins[1]].xGyroAvg);
    equation6->setCoefficient("a32", turntableWindows[turntableSidesMins[1]].yGyroAvg);
    equation6->setCoefficient("a33", turntableWindows[turntableSidesMins[1]].zGyroAvg);
    equation6->setCoefficient("b3", 1);
    linearEquations.addEquation(equation6);

    //Y rotating CLW
    Equation* equation7 = linearEquations.createEquation();
    equation7->setConstant(0.0);
    equation7->setCoefficient("a11", turntableWindows[turntableSidesMins[2]].xGyroAvg);
    equation7->setCoefficient("a12", turntableWindows[turntableSidesMins[2]].yGyroAvg);
    equation7->setCoefficient("a13", turntableWindows[turntableSidesMins[2]].zGyroAvg);
    equation7->setCoefficient("b1", 1);
    linearEquations.addEquation(equation7);

    Equation* equation8 = linearEquations.createEquation();
    equation8->setConstant(constant);
    equation8->setCoefficient("a21", turntableWindows[turntableSidesMins[2]].xGyroAvg);
    equation8->setCoefficient("a22", turntableWindows[turntableSidesMins[2]].yGyroAvg);
    equation8->setCoefficient("a23", turntableWindows[turntableSidesMins[2]].zGyroAvg);
    equation8->setCoefficient("b2", 1);
    linearEquations.addEquation(equation8);

    Equation* equation9 = linearEquations.createEquation();
    equation9->setConstant(0.0);
    equation9->setCoefficient("a31", turntableWindows[turntableSidesMins[2]].xGyroAvg);
    equation9->setCoefficient("a32", turntableWindows[turntableSidesMins[2]].yGyroAvg);
    equation9->setCoefficient("a33", turntableWindows[turntableSidesMins[2]].zGyroAvg);
    equation9->setCoefficient("b3", 1);
    linearEquations.addEquation(equation9);

    //Y rotating CCLW
    Equation* equation10 = linearEquations.createEquation();
    equation10->setConstant(0.0);
    equation10->setCoefficient("a11", turntableWindows[turntableSidesMins[3]].xGyroAvg);
    equation10->setCoefficient("a12", turntableWindows[turntableSidesMins[3]].yGyroAvg);
    equation10->setCoefficient("a13", turntableWindows[turntableSidesMins[3]].zGyroAvg);
    equation10->setCoefficient("b1", 1);
    linearEquations.addEquation(equation10);

    Equation* equation11 = linearEquations.createEquation();
    equation11->setConstant(-constant);
    equation11->setCoefficient("a21", turntableWindows[turntableSidesMins[3]].xGyroAvg);
    equation11->setCoefficient("a22", turntableWindows[turntableSidesMins[3]].yGyroAvg);
    equation11->setCoefficient("a23", turntableWindows[turntableSidesMins[3]].zGyroAvg);
    equation11->setCoefficient("b2", 1);
    linearEquations.addEquation(equation11);

    Equation* equation12 = linearEquations.createEquation();
    equation12->setConstant(0.0);
    equation12->setCoefficient("a31", turntableWindows[turntableSidesMins[3]].xGyroAvg);
    equation12->setCoefficient("a32", turntableWindows[turntableSidesMins[3]].yGyroAvg);
    equation12->setCoefficient("a33", turntableWindows[turntableSidesMins[3]].zGyroAvg);
    equation12->setCoefficient("b3", 1);
    linearEquations.addEquation(equation12);

    //Z rotating CLW
    Equation* equation13 = linearEquations.createEquation();
    equation13->setConstant(0.0);
    equation13->setCoefficient("a11", turntableWindows[turntableSidesMins[4]].xGyroAvg);
    equation13->setCoefficient("a12", turntableWindows[turntableSidesMins[4]].yGyroAvg);
    equation13->setCoefficient("a13", turntableWindows[turntableSidesMins[4]].zGyroAvg);
    equation13->setCoefficient("b1", 1);
    linearEquations.addEquation(equation13);

    Equation* equation14 = linearEquations.createEquation();
    equation14->setConstant(0.0);
    equation14->setCoefficient("a21", turntableWindows[turntableSidesMins[4]].xGyroAvg);
    equation14->setCoefficient("a22", turntableWindows[turntableSidesMins[4]].yGyroAvg);
    equation14->setCoefficient("a23", turntableWindows[turntableSidesMins[4]].zGyroAvg);
    equation14->setCoefficient("b2", 1);
    linearEquations.addEquation(equation14);

    Equation* equation15 = linearEquations.createEquation();
    equation15->setConstant(constant);
    equation15->setCoefficient("a31", turntableWindows[turntableSidesMins[4]].xGyroAvg);
    equation15->setCoefficient("a32", turntableWindows[turntableSidesMins[4]].yGyroAvg);
    equation15->setCoefficient("a33", turntableWindows[turntableSidesMins[4]].zGyroAvg);
    equation15->setCoefficient("b3", 1);
    linearEquations.addEquation(equation15);

    //Z rotating CCLW
    Equation* equation16 = linearEquations.createEquation();
    equation16->setConstant(0.0);
    equation16->setCoefficient("a11", turntableWindows[turntableSidesMins[5]].xGyroAvg);
    equation16->setCoefficient("a12", turntableWindows[turntableSidesMins[5]].yGyroAvg);
    equation16->setCoefficient("a13", turntableWindows[turntableSidesMins[5]].zGyroAvg);
    equation16->setCoefficient("b1", 1);
    linearEquations.addEquation(equation16);

    Equation* equation17 = linearEquations.createEquation();
    equation17->setConstant(0.0);
    equation17->setCoefficient("a21", turntableWindows[turntableSidesMins[5]].xGyroAvg);
    equation17->setCoefficient("a22", turntableWindows[turntableSidesMins[5]].yGyroAvg);
    equation17->setCoefficient("a23", turntableWindows[turntableSidesMins[5]].zGyroAvg);
    equation17->setCoefficient("b2", 1);
    linearEquations.addEquation(equation17);

    Equation* equation18 = linearEquations.createEquation();
    equation18->setConstant(-constant);
    equation18->setCoefficient("a31", turntableWindows[turntableSidesMins[5]].xGyroAvg);
    equation18->setCoefficient("a32", turntableWindows[turntableSidesMins[5]].yGyroAvg);
    equation18->setCoefficient("a33", turntableWindows[turntableSidesMins[5]].zGyroAvg);
    equation18->setCoefficient("b3", 1);
    linearEquations.addEquation(equation18);

    //mote completely idle
    Equation* equation19 = linearEquations.createEquation();
    equation19->setConstant(0.0);
    equation19->setCoefficient("a11", turntableWindows[turntableSidesMins[6]].xGyroAvg);
    equation19->setCoefficient("a12", turntableWindows[turntableSidesMins[6]].yGyroAvg);
    equation19->setCoefficient("a13", turntableWindows[turntableSidesMins[6]].zGyroAvg);
    equation19->setCoefficient("b1", 1);
    linearEquations.addEquation(equation19);

    Equation* equation20 = linearEquations.createEquation();
    equation20->setConstant(0.0);
    equation20->setCoefficient("a21", turntableWindows[turntableSidesMins[6]].xGyroAvg);
    equation20->setCoefficient("a22", turntableWindows[turntableSidesMins[6]].yGyroAvg);
    equation20->setCoefficient("a23", turntableWindows[turntableSidesMins[6]].zGyroAvg);
    equation20->setCoefficient("b2", 1);
    linearEquations.addEquation(equation20);

    Equation* equation21 = linearEquations.createEquation();
    equation21->setConstant(0.0);
    equation21->setCoefficient("a31", turntableWindows[turntableSidesMins[6]].xGyroAvg);
    equation21->setCoefficient("a32", turntableWindows[turntableSidesMins[6]].yGyroAvg);
    equation21->setCoefficient("a33", turntableWindows[turntableSidesMins[6]].zGyroAvg);
    equation21->setCoefficient("b3", 1);
    linearEquations.addEquation(equation21);

    linearEquations.printStatistics();

    Solution* solution = linearEquations.solveWithSVD(0.0);
    //solution->print();

    for (int i = 0; i < 7; i++){
        //application.dataRecorder.setGyroIdleWindowStart(i, turntableWindows[turntableSidesMins[i]].start);
    }

    QString returnMessage = "";

    if ( solution->getMaximumError() > 0.2 ) {
        //msgBox.setText("Calibration Error! See console for details...");
        //msgBox.exec();
        returnMessage = "Maximum error is larger than threshold! ( > 0.2 )  \n";
        return returnMessage;
    } else {
        for (unsigned int i = 0; i < linearEquations.getVariableCount(); i++) {
            qDebug() << solution->getValueAt(i);
            solutions.append(solution->getValueAt(i));
        }

        returnMessage.append("\nGyroscope calibration successful \n");

        return returnMessage;
    }
}

QString TurntableWindow::toString() const
{
    QString s = "Idle Window: \n";

    s += "Time: " + QString::number(time / C_HZ) + "\n";

    s += "xGyroAvg:" + QString::number(xGyroAvg)
         + "  yGyroAvg:" + QString::number(yGyroAvg)
         + "  zGyroAvg:" + QString::number(zGyroAvg);

        return s + "\n";
}

void TurntableCalibrationModule::clearWindows()
{
        turntableWindows.clear();
}

void TurntableCalibrationModule::clearTurntableSides()
{
    for (int i = 0; i < 7; i++) {
        turntableSidesMins[i] = 0;
    }
}

void TurntableCalibrationModule::printMatrix1D(TNT::Array1D<double> matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        std::cout << matrix[i] <<"\n";
    }
    std::cout << "\n" << flush;
}

void TurntableCalibrationModule::printMatrix2D(TNT::Array2D<double> matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        for (int j = 0; j < matrix.dim2(); j++) {
            std::cout << matrix[i][j] <<"\t";
        }
        std::cout << "\n";
    }
    std::cout << flush;
}
