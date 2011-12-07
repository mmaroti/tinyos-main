#ifndef MOTEDATA_H
#define MOTEDATA_H

#include <QVarLengthArray>
#include <QVector>
#include <QPoint>
#include <QRectF>
#include <qwt_series_data.h>

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
        QString toString() const;
        QString toCsvString() const;

        int moteID;
        double unix_time;
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
    static MoteData &instance();
    MoteData();
    ~MoteData();

    void setParam(Params param, int value);
    void setParam(Params param, double value);
    void setBootUnixTime(double value);

    void appendSample(Sample sample){
        samples.append(sample);
    }

    int getMoteID(){ return moteID; }
    int getRebootID(){ return rebootID; }
    double getLength(){ return length; }
    double getBootUnixTime(){ return boot_unix_time; }
    double getSkew1(){ return skew_1; }
    double getOffset(){ return offset; }

    QString getMoteHeader();
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

    void deleteSamplesFrom(int i, int count);

    void insertBlankSamples(int i, int x);

    void insertSampleAt(int i, const Sample &sample){
        samples.insert(i, sample);
    }

    void setTimeDelay(double delay, int from);

    int size() const;

    //MoteData *instance();

    QPointF value(int index) const;

    void append(const QPointF &pos);

    QRectF boundingRect() const;

private:

    MoteData(const MoteData &);
    MoteData &operator=( const MoteData & );

    //virtual ~MoteData();

    QVector<Sample> samples;

    int moteID;
    int rebootID;
    double length;
    double boot_unix_time;
    double skew_1;
    double offset;

    class PrivateData;
    PrivateData *d_data;
};


#endif // MOTEDATA_H
