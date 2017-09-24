/*
 * RocketLauncherDevice.cxx
 *
 *  Created on: 30.07.2013
 *      Author: Ludwig Naegele
 */

#include "stdafx.h"
#include "RocketLauncherDevice.hpp"
#include <math.h>
#include <iostream>

namespace rocketlauncher
{
	using namespace std;
	using namespace std::chrono;

	RocketLauncherDevice::RocketLauncherDevice()
	{
		desiredPan = NONE;
		desiredTilt = NONE;
		desiredFire = false;

		lastDeviceData = DeviceData();

		positionData.timestamp = chrono::high_resolution_clock::now();
		positionData.pan.movement = desiredPan;
		positionData.pan.assumedPosition = 0.0;
		positionData.pan.movedWaySinceCalibration = -1;
		positionData.pan.currentSpeed = 0.0;
		positionData.tilt.movement = desiredTilt;
		positionData.tilt.assumedPosition = 0.0;
		positionData.tilt.movedWaySinceCalibration = -1;
		positionData.tilt.currentSpeed = 0.0;

		this->cancelUpdateThread = false;
		this->updateThreadRunning = false;
	}

	RocketLauncherDevice::~RocketLauncherDevice()
	{
		stopCyclicUpdate();
	}

	void RocketLauncherDevice::startCyclicUpdate()
	{
		if (this->updateThreadRunning) return;
		this->updateThread = thread(&RocketLauncherDevice::updateCyclic, this);
		this->updateThreadRunning = true;
	}

	void RocketLauncherDevice::stopCyclicUpdate()
	{
		if (!this->updateThreadRunning) return;
		this->cancelUpdateThread = true;
		if (this->updateThread.joinable())
		{
			this->updateThread.join();
		}
	}

	void RocketLauncherDevice::updateCyclic()
	{
		while (!this->cancelUpdateThread)
		{
			time_point<high_resolution_clock> start = high_resolution_clock::now();

			updateHook();

			time_point<high_resolution_clock> end = high_resolution_clock::now();

			milliseconds timePassed = duration_cast<milliseconds>(end - start);

			milliseconds cycleDuration = milliseconds(cycleTimeMs);

			milliseconds sleepTime = cycleDuration - timePassed;

			std::this_thread::sleep_for(sleepTime);
		}
	}

	void RocketLauncherDevice::setMovement(Direction pan, Direction tilt)
	{
		this->desiredPan = pan;
		this->desiredTilt = tilt;
	}

	void RocketLauncherDevice::setFiring(bool enabled)
	{
		desiredFire = enabled;
	}

	bool RocketLauncherDevice::isLeft() const
	{
		return lastDeviceData.left;
	}

	bool RocketLauncherDevice::isRight() const
	{
		return lastDeviceData.right;
	}

	bool RocketLauncherDevice::isTop() const
	{
		return lastDeviceData.top;
	}

	bool RocketLauncherDevice::isBottom() const
	{
		return lastDeviceData.bottom;
	}

	bool RocketLauncherDevice::isRocket1Present() const
	{
		return lastDeviceData.rocket1Present;
	}

	bool RocketLauncherDevice::isRocket2Present() const
	{
		return lastDeviceData.rocket2Present;
	}

	bool RocketLauncherDevice::isRocket3Present() const
	{
		return lastDeviceData.rocket3Present;
	}

	bool RocketLauncherDevice::isPanCalibrated() const
	{
		return positionData.pan.movedWaySinceCalibration != -1;
	}

	bool RocketLauncherDevice::isTiltCalibrated() const
	{
		return positionData.tilt.movedWaySinceCalibration != -1;
	}

	double RocketLauncherDevice::getEstimatedPanPosition() const
	{
		return positionData.pan.assumedPosition;
	}

	double RocketLauncherDevice::getEstimatedTiltPosition() const
	{
		return positionData.tilt.assumedPosition;
	}

	double RocketLauncherDevice::updatePositionDataAndReturnDeltaPosition(microseconds deltaMicroseconds, JointPositionData& data, double maxAccel, double maxVelocity, double minPosition, double maxPosition)
	{
		double deltaPosition = data.currentSpeed * deltaMicroseconds.count() / 10e6;
		if (data.movement != POSITIVE && data.currentSpeed > 0) maxAccel *= -1;
		else if (data.movement == NEGATIVE) maxAccel *= -1;
		if (data.movement == NONE && data.currentSpeed == 0) maxAccel = 0;
		double currentSpeed = data.currentSpeed + (maxAccel * deltaMicroseconds.count() / 10e6);
		if (currentSpeed * data.currentSpeed < 0 && data.movement == NONE) currentSpeed = 0;
		data.currentSpeed = fmin(maxVelocity, fmax(-maxVelocity, currentSpeed));
		double assumedPosition = data.assumedPosition + deltaPosition;
		data.assumedPosition = fmin(fmax(assumedPosition, minPosition), maxPosition);
		return deltaPosition;
	}

	void RocketLauncherDevice::updateHook()
	{
		DeviceData data = readDevice(this->lastDeviceData);

		CommandData cmdData;

		if (desiredPan == POSITIVE && data.right) { cmdData.pan = NONE; }
		else { cmdData.pan = desiredPan; }
		if (desiredPan == NEGATIVE && data.left) { cmdData.pan = NONE; }
		else { cmdData.pan = desiredPan; }
		if (desiredTilt == POSITIVE && data.top) { cmdData.tilt = NONE; }
		else { cmdData.tilt = desiredTilt; }
		if (desiredTilt == NEGATIVE && data.bottom) { cmdData.tilt = NONE; }
		else { cmdData.tilt = desiredTilt; }

		bool success = writeDevice(cmdData);

		if (!success) return;

		// Update position information
		microseconds deltaTime = duration_cast<microseconds>(high_resolution_clock::now() - positionData.timestamp);
		double deltaPan = updatePositionDataAndReturnDeltaPosition(deltaTime, positionData.pan, panAccel, panMaxVel, minPan, maxPan);
		double deltaTilt = updatePositionDataAndReturnDeltaPosition(deltaTime, positionData.tilt, tiltAccel, tiltMaxVel, minTilt, maxTilt);
		positionData.timestamp = high_resolution_clock::now();

		//std::cout << top << " . " << bottom << std::endl;

		// Update position data
		positionData.pan.movement = cmdData.pan;
		positionData.tilt.movement = cmdData.tilt;

		if (positionData.pan.movedWaySinceCalibration != -1)
		{
			positionData.pan.movedWaySinceCalibration += fabs(deltaPan);
		}
		if (positionData.tilt.movedWaySinceCalibration != -1)
		{
			positionData.tilt.movedWaySinceCalibration += fabs(deltaTilt);
		}

		if (data.left)
		{
			positionData.pan.assumedPosition = minPan;
			positionData.pan.movedWaySinceCalibration = 0;
		}
		if (data.right)
		{
			positionData.pan.assumedPosition = maxPan;
			positionData.pan.movedWaySinceCalibration = 0;
		}
		if (data.top)
		{
			positionData.tilt.assumedPosition = maxTilt;
			positionData.tilt.movedWaySinceCalibration = 0;
		}
		if (data.bottom)
		{
			positionData.tilt.assumedPosition = minTilt;
			positionData.tilt.movedWaySinceCalibration = 0;
		}


	}

} /* namespace rocketlauncher */
