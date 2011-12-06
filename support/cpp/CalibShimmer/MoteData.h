#ifndef MOTEDATA_H
#define MOTEDATA_H

#include <QVarLengthArray>
#include <QVector>

//enum Coordinate { X, Y, Z };

enum Params{
    MOTEID,
    REBOOTID,
    LENGTH,
    BOOT_UNIX_TIME,
    SKEW_1,
    OFFSET
};

struct Sample
{
        Sample(); // FIXME

        int moteID;
        uint unix_time;
        long mote_time;
        int counter;

        int xAccel;
        int yAccel;
        int zAccel;
        int xGyro;
        int yGyro;
        int zGyro;
        int voltage;
        int temp;
};

class MoteData
{
public:
    MoteData();
    ~MoteData();

    void setParam(Params param, int value);
    void setParam(Params param, double value);
    void setBootUnixTime(uint value);

    void appendSample(Sample sample){
        samples.append(sample);
    }

    int getMoteID(){ return moteID; }
    int getRebootID(){ return rebootID; }
    double getLength(){ return length; }
    double getBootUnixTime(){ return boot_unix_time; }
    double getSkew1(){ return skew_1; }
    double getOffset(){ return offset; }

    Sample & getSampleAt(int i){ return samples[i];  }

    const Sample & getLastSample(){
        return samples[samples.size()-1];
    }

    const Sample & sampleAt(int i) const {
        return samples[i];
    }

    long int samplesSize() const {
        return samples.size();
    }

private:

    QVector<Sample> samples;

    int moteID;
    int rebootID;
    double length;
    double boot_unix_time;
    double skew_1;
    double offset;
};


#endif // MOTEDATA_H
