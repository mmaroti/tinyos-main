#include "curvedata.h"
#include "MoteData.h"
#include "qwt_series_data.h"

CurveData::CurveData(): boundingRectData(1.0, 1.0, -2.0, -2.0) // invalid
{}

CurveData::~CurveData()
{
    values.clear();
}

void CurveData::append(const QPointF &sample)
{
    values.append(sample);

    // adjust the bounding rectangle

    if ( boundingRectData.width() < 0 || boundingRectData.height() < 0 )
    {
        boundingRectData.setRect(sample.x(), sample.y(), 0.0, 0.0);
    }
    else
    {
        boundingRectData.setRight(sample.x());

        if ( sample.y() > boundingRectData.bottom() )
            boundingRectData.setBottom(sample.y());

        if ( sample.y() < boundingRectData.top() )
            boundingRectData.setTop(sample.y());
    }
}


//const MoteData &CurveData::values() const
//{
//    return MoteData::instance();
//}
//
//MoteData &CurveData::values()
//{
//    return MoteData::instance();
//}


QPointF CurveData::sample(size_t i) const
{
    return values[i];
}

size_t CurveData::size() const
{
    return values.size();
}

QRectF CurveData::boundingRect() const
{
    return boundingRectData;
}
