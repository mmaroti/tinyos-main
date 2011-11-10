/** Copyright (c) 2010, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Miklós Maróti
* Author: Péter Ruzicska
*/

#include <sstream>
#include <stdexcept>
#include <assert.h>
#include "DataRecorder.h"
#include "Application.h"
#include "ActiveMessage.hpp"
#include <QtDebug>
#include <qfile.h>
#include <QStringList>
#include <QMessageBox>
//#include "DataPlot.h"
#include "constants.h"
#include <math.h>
//#include "Results.hpp"
//#include "EulerAngles.hpp"


namespace {

    //int RESERVED_SAMPLES = 100000;
}

DataRecorder::DataRecorder(Application &app) :
        //A(gyro::matrix3::identity()),
        //b(gyro::vector3(0,0,0)),
        application(app)
{
    currentMote = -1;

    samples.reserve(100000);
    //loadCalibrationData();
    for(int i=0; i<6; i++){
        accelIdleWindowStart[i] = -1;
    }
    for(int i=0; i<7; i++){
        gyroIdleWindowStart[i] = -1;
    }

    from = to = 0;
    copySamples.clear();
}

DataRecorder::~DataRecorder() {
}

int DataRecorder::getFirstTime() const {

	if( samples.isEmpty() )
                return 0;

	return samples[0].time;
}

int DataRecorder::getLastTime() const {

	int size = samples.size();

	if( size == 0 )
		return 0;

	return samples[size-1].time;
}

void DataRecorder::onReceiveMessage(const ActiveMessage & msg)
{
    // first type of messages
    if( msg.type == 0x37 && msg.payload.size() == 100 )
    {

            for(int start = 0; start < 100; start += 20)
            {
                    OnlineSample sample;

                    sample.time = msg.getUInt(start);
                    sample.xAccel = msg.getShort(start + 4);
                    sample.yAccel = msg.getShort(start + 6);
                    sample.zAccel = msg.getShort(start + 8);
                    sample.xGyro = msg.getShort(start + 10);
                    sample.yGyro = msg.getShort(start + 12);
                    sample.zGyro = msg.getShort(start + 14);
                    sample.voltage = msg.getShort(start + 16);
                    sample.temp = msg.getShort(start + 18);

    //			qDebug() << "sample " + sample.toString();

                    /*if(samples.size() > 2){
                        int lag = getLag(sample.time);
                        if(lag > MAX_DUMMY) lag = MAX_DUMMY;
                        for(int i=lag; i > 0; i--){
                            Sample dummy;
                            samples.append(dummy);
                        }
                    }*/
                    samples.append(sample);
            }
            emit sampleAdded();
    }
}

void DataRecorder::clearSamples()
{
	samples.clear();

        for(int i=0; i<6; i++){
            accelIdleWindowStart[i] = -1;
        }
        for(int i=0; i<7; i++){
            gyroIdleWindowStart[i] = -1;
        }
	emit samplesCleared();
}

OnlineSample::OnlineSample()
{
        time = 0;
        xAccel = 0;
        yAccel = 0;
        zAccel = 0;
        xGyro = 0;
        yGyro = 0;
        zGyro = 0;
        voltage = 0;
        temp = 0;
        XYangle = 0.0;
        YZangle = 0.0;
        ZXangle = 0.0;
        for (int i=1;i<=7;++i)
            rotmat[i] = 0.0;
        rotmat[0] = 1.0;
        rotmat[4] = 1.0;
        rotmat[8] = 1.0;
        for (int i=0; i<3; ++i)
            integrated_angle[i] = 0.0;
}

QString OnlineSample::toString() const
{
	QString s = "(";

	s += "time:" + QString::number(time);

	if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
		s += " accel: " + QString::number(xAccel)
			+ " " + QString::number(yAccel)
			+ " " + QString::number(zAccel);

	if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
		s += " qyro: " + QString::number(xGyro)
			+ " " + QString::number(yGyro)
			+ " " + QString::number(zGyro);

	if( voltage >= 0 )
		s += " volt:" + QString::number(voltage);

	if( temp >= 0 )
		s += " temp:" + QString::number(temp);

	return s + ')';
}

QString OnlineSample::toCsvString() const
{
        QString s = "";

        s += QString::number(time) + ",";

        if( xAccel >= 0 || yAccel >= 0 || zAccel >= 0 )
                s += QString::number(xAccel)
                        + "," + QString::number(yAccel)
                        + "," + QString::number(zAccel)
                        + ",";

        if( xGyro >= 0 || yGyro >= 0 || zGyro >= 0 )
                s += QString::number(xGyro)
                        + "," + QString::number(yGyro)
                        + "," + QString::number(zGyro)
                        + ",";;

        if( voltage >= 0 )
                s += QString::number(voltage) + ",";

        if( temp >= 0 )
                s += QString::number(temp) + ",";

        for (int k=0; k<9; ++k){
            s += QString::number(rotmat[k]) + ",";
        }


        return s;
}

void DataRecorder::csvToSample(const QString& str)
{
    QStringList list = str.split(",");
    QStringListIterator csvIterator(list);
    OnlineSample sample;

    if(csvIterator.hasNext()){
        sample.time = csvIterator.next().toInt();
        sample.xAccel = csvIterator.next().toInt();
        sample.yAccel = csvIterator.next().toInt();
        sample.zAccel = csvIterator.next().toInt();
        sample.xGyro = csvIterator.next().toInt();
        sample.yGyro = csvIterator.next().toInt();
        sample.zGyro = csvIterator.next().toInt();
        sample.voltage = csvIterator.next().toInt();
        sample.temp = csvIterator.next().toInt();
    }
    if(csvIterator.hasNext()){
        for(int i=0; i<9; i++){
            sample.rotmat[i] = csvIterator.next().toDouble();
        }
    }

    samples.append(sample);
}

void DataRecorder::saveSamples(const QString& filename) const {

    QFile f( filename );

    if( !f.open( QIODevice::WriteOnly ) )
      {
          return;
      }

    QTextStream ts( &f );

    ts << "#Time,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Volt,Temp,RotMat[9]" << endl;
    for (int i=0; i<samples.size(); i++){
      ts << samples[i].toCsvString() << endl;
    }

    double xtemp, ytemp, ztemp, avgtemp, xyAngle, yzAngle, zxAngle, xGyro, yGyro, zGyro;

    ts << "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" << endl;
    for (int i = 0; i < application.dataRecorder.size(); i++) {
        xtemp = calculateCalibratedValue("xAcc", i);
        ytemp = calculateCalibratedValue("yAcc", i);
        ztemp = calculateCalibratedValue("zAcc", i);
        avgtemp = calculateAbsAcc(i);
        xyAngle = atan2(xtemp,ytemp);
        yzAngle = atan2(ytemp,ztemp);
        zxAngle = atan2(ztemp,xtemp);
        xGyro = calculateCalibratedValue("xGyro", i);
        yGyro = calculateCalibratedValue("yGyro", i);
        zGyro = calculateCalibratedValue("zGyro", i);

        ts << xtemp << "," << ytemp << "," << ztemp << "," << avgtemp << "," << xyAngle << "," << yzAngle << "," << zxAngle << "," << xGyro << "," << yGyro << "," << zGyro << endl;
    }

    ts << "#Static Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(accelCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Calibration Data" << endl;
    for (int i = 0; i < 12; ++i) {
        ts << QString::number(gyroCalibrationData[i]) + "\n" ;
    }

    ts << "#Gyro Minimum Averages" << endl;
    for (int i = 0; i < 3; ++i) {
        ts << QString::number(gyroMinAvgs[i]) + "\n" ;
    }

    ts.flush();
    f.close();

}

void DataRecorder::loadSamples(const QString& filename )
{
    clearSamples();
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        line = ts.readLine(); //skip first line of csv

        if(line[0] != QChar('#')){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            line = ts.readLine();
            while ( !line.isEmpty() && line != "#Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle,Gyro_X,Gyro_Y,Gyro_Z" )
            {
                csvToSample(line);            //convert line string to sample
                line = ts.readLine();         // line of text excluding '\n'
            }
            f.close();
        }
    }
    emit sampleAdded();
}

void DataRecorder::loadCalibFromFile(const QString& filename )
{
    QFile f( filename );
    QString line;

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );

        line = ts.readLine();
        while ( !line.isEmpty() && line != "#Static Calibration Data" )
        {
            line = ts.readLine();         // line of text excluding '\n'
        }
        if(line != "#Static Calibration Data"){
            QMessageBox msgBox;
            msgBox.setText("Wrong file format!");
            msgBox.exec();
        } else {
            for(int i = 0; i < 12; i++){
                line = ts.readLine();         // line of text excluding '\n'
                accelCalibrationData[i] = line.toDouble();
            }
            line = ts.readLine();
            if(line != "#Gyro Calibration Data"){
                QMessageBox msgBox;
                msgBox.setText("Wrong file format!");
                msgBox.exec();
            } else {
                for(int i = 0; i < 12; i++){
                    line = ts.readLine();         // line of text excluding '\n'
                    gyroCalibrationData[i] = line.toDouble();
                }
                line = ts.readLine();
                if(line != "#Gyro Minimum Averages"){
                    QMessageBox msgBox;
                    msgBox.setText("Wrong file format!");
                    msgBox.exec();
                } else {
                    for(int i = 0; i<3; i++){
                        gyroMinAvgs[i] = line.toDouble();
                    }
                }
            }
        }
        f.close();
    }
    emit fileLoaded();
}

void DataRecorder::loadCalibrationData()
{
    Q_ASSERT(currentMote > -1);

    QFile f( "../calib/mote"+QString::number(currentMote)+"calib.csv" );

    if( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) //file opened successfully
    {
        QTextStream ts( &f );
        for(int i = 0; i<12; i++){
            accelCalibrationData[i] = ts.readLine().toDouble();
        }
        for(int i = 0; i<12; i++){
            gyroCalibrationData[i] = ts.readLine().toDouble();
        }
        for(int i = 0; i<3; i++){
            gyroMinAvgs[i] = ts.readLine().toDouble();
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("Mote "+QString::number(currentMote)+
                       " does not have any calibration data, you have to calibrate it first!");
        msgBox.exec();

        setCalibToZero();
    }
    f.close();

    emit calibrationDataLoaded();
}

void DataRecorder::saveCalibrationData() const
{
    Q_ASSERT(currentMote > -1);

    QFile fi( "../calib/mote"+QString::number(currentMote)+"calib.csv" );

    if( fi.open( QIODevice::WriteOnly ) ) {

        QTextStream ts( &fi );
        for(int i = 0; i<12; i++){
            ts << accelCalibrationData[i] << endl;
        }
        for(int i = 0; i<12; i++){
            ts << gyroCalibrationData[i] << endl;
        }
        for(int i = 0; i<3; i++){
            ts << gyroMinAvgs[i] << endl;
        }

        ts.flush();

    }

    fi.close();
}

void DataRecorder::clearCalibrationData()
{
    setCalibToZero();
}

void DataRecorder::setCalibToZero()
{
    for(int i=0; i<12; i++){
        accelCalibrationData[i] = 0;
    }
    for(int i=0; i<12; i++){
        gyroCalibrationData[i] = 0;
    }
    for(int i=0; i<3; i++){
        gyroMinAvgs[i] = 0;
    }
}

/*void DataRecorder::at(int i, double data[ipo::SIZE]) const {

    using namespace ipo;

    using namespace gyro;
    using gyro::X; using gyro::Y; using gyro::Z;

    if (i<0 || i>=application.dataRecorder.size()) {

        throw std::out_of_range("Index out of range!");
    }

    const Sample& s = samples[i];

    data[TIME_STAMP] = s.time;

    vector3 accel_measured(s.xAccel, s.yAccel, s.zAccel);
    matrix3 accel_gain(accelCalibrationData);
    vector3 accel_offset(accelCalibrationData+9);

    vector3 accel = accel_gain*accel_measured + accel_offset;

    data[ACCEL_X] = accel[X];
    data[ACCEL_Y] = accel[Y];
    data[ACCEL_Z] = accel[Z];

    vector3 w_measured(s.xGyro, s.yGyro, s.zGyro);
    matrix3 w_gain(gyroCalibrationData);
    vector3 w_offset(gyroCalibrationData+9);

    vector3 w = w_gain*w_measured + w_offset;

    // FIXME Sign of y
    data[GYRO_X] =  w[X];
    data[GYRO_Y] = -w[Y];
    data[GYRO_Z] =  w[Z];
}
*/

/*void DataRecorder::update_gyro_calib(const double s[12]) {

    using namespace gyro;

    // FIXME Sign of y
    const double A_[] = { gyroCalibrationData[0], gyroCalibrationData[1], gyroCalibrationData[2],
                         -gyroCalibrationData[3],-gyroCalibrationData[4],-gyroCalibrationData[5],
                          gyroCalibrationData[6], gyroCalibrationData[7], gyroCalibrationData[8] };

    matrix3 A(A_);

    matrix3 C = matrix3::identity() + matrix3(s);

    matrix3 gain = C*A;

    gain.copy_to(gyroCalibrationData);

    // FIXME Sign of y
    for (int k=3; k<=5; ++k)
        gyroCalibrationData[k] = -gyroCalibrationData[k];

    // FIXME Sign of y
    vector3 b(gyroCalibrationData[9], -gyroCalibrationData[10], gyroCalibrationData[11]);

    vector3 d = vector3(s+9);

    vector3 offset = C*b + d;

    offset.copy_to(gyroCalibrationData+9);

    // FIXME Sign of y
    gyroCalibrationData[10] = -gyroCalibrationData[10];

    dump_calibration_data();

    // FIXME Eliminate gyroCalibrationData
    this->A = matrix3(gyroCalibrationData);
    this->b = vector3(gyroCalibrationData+9);
}*/

/*double DataRecorder::time_step(int i) const {

    using namespace gyro;

    return (samples[i].time-samples[i-1].time)/TICKS_PER_SEC;
}
*/

/*gyro::vector3 DataRecorder::angular_rate(int i) const {

    using namespace gyro;

    const Sample& s = samples[i];
    const vector3 x(s.xGyro, s.yGyro, s.zGyro);
    return A*x+b;
}

void DataRecorder::corrected_angles() {

    using namespace gyro;
    using gyro::X; using gyro::Y; using gyro::Z;

    const int n = samples.size();

    if (n<2)
        return;

    for (int i=0; i<3; ++i)
        samples[0].corrected_angle[i] = 0.0;

    vector3 sum(0, 0, 0);

    for (int i=1; i<n; ++i) {

        const vector3 angle = ((angular_rate(i-1)+angular_rate(i))/2)*time_step(i);

        const double alpha = -samples[i].integrated_angle[X];
        const double s = sin(alpha);
        const double c = cos(alpha);

        const double x =   angle[X];
        const double y = c*angle[Y]-s*angle[Z];
        const double z = s*angle[Y]+c*angle[Z];

        const vector3 diff(x, y, z);

        sum += diff;

        sum.enforce_range_minus_pi_plus_pi();

        sum.copy_to(samples[i].corrected_angle);
    }
}*/

/*void DataRecorder::integrate_angles() {

    using namespace gyro;
    const int n = samples.size();

    if (n<2)
        return;

    for (int i=0; i<3; ++i)
        samples[0].integrated_angle[i] = 0.0;

    vector3 sum(0, 0, 0);

    for (int i=1; i<n; ++i) {
        const vector3 angle = ((angular_rate(i-1)+angular_rate(i))/2)*time_step(i);
        sum += angle;
        sum.enforce_range_minus_pi_plus_pi();
        sum.copy_to(samples[i].integrated_angle);
    }
}*/

/*void DataRecorder::loadResults(const ipo::Results& res, const int begin, const int end) {

    const int n = end-begin;

    for (int i=0; i<n; ++i) {

        const double* const m = res.matrix_at(i);

        Sample& s = samples[begin+i];

        for (int k=0; k<9; ++k) {

            s.rotmat[k] = m[k];
        }
    }
    /*
    if (n==samples.size()) {

        update_gyro_calib(res.var());

        integrate_angles();

        corrected_angles();
    }
    */
//}

/*const Range DataRecorder::rangeInSample(const Range& rangeInSec, const double length) const {

    const double size = samples.size();   

    const double begin = rangeInSec.begin;

    const double end   = rangeInSec.end;

    if (!((size!=0) && (0<=begin && begin<end && end<=length))) {

        std::ostringstream os;
        os << "DataRecoder::range; begin, end, length (sec); size: ";
        os << begin << ", " << end << ", " << length << "; " << size << std::flush;

        throw std::logic_error(os.str());
    }

    const int begSample = static_cast<int>( (begin/length)*size );

    const int endSample = static_cast<int>( (end/length)*size );

    return Range(begSample, endSample);
}

double* DataRecorder::rotmat(const Range& range) const {

    double* const matrices = new double[9*range.size()];

    double* pos = matrices;

    for (int i=range.begin; i<range.end; ++i, pos+=9 ) {

        const double* const mat = samples[i].rotmat;

        for (int k=0; k<9; ++k) {

            pos[k] = mat[k];
        }
    }

    return matrices;
}*/

/*void DataRecorder::dump_calibration_data() const {

    std::cout << "Gyroscope calibration data is: " << std::endl;

    for (int k=0; k<12; ++k) {
        std::cout << gyroCalibrationData[k] << std::endl;
    }
}*/

/*bool DataRecorder::euler_angle(int i, Coordinate k, double& angle_rad) const {

    using namespace gyro;

    double euler[3];

    const bool degenerate = rotmat_to_angles_rad(samples[i].rotmat, euler);

    angle_rad = euler[k];

    return degenerate;
}*/

namespace {

    void fix_vertical_lines(double& a, double& b) {

        if (fabs(a-b)<0.75*2*M_PI)
            return;

        if(b<0)
            a -= 2*M_PI;
        else
            a += 2*M_PI;
    }
}

Angle_pair DataRecorder::corrected_angle(int i, Coordinate k) const {

    double a = samples[i-1].corrected_angle[k];
    double b = samples[i  ].corrected_angle[k];

    fix_vertical_lines(a, b);

    return Angle_pair(a, b);
}

Angle_pair DataRecorder::integrated_angle(int i, Coordinate k) const {

    double a = samples[i-1].integrated_angle[k];
    double b = samples[i  ].integrated_angle[k];

    fix_vertical_lines(a, b);

    return Angle_pair(a, b);
}

void DataRecorder::setAccelIdleWindowStart(int index, int start) {
    assert(0<=index && index < 6);
    accelIdleWindowStart[index] = start;
}

void DataRecorder::setGyroMinAvgs(int index, double value) {
    assert(0<=index && index < 3);
    gyroMinAvgs[index] = value;
}

void DataRecorder::setAccelCalibration(int index, double value) {
    assert(0<=index && index < 12);
    accelCalibrationData[index] = value;
}

void DataRecorder::setGyroCalibration(int index, double value) {
    assert(0<=index && index < 12);
    gyroCalibrationData[index] = value;
}

void DataRecorder::setGyroIdleWindowStart(int index, int start) {
    assert(0<=index && index < 7);// FIXME Why 7?
    gyroIdleWindowStart[index] = start;
}

int DataRecorder::getLag(int time) const
{
    int returnTime = 0;

    if( samples.size() > 2 ){
        int startTime = samples[samples.size()-1].time;
        if(startTime != 0){
            if(time-startTime > LAG_THRESHOLD){
                returnTime =  int((time -startTime) / 160);
            } else {
                returnTime = 0;
            }
        }
    } else {
        returnTime = 0;
    }

    return returnTime;
}

int DataRecorder::getTime(int i) const
{
    int returnTime = 0;

    if( (i < samples.size()) && (samples.size() > 2) ){
        int startTime = getFirstTime();
        if(startTime != 0){
            if(samples[i].time-startTime > LAG_THRESHOLD){
                returnTime =  int((samples[i].time -startTime) / 160);
            } else {
                returnTime = i;
            }
        }
    } else {
        returnTime = i;
    }

    return returnTime;
}

double DataRecorder::calculateAngle(double accel1, double accel2) const
{
    double alfa;
    if( (pow(accel1, 2.0) + pow(accel2, 2.0)) < pow(GRAV/2, 2.0) )
        return 10.0;

    if( fabs(accel1) > fabs(accel2) ) {
        alfa = atan(accel2/accel1);
        if( accel1 < 0 )
            alfa += M_PI;
    }
    else {
        alfa = M_PI/2 - atan(accel1/accel2);
        if( accel2 < 0 )
            alfa += M_PI;
    }

    if ( alfa >= M_PI) alfa -= 2*M_PI;
    if ( alfa < -M_PI) alfa += 2*M_PI;

    return alfa;
}

double DataRecorder::calculateCalibratedValue(QString axis, int time) const
{
    if( axis == "xAcc"){
        return (samples[time].xAccel * accelCalibrationData[0] + samples[time].yAccel * accelCalibrationData[1] + samples[time].zAccel * accelCalibrationData[2] + accelCalibrationData[9]);
    } else if( axis == "yAcc"){
        return (samples[time].xAccel * accelCalibrationData[3] + samples[time].yAccel * accelCalibrationData[4] + samples[time].zAccel * accelCalibrationData[5] + accelCalibrationData[10]);
    } else if( axis == "zAcc" ){
        return (samples[time].xAccel * accelCalibrationData[6] + samples[time].yAccel * accelCalibrationData[7] + samples[time].zAccel * accelCalibrationData[8] + accelCalibrationData[11]);
    } else if( axis == "xGyro" ){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[0] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[1] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[2] );
        return ( (samples[time].xGyro) * gyroCalibrationData[0] + (samples[time].yGyro) * gyroCalibrationData[1] + (samples[time].zGyro) * gyroCalibrationData[2] + gyroCalibrationData[9] );
    } else if( axis == "yGyro"){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[3] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[4] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[5] );
        return ( (samples[time].xGyro) * gyroCalibrationData[3] + (samples[time].yGyro) * gyroCalibrationData[4] + (samples[time].zGyro) * gyroCalibrationData[5] + gyroCalibrationData[10] );
    } else if( axis == "zGyro"){
        //return ( (samples[time].xGyro - gyroMinAvgs[0]) * gyroCalibrationData[6] + (samples[time].yGyro - gyroMinAvgs[1]) * gyroCalibrationData[7] + (samples[time].zGyro - gyroMinAvgs[2]) * gyroCalibrationData[8] );
        return ( (samples[time].xGyro) * gyroCalibrationData[6] + (samples[time].yGyro) * gyroCalibrationData[7] + (samples[time].zGyro) * gyroCalibrationData[8] + gyroCalibrationData[11] );
    } else return -1;
}

double DataRecorder::calculateAbsAcc(int time) const
{
    double avg;
    double x, y ,z;

    x = calculateCalibratedValue("xAcc", time);
    y = calculateCalibratedValue("yAcc", time);
    z = calculateCalibratedValue("zAcc", time);

    avg = sqrt( pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0) );

    return avg;
}

void DataRecorder::edit(const QString& option)
{
    if(samples.size()< to){
        to = samples.size()-1;
    }

    if(option == "trim"){
        for(int i=from; i<=to; i++){
            copySamples.append(samples[i]);
        }
        samples.clear();
        emit samplesCleared();
        samples = copySamples;
        emit sampleAdded();
        copySamples.clear();
    } else
    if(option == "copy"){
        for(int i=from; i<=to; i++){            
            samples.append(samples[i]);
        }       
        emit sampleAdded();
    } else
    if(option == "cut"){
        for(int i=0; i<from; i++){
            copySamples.append(samples[i]);
        }
        for(int i=to; i<samples.size(); i++){
            copySamples.append(samples[i]);
        }
        samples.clear();
        emit samplesCleared();
        samples = copySamples;
        emit sampleAdded();
        copySamples.clear();
    }
}

MinMaxAvg DataRecorder::minMaxAvgOnRange(int start, int end, QString value)
{
    MinMaxAvg minMaxAvg;
    Coordinate c;
    double sum = 0;
    double min = 99999.9;
    double max = 0.0;

    if(value == "XCORRANG") c = X;
    if(value == "YCORRANG") c = Y;
    if(value == "ZCORRANG") c = Z;


    if(start == end){
        minMaxAvg.avg = 0.0;
        minMaxAvg.min = 0.0;
        minMaxAvg.max = 0.0;
    } else {
        for(int i = start; i<end; i++){
            Angle_pair a = corrected_angle(i, c);
            sum += a.angle1;
            if(a.angle1<min) min = a.angle1;
            if(a.angle1>max) max = a.angle1;
        }
        minMaxAvg.min = min;
        minMaxAvg.max = max;
        minMaxAvg.avg = sum / (end-start);
    }

    return minMaxAvg;
}
