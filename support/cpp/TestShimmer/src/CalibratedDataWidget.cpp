#include "CalibratedDataWidget.h"
#include "ui_CalibratedDataWidget.h"
#include "CalibratedDataPlot.h"
#include "Application.h"
#include "QtDebug"
#include <qfiledialog.h>

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

void CalibratedDataWidget::on_recordButton_clicked()
{
        QString text = ui->recordButton->text();
        if( text == "Record" )
        {
                ui->recordButton->setText("Stop");
                connect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
        }
        else
        {
                ui->recordButton->setText("Record");
                disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));
        }
}

void CalibratedDataWidget::on_clearButton_clicked()
{
//	ui->recordButton->setText("Record");
//	disconnect(&application.serialListener, SIGNAL(receiveMessage(ActiveMessage)), &application.dataRecorder, SLOT(onReceiveMessage(ActiveMessage)));

        application.dataRecorder.clearMessages();
}

void CalibratedDataWidget::on_loadButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select one or more files to open", "c:/");
    if ( !file.isEmpty() ) {
        application.dataRecorder.loadSamples( file );
    }
    plot->CalibratedDataPlot::onSampleAdded();

}

void CalibratedDataWidget::on_saveButton_clicked()
{
    QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
    if ( !fn.isEmpty() ) {
        application.dataRecorder.saveSamples( fn );
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



void CalibratedDataWidget::on_xGyro_clicked()
{
        plot->setGraphs(CalibratedDataPlot::XGYRO, ui->xGyro->checkState());
}

void CalibratedDataWidget::on_yGyro_clicked()
{
        plot->setGraphs(CalibratedDataPlot::YGYRO, ui->yGyro->checkState());
}

void CalibratedDataWidget::on_zGyro_clicked()
{
        plot->setGraphs(CalibratedDataPlot::ZGYRO, ui->zGyro->checkState());
}

void CalibratedDataWidget::on_voltage_clicked()
{
        plot->setGraphs(CalibratedDataPlot::VOLTAGE, ui->voltage->checkState());
}

void CalibratedDataWidget::on_temp_clicked()
{
        plot->setGraphs(CalibratedDataPlot::TEMP, ui->temp->checkState());
}


