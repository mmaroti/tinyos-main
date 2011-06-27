#ifndef CURVEDATA_H
#define CURVEDATA_H

#include <qwt_series_data.h>
#include <qpointer.h>
#include <QVector>
#include <QPointF>

class MoteData;
//class DataRecorder;

class CurveData: public QwtArraySeriesData<QPointF>
{
public:
    CurveData();
    ~CurveData();

    void append(const QPointF&);
    virtual QRectF boundingRect() const;
    //const MoteData &values() const;
    //MoteData &values();

    virtual QPointF sample(size_t i) const;
    virtual size_t size() const;

    //virtual QRectF boundingRect() const;

    QVector<QPointF> values;
    QRectF boundingRectData;
};

#endif
