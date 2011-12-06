#ifndef APPLICATION_H
#define APPLICATION_H

#include "MoteDataHolder.h"
#include "StationaryCalibrationModule.h"
#include "TurntableCalibrationModule.h"

class Application
{
public:
    Application();

    MoteDataHolder moteDataHolder;
    StationaryCalibrationModule statCalMod;
    TurntableCalibrationModule turnCalMod;
};

#endif // APPLICATION_H
