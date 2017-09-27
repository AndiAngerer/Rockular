#pragma once
#include "RocketLauncherDevice.hpp"

namespace rocketlauncher {

	class RocketLauncherDeviceSim :
		public RocketLauncherDevice
	{
	private:
		int firedCount;
		Direction lastPan;
		Direction lastTilt;
	protected:
		DeviceData readDevice(DeviceData last);
		bool writeDevice(CommandData data);
	public:
		RocketLauncherDeviceSim();
		~RocketLauncherDeviceSim();

		void setDeviceOnline(bool online);
		bool getDeviceOnline() const;
	};
} // namespace rocketlauncher

