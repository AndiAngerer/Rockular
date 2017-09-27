#pragma once

#include <QtWidgets/QWidget>
#include "ui_RocketLauncherControlCenter.h"
#include "include/RocketLauncherDevice.hpp"
#include "include/RocketLauncherDeviceSim.h"

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
	RocketLauncherDevice *device;
};
