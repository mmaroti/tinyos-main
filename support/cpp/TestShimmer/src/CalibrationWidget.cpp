#include "CalibrationWidget.h"
#include "ui_CalibrationWidget.h"
#include "DataRecorder.h"
#include "CalibrationModule.h"
#include "PeriodicalCalibrationModule.h"
#include "Application.h"
#include <qfiledialog.h>

CalibrationWidget::CalibrationWidget(QWidget *parent, Application &app) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget),
    application(app)
{
    ui->setupUi(this);

    calibrationModule = new CalibrationModule(app);
    periodicalCalibrationModule = new PeriodicalCalibrationModule(app, *calibrationModule);

    QString message = "";
    message.append("Calibration data stored in system: \n");
    message.append("\n Accelerometer Calibration Data: \n");

    int size = application.settings.beginReadArray("calibrationData");    
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        message.append( application.settings.value("calibrationData").toString() + "\n" );
        calibrationModule->setCalibrationDataAt(i, application.settings.value("calibrationData").toDouble() );
    }
    application.settings.endArray();

    message.append("\n Gyroscope Calibration Data: \n");
    size = application.settings.beginReadArray("gyroCalibrationData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        message.append( application.settings.value("gyroCalibrationData").toString() + "\n" );
    }
    application.settings.endArray();

    message.append("\n Gyroscope Avarages Data: \n");
    size = application.settings.beginReadArray("gyroAvgsData");
    for (int i = 0; i < size; ++i) {
        application.settings.setArrayIndex(i);
        message.append( application.settings.value("gyroAvgsData").toString() + "\n");
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

            message.append("\nDONE...");
            ui->calibrationResults->setText(message);
            emit calibrationDone();

            for (int i = 0; i < 6; i++) {
                message.append(calibrationModule->at(calibrationModule->atIdleSides(i)).toString());
                message.append("\n");
            }

            message.append("Gyroscope averages: \n");
            for (int i = 0; i < 3; i++) {                
                message.append(QString::number(calibrationModule->getGyroAvgAt(i)));
                message.append("\n");
            }
            application.showConsoleMessage(message);
        }
    } else if ( ui->periodicalButton->isChecked() ) {
        periodicalCalibrationModule->Calibrate("x");
        periodicalCalibrationModule->Calibrate("y");
        message.append(periodicalCalibrationModule->Calibrate("z"));
        message.append(periodicalCalibrationModule->SVD());
        application.showConsoleMessage(message);
        emit calibrationDone();
    } else {
        message = "Please select a module!";
    }

    message.clear();
    calibrationModule->clearWindows();
    calibrationModule->clearIdleSides();
}
