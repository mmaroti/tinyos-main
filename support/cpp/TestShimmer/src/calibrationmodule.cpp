#include "CalibrationModule.h"
#include "DataRecorder.h"
#include <QtDebug>
#include "qfile.h"
#include "LinearEquations.h"

using namespace std;

CalibrationModule::CalibrationModule(Application &app) : application(app)
{
    xMin = 9999; yMin = 9999; zMin = 9999;
    xMax = 0; yMax = 0; zMax = 0;
    xDiff = 0; yDiff = 0; zDiff = 0;
    xAvg = 0.0; yAvg = 0.0; zAvg = 0.0;
    xMinAvg = 9999.99; xMaxAvg = 0.0; yMinAvg = 9999.99; yMaxAvg = 0.0; zMinAvg = 9999.99; zMaxAvg = 0.0;
    xSum = 0; ySum = 0; zSum = 0;
}

CalibrationModule::~CalibrationModule()
{
    clearWindows();
    clearIdleSides();
}

IdleWindow::IdleWindow()
{
    xMin = -1;
    xMax = -1;
    xAvg = -1;
    yMin = -1;
    yMax = -1;
    yAvg = -1;
    zMin = -1;
    zMax = -1;
    zAvg = -1;
}

QString CalibrationModule::Calibrate()
{
    int xAccel,yAccel,zAccel;


    for(int j = 0; j < application.dataRecorder.size()-WINDOW; j+=10) {
        for(int i = j; i < j + WINDOW; i++){
            xAccel = application.dataRecorder.at(i).xAccel;
            yAccel = application.dataRecorder.at(i).yAccel;
            zAccel = application.dataRecorder.at(i).zAccel;

            if(xAccel > xMax) xMax = xAccel; if(xAccel < xMin) xMin = xAccel;
            if(yAccel > yMax) yMax = yAccel; if(yAccel < yMin) yMin = yAccel;
            if(zAccel > zMax) zMax = zAccel; if(zAccel < zMin) zMin = zAccel;
            xSum = xSum + xAccel;
            ySum = ySum + yAccel;
            zSum = zSum + zAccel;
        }
        xAvg = (float) xSum/WINDOW;
        yAvg = (float) ySum/WINDOW;
        zAvg = (float) zSum/WINDOW;
        if(xAvg > xMaxAvg) xMaxAvg = xAvg; if(xAvg < xMinAvg) xMinAvg = xAvg;
        if(yAvg > yMaxAvg) yMaxAvg = yAvg; if(yAvg < yMinAvg) yMinAvg = yAvg;
        if(zAvg > zMaxAvg) zMaxAvg = zAvg; if(zAvg < zMinAvg) zMinAvg = zAvg;
        xDiff = xMax - xMin;
        yDiff = yMax - yMin;
        zDiff = zMax - zMin;
        if( (xDiff < MAXDIFF) && (yDiff < MAXDIFF) && (zDiff < MAXDIFF) ){
            IdleWindow idleWindow;

            idleWindow.xMax = xMax; idleWindow.xMin = xMin; idleWindow.xAvg = xAvg;
            idleWindow.yMax = yMax; idleWindow.yMin = yMin; idleWindow.yAvg = yAvg;
            idleWindow.zMax = zMax; idleWindow.zMin = zMin; idleWindow.zAvg = zAvg;
            idleWindows.append(idleWindow);
        }

        xMin = 9999; yMin = 9999; zMin = 9999;
        xMax = 0; yMax = 0; zMax = 0;
        xDiff = 0; yDiff = 0; zDiff = 0;
        xAvg = 0.0; yAvg = 0.0; zAvg = 0.0;
        xSum = 0; ySum = 0; zSum = 0;
    }

    if (idleWindows.size() < 6){
        return "Please load a data record with the mote being idle on each side for at least a 2 seconds!";
    } else {
        return Classify();
    }
}

QString CalibrationModule::Classify()
{
    float xTemp,yTemp,zTemp;
    int xPMinDiff = 3*MAXDIFF; int xNMinDiff = 3*MAXDIFF;
    int yPMinDiff = 3*MAXDIFF; int yNMinDiff = 3*MAXDIFF;
    int zPMinDiff = 3*MAXDIFF; int zNMinDiff = 3*MAXDIFF;

    for (int i = 0; i < 6; i++) {
        idleSidesMins[i] = -1;
    }

    for (int i=0; i<idleWindows.size(); i++) {
        xTemp = idleWindows[i].xAvg; yTemp = idleWindows[i].yAvg; zTemp = idleWindows[i].zAvg;
        int actDiff = (idleWindows[i].xMax-idleWindows[i].xMin)+(idleWindows[i].yMax-idleWindows[i].yMin)+(idleWindows[i].zMax-idleWindows[i].zMin);
        if ( (xTemp >= xP) && (xTemp <= xMaxAvg) && (yTemp >= yZ_L) && (yTemp <= yZ_U ) && (zTemp >= zZ_L) && (zTemp <= zZ_U ) ) {
            //+x side
            if ( xPMinDiff > actDiff ) {
                xPMinDiff = actDiff;
                idleSidesMins[0] = i;
            }
        } else if ( (xTemp >= xMinAvg) && (xTemp <= xN) && (yTemp >= yZ_L) && (yTemp <= yZ_U ) && (zTemp >= zZ_L) && (zTemp <= zZ_U ) ) {
            //-x side
            if ( xNMinDiff > actDiff ) {
                xNMinDiff = actDiff;
                idleSidesMins[1] = i;
            }
        } else if ( (xTemp >= xZ_L) && (xTemp <= xZ_U ) && (yTemp >= yP) && (yTemp <= yMaxAvg) && (zTemp >= zZ_L) && (zTemp <= zZ_U ) ) {
            //+y side
            if ( yPMinDiff > actDiff ) {
                yPMinDiff = actDiff;
                idleSidesMins[2] = i;
            }
        } else if ( (xTemp >= xZ_L) && (xTemp <= xZ_U ) && (yTemp >= yMinAvg) && (yTemp <= yN) && (zTemp >= zZ_L) && (zTemp <= zZ_U ) ) {
            //-y side
            if ( yNMinDiff > actDiff ) {
                yNMinDiff = actDiff;
                idleSidesMins[3] = i;
            }
        } else if ( (xTemp >= xZ_L) && (xTemp <= xZ_U ) && (yTemp >= yZ_L) && (yTemp <= yZ_U ) && (zTemp >= zP) && (zTemp <= zMaxAvg) ) {
            //+z side
            if ( zPMinDiff > actDiff ) {
                zPMinDiff = actDiff;
                idleSidesMins[4] = i;
            }
        } else if ( (xTemp >= xZ_L) && (xTemp <= xZ_U ) && (yTemp >= yZ_L) && (yTemp <= yZ_U ) && (zTemp >= zMinAvg) && (zTemp <= zN) ) {
            //-z side
            if ( zNMinDiff > actDiff ) {
                zNMinDiff = actDiff;
                idleSidesMins[5] = i;
            }
        } else {
            //calibration error
            return "Idle window outside boundaries!";
        }
    }
    xMinAvg = 9999.99; xMaxAvg = 0.0; yMinAvg = 9999.99; yMaxAvg = 0.0; zMinAvg = 9999.99; zMaxAvg = 0.0;

    for (int i = 0; i < 6; i++) {
        if ( idleSidesMins[i] == -1 ) {
            return "Calibration is missing the mote being idle on one of its sides! (Please load a record with the mote being idle on each side for at least 2 seconds!)";
        }
    }

    return LSF();
}

#include "tnt_math_utils.h"

QString CalibrationModule::LSF() {

    LinearEquations linearEquations;

    Equation* equation1 = linearEquations.createEquation();
    equation1->setConstant(GRAV);
    equation1->setCoefficient("a11", idleWindows[idleSidesMins[0]].xAvg);
    equation1->setCoefficient("a12", idleWindows[idleSidesMins[0]].yAvg);
    equation1->setCoefficient("a13", idleWindows[idleSidesMins[0]].zAvg);
    equation1->setCoefficient("a21", 0);
    equation1->setCoefficient("a22", 0);
    equation1->setCoefficient("a23", 0);
    equation1->setCoefficient("a31", 0);
    equation1->setCoefficient("a32", 0);
    equation1->setCoefficient("a33", 0);
    equation1->setCoefficient("b1", 1.0);
    equation1->setCoefficient("b2", 0);
    equation1->setCoefficient("b3", 0);
    linearEquations.addEquation(equation1);

    Equation* equation2 = linearEquations.createEquation();
    equation2->setConstant(0.0);
    equation2->setCoefficient("a11", 0);
    equation2->setCoefficient("a12", 0);
    equation2->setCoefficient("a13", 0);
    equation2->setCoefficient("a21", idleWindows[idleSidesMins[0]].xAvg);
    equation2->setCoefficient("a22", idleWindows[idleSidesMins[0]].yAvg);
    equation2->setCoefficient("a23", idleWindows[idleSidesMins[0]].zAvg);
    equation2->setCoefficient("a31", 0);
    equation2->setCoefficient("a32", 0);
    equation2->setCoefficient("a33", 0);
    equation2->setCoefficient("b1", 0);
    equation2->setCoefficient("b2", 1);
    equation2->setCoefficient("b3", 0);
    linearEquations.addEquation(equation2);

    Equation* equation3 = linearEquations.createEquation();
    equation3->setConstant(0.0);
    equation3->setCoefficient("a11", 0);
    equation3->setCoefficient("a12", 0);
    equation3->setCoefficient("a13", 0);
    equation3->setCoefficient("a21", 0);
    equation3->setCoefficient("a22", 0);
    equation3->setCoefficient("a23", 0);
    equation3->setCoefficient("a31", idleWindows[idleSidesMins[0]].xAvg);
    equation3->setCoefficient("a32", idleWindows[idleSidesMins[0]].yAvg);
    equation3->setCoefficient("a33", idleWindows[idleSidesMins[0]].zAvg);
    equation3->setCoefficient("b1", 0);
    equation3->setCoefficient("b2", 0);
    equation3->setCoefficient("b3", 1);
    linearEquations.addEquation(equation3);

    Equation* equation4 = linearEquations.createEquation();
    equation4->setConstant(-GRAV);
    equation4->setCoefficient("a11", idleWindows[idleSidesMins[1]].xAvg);
    equation4->setCoefficient("a12", idleWindows[idleSidesMins[1]].yAvg);
    equation4->setCoefficient("a13", idleWindows[idleSidesMins[1]].zAvg);
    equation4->setCoefficient("a21", 0);
    equation4->setCoefficient("a22", 0);
    equation4->setCoefficient("a23", 0);
    equation4->setCoefficient("a31", 0);
    equation4->setCoefficient("a32", 0);
    equation4->setCoefficient("a33", 0);
    equation4->setCoefficient("b1", 1.0);
    equation4->setCoefficient("b2", 0);
    equation4->setCoefficient("b3", 0);
    linearEquations.addEquation(equation4);

    Equation* equation5 = linearEquations.createEquation();
    equation5->setConstant(0.0);
    equation5->setCoefficient("a11", 0);
    equation5->setCoefficient("a12", 0);
    equation5->setCoefficient("a13", 0);
    equation5->setCoefficient("a21", idleWindows[idleSidesMins[1]].xAvg);
    equation5->setCoefficient("a22", idleWindows[idleSidesMins[1]].yAvg);
    equation5->setCoefficient("a23", idleWindows[idleSidesMins[1]].zAvg);
    equation5->setCoefficient("a31", 0);
    equation5->setCoefficient("a32", 0);
    equation5->setCoefficient("a33", 0);
    equation5->setCoefficient("b1", 0);
    equation5->setCoefficient("b2", 1);
    equation5->setCoefficient("b3", 0);
    linearEquations.addEquation(equation5);

    Equation* equation6 = linearEquations.createEquation();
    equation6->setConstant(0.0);
    equation6->setCoefficient("a11", 0);
    equation6->setCoefficient("a12", 0);
    equation6->setCoefficient("a13", 0);
    equation6->setCoefficient("a21", 0);
    equation6->setCoefficient("a22", 0);
    equation6->setCoefficient("a23", 0);
    equation6->setCoefficient("a31", idleWindows[idleSidesMins[1]].xAvg);
    equation6->setCoefficient("a32", idleWindows[idleSidesMins[1]].yAvg);
    equation6->setCoefficient("a33", idleWindows[idleSidesMins[1]].zAvg);
    equation6->setCoefficient("b1", 0);
    equation6->setCoefficient("b2", 0);
    equation6->setCoefficient("b3", 1);
    linearEquations.addEquation(equation6);

    Equation* equation7 = linearEquations.createEquation();
    equation7->setConstant(0.0);
    equation7->setCoefficient("a11", idleWindows[idleSidesMins[2]].xAvg);
    equation7->setCoefficient("a12", idleWindows[idleSidesMins[2]].yAvg);
    equation7->setCoefficient("a13", idleWindows[idleSidesMins[2]].zAvg);
    equation7->setCoefficient("a21", 0);
    equation7->setCoefficient("a22", 0);
    equation7->setCoefficient("a23", 0);
    equation7->setCoefficient("a31", 0);
    equation7->setCoefficient("a32", 0);
    equation7->setCoefficient("a33", 0);
    equation7->setCoefficient("b1", 1.0);
    equation7->setCoefficient("b2", 0);
    equation7->setCoefficient("b3", 0);
    linearEquations.addEquation(equation7);

    Equation* equation8 = linearEquations.createEquation();
    equation8->setConstant(GRAV);
    equation8->setCoefficient("a11", 0);
    equation8->setCoefficient("a12", 0);
    equation8->setCoefficient("a13", 0);
    equation8->setCoefficient("a21", idleWindows[idleSidesMins[2]].xAvg);
    equation8->setCoefficient("a22", idleWindows[idleSidesMins[2]].yAvg);
    equation8->setCoefficient("a23", idleWindows[idleSidesMins[2]].zAvg);
    equation8->setCoefficient("a31", 0);
    equation8->setCoefficient("a32", 0);
    equation8->setCoefficient("a33", 0);
    equation8->setCoefficient("b1", 0);
    equation8->setCoefficient("b2", 1);
    equation8->setCoefficient("b3", 0);
    linearEquations.addEquation(equation8);

    Equation* equation9 = linearEquations.createEquation();
    equation9->setConstant(0.0);
    equation9->setCoefficient("a11", 0);
    equation9->setCoefficient("a12", 0);
    equation9->setCoefficient("a13", 0);
    equation9->setCoefficient("a21", 0);
    equation9->setCoefficient("a22", 0);
    equation9->setCoefficient("a23", 0);
    equation9->setCoefficient("a31", idleWindows[idleSidesMins[2]].xAvg);
    equation9->setCoefficient("a32", idleWindows[idleSidesMins[2]].yAvg);
    equation9->setCoefficient("a33", idleWindows[idleSidesMins[2]].zAvg);
    equation9->setCoefficient("b1", 0);
    equation9->setCoefficient("b2", 0);
    equation9->setCoefficient("b3", 1);
    linearEquations.addEquation(equation9);

    Equation* equation10 = linearEquations.createEquation();
    equation10->setConstant(0.0);
    equation10->setCoefficient("a11", idleWindows[idleSidesMins[3]].xAvg);
    equation10->setCoefficient("a12", idleWindows[idleSidesMins[3]].yAvg);
    equation10->setCoefficient("a13", idleWindows[idleSidesMins[3]].zAvg);
    equation10->setCoefficient("a21", 0);
    equation10->setCoefficient("a22", 0);
    equation10->setCoefficient("a23", 0);
    equation10->setCoefficient("a31", 0);
    equation10->setCoefficient("a32", 0);
    equation10->setCoefficient("a33", 0);
    equation10->setCoefficient("b1", 1.0);
    equation10->setCoefficient("b2", 0);
    equation10->setCoefficient("b3", 0);
    linearEquations.addEquation(equation10);

    Equation* equation11 = linearEquations.createEquation();
    equation11->setConstant(-GRAV);
    equation11->setCoefficient("a11", 0);
    equation11->setCoefficient("a12", 0);
    equation11->setCoefficient("a13", 0);
    equation11->setCoefficient("a21", idleWindows[idleSidesMins[3]].xAvg);
    equation11->setCoefficient("a22", idleWindows[idleSidesMins[3]].yAvg);
    equation11->setCoefficient("a23", idleWindows[idleSidesMins[3]].zAvg);
    equation11->setCoefficient("a31", 0);
    equation11->setCoefficient("a32", 0);
    equation11->setCoefficient("a33", 0);
    equation11->setCoefficient("b1", 0);
    equation11->setCoefficient("b2", 1);
    equation11->setCoefficient("b3", 0);
    linearEquations.addEquation(equation11);

    Equation* equation12 = linearEquations.createEquation();
    equation12->setConstant(0.0);
    equation12->setCoefficient("a11", 0);
    equation12->setCoefficient("a12", 0);
    equation12->setCoefficient("a13", 0);
    equation12->setCoefficient("a21", 0);
    equation12->setCoefficient("a22", 0);
    equation12->setCoefficient("a23", 0);
    equation12->setCoefficient("a31", idleWindows[idleSidesMins[3]].xAvg);
    equation12->setCoefficient("a32", idleWindows[idleSidesMins[3]].yAvg);
    equation12->setCoefficient("a33", idleWindows[idleSidesMins[3]].zAvg);
    equation12->setCoefficient("b1", 0);
    equation12->setCoefficient("b2", 0);
    equation12->setCoefficient("b3", 1);
    linearEquations.addEquation(equation12);

    Equation* equation13 = linearEquations.createEquation();
    equation13->setConstant(0.0);
    equation13->setCoefficient("a11", idleWindows[idleSidesMins[4]].xAvg);
    equation13->setCoefficient("a12", idleWindows[idleSidesMins[4]].yAvg);
    equation13->setCoefficient("a13", idleWindows[idleSidesMins[4]].zAvg);
    equation13->setCoefficient("a21", 0);
    equation13->setCoefficient("a22", 0);
    equation13->setCoefficient("a23", 0);
    equation13->setCoefficient("a31", 0);
    equation13->setCoefficient("a32", 0);
    equation13->setCoefficient("a33", 0);
    equation13->setCoefficient("b1", 1.0);
    equation13->setCoefficient("b2", 0);
    equation13->setCoefficient("b3", 0);
    linearEquations.addEquation(equation13);

    Equation* equation14 = linearEquations.createEquation();
    equation14->setConstant(0.0);
    equation14->setCoefficient("a11", 0);
    equation14->setCoefficient("a12", 0);
    equation14->setCoefficient("a13", 0);
    equation14->setCoefficient("a21", idleWindows[idleSidesMins[4]].xAvg);
    equation14->setCoefficient("a22", idleWindows[idleSidesMins[4]].yAvg);
    equation14->setCoefficient("a23", idleWindows[idleSidesMins[4]].zAvg);
    equation14->setCoefficient("a31", 0);
    equation14->setCoefficient("a32", 0);
    equation14->setCoefficient("a33", 0);
    equation14->setCoefficient("b1", 0);
    equation14->setCoefficient("b2", 1);
    equation14->setCoefficient("b3", 0);
    linearEquations.addEquation(equation14);

    Equation* equation15 = linearEquations.createEquation();
    equation15->setConstant(GRAV);
    equation15->setCoefficient("a11", 0);
    equation15->setCoefficient("a12", 0);
    equation15->setCoefficient("a13", 0);
    equation15->setCoefficient("a21", 0);
    equation15->setCoefficient("a22", 0);
    equation15->setCoefficient("a23", 0);
    equation15->setCoefficient("a31", idleWindows[idleSidesMins[4]].xAvg);
    equation15->setCoefficient("a32", idleWindows[idleSidesMins[4]].yAvg);
    equation15->setCoefficient("a33", idleWindows[idleSidesMins[4]].zAvg);
    equation15->setCoefficient("b1", 0);
    equation15->setCoefficient("b2", 0);
    equation15->setCoefficient("b3", 1);
    linearEquations.addEquation(equation15);

    Equation* equation16 = linearEquations.createEquation();
    equation16->setConstant(0.0);
    equation16->setCoefficient("a11", idleWindows[idleSidesMins[5]].xAvg);
    equation16->setCoefficient("a12", idleWindows[idleSidesMins[5]].yAvg);
    equation16->setCoefficient("a13", idleWindows[idleSidesMins[5]].zAvg);
    equation16->setCoefficient("a21", 0);
    equation16->setCoefficient("a22", 0);
    equation16->setCoefficient("a23", 0);
    equation16->setCoefficient("a31", 0);
    equation16->setCoefficient("a32", 0);
    equation16->setCoefficient("a33", 0);
    equation16->setCoefficient("b1", 1.0);
    equation16->setCoefficient("b2", 0);
    equation16->setCoefficient("b3", 0);
    linearEquations.addEquation(equation16);

    Equation* equation17 = linearEquations.createEquation();
    equation17->setConstant(0.0);
    equation17->setCoefficient("a11", 0);
    equation17->setCoefficient("a12", 0);
    equation17->setCoefficient("a13", 0);
    equation17->setCoefficient("a21", idleWindows[idleSidesMins[5]].xAvg);
    equation17->setCoefficient("a22", idleWindows[idleSidesMins[5]].yAvg);
    equation17->setCoefficient("a23", idleWindows[idleSidesMins[5]].zAvg);
    equation17->setCoefficient("a31", 0);
    equation17->setCoefficient("a32", 0);
    equation17->setCoefficient("a33", 0);
    equation17->setCoefficient("b1", 0);
    equation17->setCoefficient("b2", 1);
    equation17->setCoefficient("b3", 0);
    linearEquations.addEquation(equation17);

    Equation* equation18 = linearEquations.createEquation();
    equation18->setConstant(-GRAV);
    equation18->setCoefficient("a11", 0);
    equation18->setCoefficient("a12", 0);
    equation18->setCoefficient("a13", 0);
    equation18->setCoefficient("a21", 0);
    equation18->setCoefficient("a22", 0);
    equation18->setCoefficient("a23", 0);
    equation18->setCoefficient("a31", idleWindows[idleSidesMins[5]].xAvg);
    equation18->setCoefficient("a32", idleWindows[idleSidesMins[5]].yAvg);
    equation18->setCoefficient("a33", idleWindows[idleSidesMins[5]].zAvg);
    equation18->setCoefficient("b1", 0);
    equation18->setCoefficient("b2", 0);
    equation18->setCoefficient("b3", 1);
    linearEquations.addEquation(equation18);

//    linearEquations.printStatistics();

    Solution* solution = linearEquations.solveWithSVD(0.0);
    solution->print();

    application.settings.beginWriteArray("calibrationData");
    for (unsigned int i = 0; i < linearEquations.getVariableCount(); i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("calibrationData", solution->getValueAt(i));
    }
    application.settings.endArray();

    int size = application.settings.beginReadArray("calibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);

        qDebug() << application.settings.value("calibrationData");
        calibrationData[i] = application.settings.value("calibrationData").toDouble();
    }
    application.settings.endArray();

    //linearEquations.~LinearEquations();



    /*
    TNT::Array1D<double> b(18);

    b[0] = GRAV; b[1] = 0.0; b [2] = 0.0; b[3] = -GRAV; b[4] = 0.0; b [5] = 0.0;
    b[6] = 0.0; b[7] = GRAV; b [8] = 0.0; b[9] = 0.0; b[10] = -GRAV; b [11] = 0.0;
    b[12] = 0.0; b[13] = 0.0; b [14] = GRAV; b[15] = 0.0; b[16] = 0.0; b [17] = -GRAV;

    TNT::Array2D<double> A(18,12);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            if ( j == 0) {
                A[i*3+j][0] = idleWindows[idleSidesMins[i]].xAvg;
                A[i*3+j][1] = idleWindows[idleSidesMins[i]].yAvg;
                A[i*3+j][2] = idleWindows[idleSidesMins[i]].zAvg;
                A[i*3+j][3] = 0.0; A[i*3+j][4] = 0.0; A[i*3+j][5] = 0.0;
                A[i*3+j][6] = 0.0; A[i*3+j][7] = 0.0; A[i*3+j][8] = 0.0;
                A[i*3+j][9] = 1.0;
                A[i*3+j][10] = 0.0;
                A[i*3+j][11] = 0.0;
            } else if ( j == 1) {
                A[i*3+j][0] = 0.0; A[i*3+j][1] = 0.0; A[i*3+j][2] = 0.0;
                A[i*3+j][3] = idleWindows[idleSidesMins[i]].xAvg;
                A[i*3+j][4] = idleWindows[idleSidesMins[i]].yAvg;
                A[i*3+j][5] = idleWindows[idleSidesMins[i]].zAvg;
                A[i*3+j][6] = 0.0; A[i*3+j][7] = 0.0; A[i*3+j][8] = 0.0;
                A[i*3+j][9] = 0.0;
                A[i*3+j][10] = 1.0;
                A[i*3+j][11] = 0.0;
            } else if (j == 2) {
                A[i*3+j][0] = 0.0; A[i*3+j][1] = 0.0; A[i*3+j][2] = 0.0;
                A[i*3+j][3] = 0.0; A[i*3+j][4] = 0.0; A[i*3+j][5] = 0.0;
                A[i*3+j][6] = idleWindows[idleSidesMins[i]].xAvg;
                A[i*3+j][7] = idleWindows[idleSidesMins[i]].yAvg;
                A[i*3+j][8] = idleWindows[idleSidesMins[i]].zAvg;
                A[i*3+j][9] = 0.0;
                A[i*3+j][10] = 0.0;
                A[i*3+j][11] = 1.0;
            }
        }
    }

    printMatrix2D(A);
    printMatrix1D(b);

    TNT::Array1D<double> x(9);
    JAMA::QR<double> qr(A);

    TNT::Array2D<double> q,r;
    q = qr.getQ().copy();
    r = qr.getR().copy();
    printMatrix2D(q);
    printMatrix2D(r);
    x = qr.solve(b).copy();

    printMatrix1D(x);

    application.settings.beginWriteArray("calibrationData");
    for (int i = 0; i < x.dim(); i++) {
        application.settings.setArrayIndex(i);
        application.settings.setValue("calibrationData", x[i]);
    }
    application.settings.endArray();

    int size = application.settings.beginReadArray("calibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);

        qDebug() << application.settings.value("calibrationData");
        calibrationData[i] = application.settings.value("calibrationData").toDouble();
    }
    application.settings.endArray();

    if ( x.dim() == 0) {
        return "LSF error...";
    } else {
        return "1";
    }*/
    return "1";
}

QString IdleWindow::toString() const
{
    QString s = "Idle Window: \n";
    //QString x = "";

    //x+= QString::number(xAvg*100);
    s += "xAvg:" + QString::number(xAvg)
         + "  yAvg:" + QString::number(yAvg)
         + "  zAvg:" + QString::number(zAvg);

        return s + "\n";
        //return x + "\n";
}

void CalibrationModule::saveCalibratedData( QString filename )
{
    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );
    double xtemp, ytemp, ztemp;
    ts << "Accel_X,Accel_Y,Accel_Z" << endl;
    for (int i = 0; i < application.dataRecorder.size(); i++) {
        xtemp = application.dataRecorder.at(i).xAccel * calibrationData[0] + application.dataRecorder.at(i).yAccel * calibrationData[1] + application.dataRecorder.at(i).zAccel * calibrationData[2] + calibrationData[9];
        ytemp = application.dataRecorder.at(i).xAccel * calibrationData[3] + application.dataRecorder.at(i).yAccel * calibrationData[4] + application.dataRecorder.at(i).zAccel * calibrationData[5] + calibrationData[10];
        ztemp = application.dataRecorder.at(i).xAccel * calibrationData[6] + application.dataRecorder.at(i).yAccel * calibrationData[7] + application.dataRecorder.at(i).zAccel * calibrationData[8] + calibrationData[11];
        ts << xtemp << "," << ytemp << "," << ztemp << endl;
    }


    ts.flush();
    f.close();

}

void CalibrationModule::clearWindows()
{
        idleWindows.clear();
}

void CalibrationModule::clearIdleSides()
{
    for (int i = 0; i < 6; i++) {
        idleSidesMins[i] = 0;
    }
}

void CalibrationModule::printMatrix1D(TNT::Array1D<double> matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        std::cout << matrix[i] <<"\n";
    }
    std::cout << "\n" << flush;
}

void CalibrationModule::printMatrix2D(TNT::Array2D<double> matrix) {
    std::cout << "\n";
    for (int i = 0; i < matrix.dim1(); i++) {
        for (int j = 0; j < matrix.dim2(); j++) {
            std::cout << matrix[i][j] <<"\t";
        }
        std::cout << "\n";
    }
    std::cout << flush;
}
