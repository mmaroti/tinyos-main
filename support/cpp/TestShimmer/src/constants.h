#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GRAV 9.81                       //gravitational acceleration
#define LAG_THRESHOLD 200               //the threshold for plotting lags from the radio

// ---===PERIODICAL CALIBRATION===---

#define NUMOFROT 10.0

// ---===STATIC CALIBRATION===---

//4G
/*enum LowSensitivity{
    MAXDIFF = 20,    //maximum difference between max and min values from an idle mote
    WINDOW = 100  //data window size, for finding idle shimmer positions
};*/

//1.5G
enum HighSensitivity{
    MAXDIFF = 60,    //maximum difference between max and min values from an idle mote
    WINDOW = 100  //data window size, for finding idle shimmer positions
};

#define xN   0.90*xMinAvg+0.1*xMaxAvg  //region borders for classifying idle windows
#define xZ_L 0.6*xMinAvg+0.4*xMaxAvg  //N = NEGATIVE_Gravity STATE
#define xZ_U 0.4*xMinAvg+0.6*xMaxAvg  //Z = ZERO_Gravity STATE
#define xP   0.1*xMinAvg+0.9*xMaxAvg  //P = POSITIVE_Gravity STATE
#define yN   0.9*yMinAvg+0.1*yMaxAvg  //L = lower border; U = upper border;
#define yZ_L 0.6*yMinAvg+0.4*yMaxAvg  //x,y,z : axis
#define yZ_U 0.4*yMinAvg+0.6*yMaxAvg
#define yP   0.1*yMinAvg+0.9*yMaxAvg
#define zN   0.9*zMinAvg+0.1*zMaxAvg
#define zZ_L 0.6*zMinAvg+0.4*zMaxAvg
#define zZ_U 0.4*zMinAvg+0.6*zMaxAvg
#define zP   0.1*zMinAvg+0.9*zMaxAvg


// ---===TURNTABLE CALIBRATION===---


#define GYROWINDOW 100  //data window size, for finding idle shimmer positions
#define GYROMAXDIFF 30  //maximum difference between max and min values from an idle mote
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
