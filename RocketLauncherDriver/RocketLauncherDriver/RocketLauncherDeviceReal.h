#pragma once
#include "RocketLauncherDevice.hpp"

namespace rocketlauncher {

	class RocketLauncherDeviceReal :
		public RocketLauncherDevice
	{
	private:
		hidapi::HidDevice hid_device;
		bool deviceOnline;
		bool setupDataSent;

		bool sendFeature(unsigned char chr1, unsigned char chr2, unsigned char chr3, unsigned char chr4, unsigned char chr5);
		bool getFeature(unsigned char *send_feature_buffer);
	
	protected:
		DeviceData readDevice(DeviceData last);
		bool writeDevice(CommandData data);

	public:
		RocketLauncherDeviceReal();
		~RocketLauncherDeviceReal();

		void setDeviceOnline(bool online);
		bool getDeviceOnline() const;
	};
} // namespace rocketlauncher

