#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GRAV 9.81                       //gravitational acceleration

// ---===PERIODICAL CALIBRATION===---

#define NUMOFROT 10.0

// ---===STATIC CALIBRATION===---

/*enum LowSensitivity{
    MAXDIFF = 20,    //maximum difference between max and min values from an idle mote
    WINDOW = 100  //data window size, for finding idle shimmer positions
};*/

enum HighSensitivity{
    MAXDIFF = 60,    //maximum difference between max and min values from an idle mote
    WINDOW = 200  //data window size, for finding idle shimmer positions
};

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


// ---===TURNTABLE CALIBRATION===---


#define GYROWINDOW 200  //data window size, for finding idle shimmer positions
#define GYROMAXDIFF 20  //maximum difference between max and min values from an idle mote
#define xCCLW   0.9*xMinGyroAvg+0.1*xMaxGyroAvg  //region borders for classifying idle windows
#define xI_L    0.7*xMinGyroAvg+0.3*xMaxGyroAvg  //CLW = ClockWise rotation
#define xI_U    0.3*xMinGyroAvg+0.7*xMaxGyroAvg  //CCLW = counter ClockWise rotation
#define xCLW    0.1*xMinGyroAvg+0.9*xMaxGyroAvg  //I = Idle
#define yCCLW   0.9*yMinGyroAvg+0.1*yMaxGyroAvg  //L = lower border; U = upper border;
#define yI_L    0.7*yMinGyroAvg+0.3*yMaxGyroAvg  //x,y,z : axis
#define yI_U    0.3*yMinGyroAvg+0.7*yMaxGyroAvg
#define yCLW    0.1*yMinGyroAvg+0.9*yMaxGyroAvg
#define zCCLW   0.9*zMinGyroAvg+0.1*zMaxGyroAvg
#define zI_L    0.7*zMinGyroAvg+0.3*zMaxGyroAvg
#define zI_U    0.3*zMinGyroAvg+0.7*zMaxGyroAvg
#define zCLW    0.1*zMinGyroAvg+0.9*zMaxGyroAvg

#endif // CONSTANTS_H
