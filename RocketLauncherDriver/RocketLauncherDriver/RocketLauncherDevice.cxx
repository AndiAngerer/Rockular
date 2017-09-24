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

	RocketLauncherDevice::RocketLauncherDevice():
					hid_device(0x0416, 0x9391, 0)
	{
		pan = NONE;
		tilt = NONE;
		fire = false;

		right = left = bottom = top = false;
		rocket1Present = rocket2Present = rocket3Present = false;

		deviceOnline = false;

		positionData.timestamp = chrono::high_resolution_clock::now();
		positionData.pan.movement = pan;
		positionData.pan.assumedPosition = 0.0;
		positionData.pan.movedWaySinceCalibration = -1;
		positionData.pan.currentSpeed = 0.0;
		positionData.tilt.movement = tilt;
		positionData.tilt.assumedPosition = 0.0;
		positionData.tilt.movedWaySinceCalibration = -1;
		positionData.tilt.currentSpeed = 0.0;

		setupDataSent = false;

	}

	RocketLauncherDevice::~RocketLauncherDevice()
	{
		unsigned char send_feature_buffer[6];
		send_feature_buffer[0] = 0x00;
		send_feature_buffer[1] = 0x5F;
		send_feature_buffer[2] = 0x00;
		send_feature_buffer[3] = 0xe0;
		send_feature_buffer[4] = 0xff;
		send_feature_buffer[5] = 0xfe;
		hid_device.setFeature(send_feature_buffer, 6);
	}

	void RocketLauncherDevice::setMovement(Direction pan, Direction tilt)
	{
		this->pan = pan;
		this->tilt = tilt;
	}

	void RocketLauncherDevice::setFiring(bool enabled)
	{
		fire = enabled;
	}

	bool RocketLauncherDevice::isLeft() const
	{
		return left;
	}

	bool RocketLauncherDevice::isRight() const
	{
		return right;
	}

	bool RocketLauncherDevice::isTop() const
	{
		return top;
	}

	bool RocketLauncherDevice::isBottom() const
	{
		return bottom;
	}

	bool RocketLauncherDevice::isRocket1Present() const
	{
		return rocket1Present;
	}

	bool RocketLauncherDevice::isRocket2Present() const
	{
		return rocket2Present;
	}

	bool RocketLauncherDevice::isRocket3Present() const
	{
		return rocket3Present;
	}

	bool RocketLauncherDevice::isPanCalibrated() const
	{
		return positionData.pan.movedWaySinceCalibration!=-1;
	}

	bool RocketLauncherDevice::isTiltCalibrated() const
	{
		return positionData.tilt.movedWaySinceCalibration!=-1;
	}

	double RocketLauncherDevice::getEstimatedPanPosition() const
	{
		return positionData.pan.assumedPosition;
	}

	double RocketLauncherDevice::getEstimatedTiltPosition() const
	{
		return positionData.tilt.assumedPosition;
	}

	bool RocketLauncherDevice::startHook()
	{
		return true;
	}

	double RocketLauncherDevice::updatePositionDataAndReturnDeltaPosition(microseconds deltaMicroseconds, JointPositionData& data, double maxAccel, double maxVelocity, double minPosition, double maxPosition) {
		double deltaPosition = data.currentSpeed * deltaMicroseconds.count() / 10e6;
		if (data.movement!=POSITIVE && data.currentSpeed>0) maxAccel *= -1;
		else if (data.movement==NEGATIVE) maxAccel *=-1;
		if (data.movement==NONE && data.currentSpeed==0) maxAccel = 0;
		double currentSpeed = data.currentSpeed + (maxAccel * deltaMicroseconds.count() / 10e6);
		if (currentSpeed * data.currentSpeed < 0 && data.movement==NONE) currentSpeed = 0;
		data.currentSpeed = fmin(maxVelocity, fmax(-maxVelocity, currentSpeed));
		double assumedPosition = data.assumedPosition + deltaPosition;
		data.assumedPosition = fmin(fmax(assumedPosition, minPosition), maxPosition);
		return deltaPosition;
	}


	bool RocketLauncherDevice::sendFeature(unsigned char chr1, unsigned char chr2, unsigned char chr3, unsigned char chr4, unsigned char chr5)
	{
		unsigned char send_feature_buffer[6];
		send_feature_buffer[0] = 0x00;
		send_feature_buffer[1] = chr1;
		send_feature_buffer[2] = chr2;
		send_feature_buffer[3] = chr3;
		send_feature_buffer[4] = chr4;
		send_feature_buffer[5] = chr5;
		return hid_device.setFeature(send_feature_buffer, 6);
	}

	bool RocketLauncherDevice::getFeature(unsigned char *receive_feature_buffer)
	{
		unsigned char _receive_feature_buffer[6];
		_receive_feature_buffer[0] = 0x00;
		_receive_feature_buffer[1] = receive_feature_buffer[0];
		_receive_feature_buffer[2] = receive_feature_buffer[1];
		_receive_feature_buffer[3] = receive_feature_buffer[2];
		_receive_feature_buffer[4] = receive_feature_buffer[3];
		_receive_feature_buffer[5] = receive_feature_buffer[4];
		bool result = hid_device.getFeature(_receive_feature_buffer, 6);
		receive_feature_buffer[0] = _receive_feature_buffer[1];
		receive_feature_buffer[1] = _receive_feature_buffer[2];
		receive_feature_buffer[2] = _receive_feature_buffer[3];
		receive_feature_buffer[3] = _receive_feature_buffer[4];
		receive_feature_buffer[4] = _receive_feature_buffer[5];
		return result;
	}

	void RocketLauncherDevice::updateHook()
	{
		// Update position information
		microseconds deltaTime = duration_cast<microseconds>(high_resolution_clock::now() - positionData.timestamp);
		double deltaPan = updatePositionDataAndReturnDeltaPosition(deltaTime, positionData.pan, panAccel, panMaxVel, minPan, maxPan);
		double deltaTilt = updatePositionDataAndReturnDeltaPosition(deltaTime, positionData.tilt, tiltAccel, tiltMaxVel, minTilt, maxTilt);
		positionData.timestamp = high_resolution_clock::now();

		unsigned char receive_feature_buffer[5];

		if (!setupDataSent) { // || RTT::os::TimeService::Instance()->getSeconds(setupDataSent)>0.5) {
			if (!sendFeature(0x51, 0x01, 0x00, 0x00, 0x00)) {
				setDeviceOnline(false);
				return;
			}
			if (!getFeature(receive_feature_buffer)) {
				setDeviceOnline(false);
				return;
			}

			if (!sendFeature(0x5D, 0x00, 0x7F, 0xFF, 0xFE)) {
				setDeviceOnline(false);
				return;
			}
			if (!getFeature(receive_feature_buffer)) {
				setDeviceOnline(false);
				return;
			}
			cout << (int)receive_feature_buffer[1] << std::endl;

			setupDataSent = true;
		}


		while (true) {
			if (!sendFeature(0x5C, 0x00, 0x00, 0x00, 0x00)) {
				setDeviceOnline(false);
				return;
			}
			if (!getFeature(receive_feature_buffer)) {
				setDeviceOnline(false);
				return;
			}
			if (receive_feature_buffer[0]==0x5C) break;
			//std::cout << ".";
		}
		//std::cout << std::endl;

		// Interpret data
		rocket1Present = (receive_feature_buffer[1] & 0x01) == 0x00;
		rocket2Present = (receive_feature_buffer[1] & 0x02) == 0x00;
		rocket3Present = (receive_feature_buffer[1] & 0x40) == 0x00;
		right = (receive_feature_buffer[1] & 0x10) == 0x00;

		while (true) {
			if (!sendFeature(0x5E, 0x00, 0x00, 0x00, 0x00)) {
				setDeviceOnline(false);
				return;
			}
			if (!getFeature(receive_feature_buffer)) {
				setDeviceOnline(false);
				return;
			}
			if (receive_feature_buffer[0]==0x5E) break;
			//std::cout << "*";
		}
		//std::cout << std::endl;

		// Interpret data
		left = (receive_feature_buffer[1] & 0x20) == 0x00;
		top = (receive_feature_buffer[1] & 0x80) == 0x00;
		bottom = (receive_feature_buffer[1] & 0x40) == 0x00;

		if (pan==POSITIVE && right) pan = NONE;
		if (pan==NEGATIVE && left) pan = NONE;
		if (tilt==POSITIVE && top) tilt = NONE;
		if (tilt==NEGATIVE && bottom) tilt = NONE;

		unsigned char cmd = 0x00;
		if (pan==POSITIVE) cmd += 0x04;
		if (pan==NEGATIVE) cmd += 0x08;
		if (tilt==POSITIVE) cmd += 0x02;
		if (tilt==NEGATIVE) cmd += 0x01;
		if (fire) cmd += 0x10;
		if (!sendFeature(0x5F, cmd, 0xE0, 0xFF, 0xFE)) {
			setDeviceOnline(false);
			return;
		}

		//std::cout << top << " . " << bottom << std::endl;

		// Update position data
		positionData.pan.movement = pan;
		positionData.tilt.movement = tilt;

		if (positionData.pan.movedWaySinceCalibration!=-1) {
			positionData.pan.movedWaySinceCalibration += fabs(deltaPan);
		}
		if (positionData.tilt.movedWaySinceCalibration!=-1) {
			positionData.tilt.movedWaySinceCalibration += fabs(deltaTilt);
		}

		if (left) {
			positionData.pan.assumedPosition = minPan;
			positionData.pan.movedWaySinceCalibration = 0;
		}
		if (right) {
			positionData.pan.assumedPosition = maxPan;
			positionData.pan.movedWaySinceCalibration = 0;
		}
		if (top) {
			positionData.tilt.assumedPosition = maxTilt;
			positionData.tilt.movedWaySinceCalibration = 0;
		}
		if (bottom) {
			positionData.tilt.assumedPosition = minTilt;
			positionData.tilt.movedWaySinceCalibration = 0;
		}

		setDeviceOnline(true);
	}

	void RocketLauncherDevice::setDeviceOnline(bool online) {
		if (deviceOnline == online) return;
		this->deviceOnline = online;
		if (this->deviceOnline) {
			positionData.pan.movedWaySinceCalibration = -1;
			positionData.tilt.movedWaySinceCalibration = -1;
			setupDataSent = -1;
		}
		//std::cout << "Rocketlauncher went " << devicestate << std::endl;
	}

	bool RocketLauncherDevice::getDeviceOnline() const {
		return deviceOnline;
	}

} /* namespace rocketlauncher */
