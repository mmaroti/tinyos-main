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

    QString message = "";
    int size = application.settings.beginReadArray("calibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);

        message.append( application.settings.value("calibrationData").toString() + "\n" );
        calibrationModule->setCalibrationDataAt(i, application.settings.value("calibrationData").toDouble() );
    }
    application.settings.endArray();
    ui->calibrationResults->setText(message);
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
            message = calibrationModule->Calibrate();
            for (int i = 0; i < 6; i++) {
                message.append(calibrationModule->at(calibrationModule->atIdleSides(i)).toString());
                message.append("\n");
            }
            message.append("Gyroscope avarages: \n");
            for (int i = 0; i < 3; i++) {                
                message.append(QString::number(calibrationModule->getGyroAvgAt(i)));
                message.append("\n");
            }
            application.showConsolMessage(message);
            message = "Done. See Console for details.";

        }
    } else {
        message = "Please select a module!";
    }
    ui->calibrationResults->setText(message);
    message.clear();
    calibrationModule->clearWindows();
    calibrationModule->clearIdleSides();
}
