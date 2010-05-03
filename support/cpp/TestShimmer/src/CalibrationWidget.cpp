#include "CalibrationWidget.h"
#include "ui_CalibrationWidget.h"
#include "DataRecorder.h"
#include "CalibrationModule.h"
#include "Application.h"
#include <qfiledialog.h>

CalibrationWidget::CalibrationWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget),
    application(app)
{
    ui->setupUi(this);
    calibrationModule = new CalibrationModule(app);
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
	ui->retranslateUi(this);
	break;
    default:
	break;
    }
}

void CalibrationWidget::on_startButton_clicked()
{
    QString message = "";
    if ( ui->stationaryButton->isChecked() ) {
        if ( application.dataRecorder.size() < WINDOW*6 ) {
            message = "Error: Not enough data for calibration! Please Load a longer record!";
        } else {
            if ( calibrationModule->Calibrate() == "1" ) {
                for (int i = 0; i < 6; i++){
                    message.append(calibrationModule->at(calibrationModule->atIdleSides(i)).toString());
                    message.append("\n");
                    }
                QString fn = QFileDialog::getSaveFileName(  this, "Choose a filename to save under", "c:/", "CSV (*.csv)");
                if ( !fn.isEmpty() ) {
                   // application.dataRecorder.saveSamples( fn );
                    calibrationModule->saveCalibratedData( fn );
                }
            } else {
                message = "Calibration Error!";
            }
        }
    } else {
        message = "Please select a module!";
    }
    ui->calibrationResults->setText(message);
    message.clear();
    calibrationModule->clearWindows();
    calibrationModule->clearIdleSides();
}
