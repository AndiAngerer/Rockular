/*
 * RocketLauncherDevice.hpp
 *
 *  Created on: 30.07.2013
 *      Author: Ludwig Naegele
 */

#ifndef ROCKETLAUNCHERDEVICE_HPP_
#define ROCKETLAUNCHERDEVICE_HPP_

#include "HidDevice.hpp"
#include "IRocketLauncherDevice.h"
#include <mutex>
#include <chrono>
#include <thread>

namespace rocketlauncher
{

	

	struct JointPositionData {
		Direction movement;
		double currentSpeed;

		// starts at 0 even if never adjusted
		double assumedPosition;

		// -1 if never adjusted, should be reset when a limit is reached
		double movedWaySinceCalibration;
	};

	struct PositionData {
		std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
		JointPositionData pan;
		JointPositionData tilt;
	};

	struct DeviceData
	{
		bool right = false, left = false, bottom = false, top = false, rocket1Present = false, rocket2Present = false, rocket3Present = false; // output sent by device
	};

	struct CommandData
	{
		Direction pan;
		Direction tilt;
		bool fire;
	};

	const std::string dev_rocketlauncher = "rocketlauncher";

	// cycle time for control
	const int cycleTimeMs = 50;

	// angle velocity in rad/sec
	const double panMaxVel = 0.32;
	const double panAccel = 4;
	const double tiltMaxVel = 0.474;
	const double tiltAccel = 2;
	const double maxPan = 1.57;
	const double minPan = -1.57;
	const double maxTilt = 0.785;
	const double minTilt = -0.174;

	class RocketLauncherDevice : public IRocketLauncherDevice
	{
	private:
		std::thread updateThread;
		bool updateThreadRunning;
		bool cancelUpdateThread;
		DeviceData lastDeviceData;

		void updateCyclic();
	protected:
		PositionData positionData;
		Direction desiredPan;
		Direction desiredTilt;
		bool desiredFire;

		double updatePositionDataAndReturnDeltaPosition(std::chrono::microseconds deltaMicroseconds, JointPositionData& data, double maxAccel, double maxVelocity, double minPosition, double maxPosition);
		void startCyclicUpdate();
		void stopCyclicUpdate();

		virtual DeviceData readDevice(DeviceData last) = 0;
		virtual bool writeDevice(CommandData data) = 0;

	public:
		RocketLauncherDevice();
		virtual ~RocketLauncherDevice();

		void setMovement(Direction pan, Direction tilt);

		void setFiring(bool enabled);

		bool isLeft() const;
		bool isRight() const;
		bool isTop() const;
		bool isBottom() const;
		bool isRocket1Present() const;
		bool isRocket2Present() const;
		bool isRocket3Present() const;
		bool isPanCalibrated() const;
		bool isTiltCalibrated() const;
		double getEstimatedPanPosition() const;
		double getEstimatedTiltPosition() const;

		void updateHook();
	};

} /* namespace rocketlauncher */
#endif /* ROCKETLAUNCHERDEVICE_HPP_ */
