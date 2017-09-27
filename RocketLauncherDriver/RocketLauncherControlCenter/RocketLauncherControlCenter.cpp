#include "RocketLauncherControlCenter.h"
#include<iostream>
#include "include/RocketLauncherDevice.hpp"
#include "include/RocketLauncherDeviceSim.h"

using namespace rocketlauncher;

RocketLauncherControlCenter::RocketLauncherControlCenter(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	device = new RocketLauncherDeviceSim();

	QObject::connect(this->ui.pushButton, &QPushButton::clicked, this, &RocketLauncherControlCenter::buttonClicked);
	QObject::connect(this, &RocketLauncherControlCenter::positionChanged, this->ui.label, &QLabel::setText);

	emit positionChanged(QString::number(device->getEstimatedPanPosition()));
}

void RocketLauncherControlCenter::buttonClicked()
{
	std::cout << "Clicked";
}
