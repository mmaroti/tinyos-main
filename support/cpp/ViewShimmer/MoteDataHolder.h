#ifndef MOTEDATAHOLDER_H
#define MOTEDATAHOLDER_H

#include <QVarLengthArray>
#include <QProgressBar>
#include "MoteData.h"

class Application;

class MoteDataHolder : public QObject {
    Q_OBJECT
public:
    MoteDataHolder(Application &app);
    ~MoteDataHolder();

    void loadCSVData( QString filename );
    void createMoteData( const QString& line );
    const Sample createSample(const QString& str, bool load);
    MoteData* findMoteID(int id);

    int motesCount() { return motes.size(); }

    MoteData* mote(int i){
            return motes[i]; // BUG Segmentation fault
    }

    void clearMotes() {
        motes.clear();
    }

    int findNearestSample(double time, int mote);

    void printMoteData(int id);
    void printMotesHeader();

public slots:
    //void on_loadButton_clicked();

signals:
    void loadFinished();


protected:
    void changeEvent(QEvent *e);

private:

    Application& application;
    QVarLengthArray<MoteData*> motes;
    //QProgressBar* progressBar;
    int progressCounter;
};

#endif // MoteDataHolder_H
