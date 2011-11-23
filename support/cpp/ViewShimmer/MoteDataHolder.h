#ifndef MOTEDATAHOLDER_H
#define MOTEDATAHOLDER_H

#include <QVarLengthArray>
#include "MoteData.h"
#include "ActiveMessage.hpp"

class Application;

class MoteDataHolder : public QObject {
    Q_OBJECT
public:
    MoteDataHolder(Application &app);
    ~MoteDataHolder();

    void loadCSVData( QString filename );
    void saveData( const QString& ) const;

    void createMoteDataFromCSV( const QString& line );
    void createMoteDataFromOnline( int moteID );
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

    void calculateOffset(int sample);

    void printMoteData(int id);
    void printMotesHeader();

public slots:
    //void on_loadButton_clicked();

    void createSampleFromOnline(const ActiveMessage & msg);

signals:
    void loadFinished();
    void sampleAdded(int mote);

protected:
    void changeEvent(QEvent *e);

private:

    Application& application;
    QVarLengthArray<MoteData*> motes;
    int progressCounter;
};

#endif // MoteDataHolder_H
