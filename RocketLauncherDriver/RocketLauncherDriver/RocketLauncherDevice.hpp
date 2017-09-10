/*
 * RocketLauncherDevice.hpp
 *
 *  Created on: 30.07.2013
 *      Author: Ludwig Naegele
 */

#ifndef ROCKETLAUNCHERDEVICE_HPP_
#define ROCKETLAUNCHERDEVICE_HPP_

#include "../../hidapi/driver/HidDevice.hpp"
#include <rcc/Device.hpp>
#include <rcc/DeviceFactory.hpp>

#include <rtt/os/TimeService.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/os/Mutex.hpp>

namespace rocketlauncher
{

	enum Direction {
		POSITIVE,
		NEGATIVE,
		NONE
	};

	struct JointPositionData {
		Direction movement;
		double currentSpeed;

		// starts at 0 even if never adjusted
		double assumedPosition;

		// -1 if never adjusted
		double movedWaySinceCalibration;
	};

	struct PositionData {
		RTT::os::TimeService::nsecs timestamp;
		JointPositionData pan;
		JointPositionData tilt;
	};

	const std::string dev_rocketlauncher = "rocketlauncher";

	// angle velocity in rad/sec
	const double panMaxVel = 0.32;
	const double panAccel = 4;
	const double tiltMaxVel = 0.474;
	const double tiltAccel = 2;
	const double maxPan = 1.57;
	const double minPan = -1.57;
	const double maxTilt = 0.785;
	const double minTilt = -0.174;

	class RocketLauncherDevice: public RPI::Device, public RTT::TaskContext
	{
	private:
		hidapi::HidDevice hid_device;
		RPI::DeviceState devicestate;
		Direction pan;
		Direction tilt;
		bool right, left, bottom, top, rocket1Present, rocket2Present, rocket3Present, fire;

		PositionData positionData;

		RTT::os::TimeService::nsecs setupDataSent;

		void setDeviceState(RPI::DeviceState);
		double updatePositionDataAndReturnDeltaPosition(RTT::os::TimeService::Seconds deltaTime, JointPositionData& data, double maxAccel, double maxVelocity, double minPosition, double maxPosition);

		bool sendFeature(unsigned char chr1, unsigned char chr2, unsigned char chr3, unsigned char chr4, unsigned char chr5);
		bool getFeature(unsigned char *send_feature_buffer);


	public:
		RocketLauncherDevice(std::string name, RPI::parameter_t parameters);
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

		bool startHook();
		void updateHook();

		static RocketLauncherDevice* createDevice(std::string name, RPI::parameter_t parameters);
		void updateParameters();
		std::set<std::string> getMutableParameters() const;
		RPI::DeviceState getDeviceState() const;

		void setEStop(bool estop);
	};

} /* namespace rocketlauncher */
#endif /* ROCKETLAUNCHERDEVICE_HPP_ */
