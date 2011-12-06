#ifndef MOTEDATAHOLDER_H
#define MOTEDATAHOLDER_H

#include <QObject>
#include <QVarLengthArray>
#include "MoteData.h"

class MoteDataHolder : public QObject {
    Q_OBJECT
public:
    MoteDataHolder();
    ~MoteDataHolder();

    void loadCSVData( QString filename );

    void createMoteDataFromCSV( const QString& line );
    const Sample createSample(const QString& str, bool load);

    int findMotePos( const MoteData& moteData );

    int motesCount() { return motes.size(); }

    MoteData* getMoteData(int id);

    MoteData* mote(int i){
            return motes[i];
    }

    void clearMotes() {
        motes.clear();
    }

    int findNearestSample(double time, int mote);

private:

    QVarLengthArray<MoteData*> motes;
};

#endif // MoteDataHolder_H
