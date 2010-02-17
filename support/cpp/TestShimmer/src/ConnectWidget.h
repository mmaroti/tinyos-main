#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <QWidget>
#include <QHash>
#include <QIcon>
#include "SerialListener.h"

class Application;
class QRadioButton;
class QListWidgetItem;

namespace Ui {
	class ConnectWidget;
}

class ConnectWidget : public QWidget {
	Q_OBJECT
public:
	ConnectWidget(QWidget *parent, Application &app);
	~ConnectWidget();

signals:
	void portChanged(QString portName, int baudRate);

protected slots:
	void onReceiveMessage(const ActiveMessage & msg);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::ConnectWidget *ui;
	QIcon moteIcon;

	QHash<QString, QRadioButton*> portButtons;
	QHash<int, QListWidgetItem*> moteItems;

private slots:
	void on_refreshButton_clicked();
	void on_disconnectButton_clicked();
	void on_connectButton_clicked();
};

#endif // CONNECTWIDGET_H
