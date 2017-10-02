#pragma once

#include <QtWidgets/QWidget>
#include "ui_RocketLauncherControlCenter.h"
#include "include/IRocketLauncherDevice.h"

using namespace rocketlauncher;

class RocketLauncherControlCenter : public QWidget
{
	Q_OBJECT

public:
	RocketLauncherControlCenter(QWidget *parent = Q_NULLPTR);

public slots:
	void buttonClicked();

signals:
	void positionChanged(QString value);
	
private:
	Ui::RocketLauncherControlCenterClass ui;
	IRocketLauncherDevice *device;
};
