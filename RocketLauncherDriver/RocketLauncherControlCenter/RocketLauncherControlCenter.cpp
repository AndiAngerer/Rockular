#include "RocketLauncherControlCenter.h"
#include<iostream>
#include "include/IRocketLauncherDevice.h"

using namespace rocketlauncher;

RocketLauncherControlCenter::RocketLauncherControlCenter(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	device = rocketlauncher::GetSimulatedRocketLauncher();

	QObject::connect(this->ui.pushButton, &QPushButton::clicked, this, &RocketLauncherControlCenter::buttonClicked);
	QObject::connect(this, &RocketLauncherControlCenter::positionChanged, this->ui.label, &QLabel::setText);

	emit positionChanged(QString::number(device->getEstimatedPanPosition()));
}

void RocketLauncherControlCenter::buttonClicked()
{
	device->setMovement(Direction::POSITIVE, Direction::POSITIVE);
}
