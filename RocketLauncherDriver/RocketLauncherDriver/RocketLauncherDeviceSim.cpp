#include "stdafx.h"
#include "RocketLauncherDeviceSim.h"
#include <iostream>

namespace rocketlauncher {

	RocketLauncherDeviceSim::RocketLauncherDeviceSim()
	{
		firedCount = 0;
		lastPan = Direction::NONE;
		lastTilt = Direction::NONE;
		startCyclicUpdate();
	}

	RocketLauncherDeviceSim::~RocketLauncherDeviceSim()
	{
		stopCyclicUpdate();
	}

	DeviceData RocketLauncherDeviceSim::readDevice(DeviceData last) 
	{
		DeviceData data;
		data.bottom = positionData.tilt.assumedPosition <= minTilt && (lastTilt != Direction::POSITIVE);
		data.top = positionData.tilt.assumedPosition >= maxTilt && (lastTilt != Direction::NEGATIVE);
		data.left = positionData.pan.assumedPosition <= minPan && (lastPan != Direction::POSITIVE);
		data.right = positionData.pan.assumedPosition >= maxPan && (lastPan != Direction::NEGATIVE);
		data.rocket1Present = this->firedCount < 1;
		data.rocket2Present = this->firedCount < 2;
		data.rocket3Present = this->firedCount < 3;

		//std::cout << "Returning simulated device data" << std::endl;

		return data;
	}

	bool RocketLauncherDeviceSim::writeDevice(CommandData data)
	{
		this->lastPan = data.pan;
		this->lastTilt = data.tilt;

		//TODO: firing should take some time
		if (data.fire && firedCount < 3) 
		{
			++this->firedCount;
		}

		return true;
	}

	void RocketLauncherDeviceSim::setDeviceOnline(bool online) 
	{
		// does nothing
	}

	bool RocketLauncherDeviceSim::getDeviceOnline() const 
	{
		return true;
	}

} // namespace rocketlauncher
