#include "CalibratedDataWidget.h"
#include "ui_CalibratedDataWidget.h"
#include "CalibratedDataPlot.h"
#include "Application.h"
#include "QtDebug"
#include <qfiledialog.h>
#include "cmath"

CalibratedDataWidget::CalibratedDataWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::CalibratedDataWidget),
    application(app)
{
        ui->setupUi(this);
        plot = new CalibratedDataPlot(ui->scrollArea, app);
        ui->scrollArea->setWidget(plot);
}

CalibratedDataWidget::~CalibratedDataWidget()
{
        delete ui;
}

void CalibratedDataWidget::changeEvent(QEvent *e)
{
        QWidget::changeEvent(e);
        switch (e->type())
        {
        case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
        default:
                break;
        }
}

void CalibratedDataWidget::on_exportButton_clicked()
{
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
       
        QFile f( fn );
    
        if( !f.open( QIODevice::WriteOnly ) )
          {
              return;
          }

        QTextStream ts( &f );
        double xtemp, ytemp, ztemp, avgtemp, xyAngle, yzAngle, zxAngle;
        ts << "Accel_X,Accel_Y,Accel_Z,AVG_Accel,XY_Angle,YZ_Angle,ZX_Angle" << endl;
        for (int i = 0; i < application.dataRecorder.size(); i++) {
            xtemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(0) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(1) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(2) + plot->calibrationDataAt(9);
            ytemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(3) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(4) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(5) + plot->calibrationDataAt(10);
            ztemp = application.dataRecorder.at(i).xAccel * plot->calibrationDataAt(6) + application.dataRecorder.at(i).yAccel * plot->calibrationDataAt(7) + application.dataRecorder.at(i).zAccel * plot->calibrationDataAt(8) + plot->calibrationDataAt(11);
            avgtemp = sqrt( pow(xtemp, 2.0) + pow(ytemp, 2.0) + pow(ztemp, 2.0) );
            xyAngle = plot->calculateAngle(xtemp,ytemp);
            yzAngle = plot->calculateAngle(ytemp,ztemp);
            zxAngle = plot->calculateAngle(ztemp,xtemp);

            ts << xtemp << "," << ytemp << "," << ztemp << "," << avgtemp << "," << xyAngle << "," << yzAngle << "," << zxAngle << endl;
        }
    
        ts.flush();
        f.close();
    }

}

void CalibratedDataWidget::on_xAccel_clicked()
{
        plot->setGraphs(CalibratedDataPlot::XACCEL, ui->xAccel->checkState());
}

void CalibratedDataWidget::on_yAccel_clicked()
{
        plot->setGraphs(CalibratedDataPlot::YACCEL, ui->yAccel->checkState());
}

void CalibratedDataWidget::on_zAccel_clicked()
{
        plot->setGraphs(CalibratedDataPlot::ZACCEL, ui->zAccel->checkState());
}

void CalibratedDataWidget::on_avgAccel_clicked()
{
        plot->setGraphs(CalibratedDataPlot::AVGACC, ui->avgAccel->checkState());
}

void CalibratedDataWidget::on_xyangle_clicked()
{
        plot->setGraphs(CalibratedDataPlot::XYANGLE, ui->xyangle->checkState());
}

void CalibratedDataWidget::on_yzangle_clicked()
{
        plot->setGraphs(CalibratedDataPlot::YZANGLE, ui->yzangle->checkState());
}

void CalibratedDataWidget::on_zxangle_clicked()
{
        plot->setGraphs(CalibratedDataPlot::ZXANGLE, ui->zxangle->checkState());
}
