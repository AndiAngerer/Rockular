#include "stdafx.h"
#include "RocketLauncherDeviceReal.h"
#include <iostream>

namespace rocketlauncher
{

	RocketLauncherDeviceReal::RocketLauncherDeviceReal() : hid_device(0x0416, 0x9391, 0)
	{
		deviceOnline = false;
		setupDataSent = false;
	}


	RocketLauncherDeviceReal::~RocketLauncherDeviceReal()
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

	void RocketLauncherDeviceReal::setDeviceOnline(bool online)
	{
		if (this->deviceOnline == online) return;
		this->deviceOnline = online;
		if (this->deviceOnline)
		{
			positionData.pan.movedWaySinceCalibration = -1;
			positionData.tilt.movedWaySinceCalibration = -1;
			setupDataSent = false;
		}
		//std::cout << "Rocketlauncher went " << devicestate << std::endl;
	}

	bool RocketLauncherDeviceReal::getDeviceOnline() const
	{
		return deviceOnline;
	}

	DeviceData RocketLauncherDeviceReal::readDevice(DeviceData last)
	{
		std::cout << "Returning simulated device data" << std::endl;

		unsigned char receive_feature_buffer[5];

		if (!setupDataSent)
		{ // || RTT::os::TimeService::Instance()->getSeconds(setupDataSent)>0.5) {
			if (!sendFeature(0x51, 0x01, 0x00, 0x00, 0x00))
			{
				setDeviceOnline(false);
				return last;
			}
			if (!getFeature(receive_feature_buffer))
			{
				setDeviceOnline(false);
				return last;
			}

			if (!sendFeature(0x5D, 0x00, 0x7F, 0xFF, 0xFE))
			{
				setDeviceOnline(false);
				return last;
			}
			if (!getFeature(receive_feature_buffer))
			{
				setDeviceOnline(false);
				return last;
			}
			std::cout << (int)receive_feature_buffer[1] << std::endl;

			setupDataSent = true;
		}


		while (true)
		{
			if (!sendFeature(0x5C, 0x00, 0x00, 0x00, 0x00))
			{
				setDeviceOnline(false);
				return last;
			}
			if (!getFeature(receive_feature_buffer))
			{
				setDeviceOnline(false);
				return last;
			}
			if (receive_feature_buffer[0] == 0x5C) break;
			//std::cout << ".";
		}
		//std::cout << std::endl;

		// Interpret data
		DeviceData data;
		data.rocket1Present = (receive_feature_buffer[1] & 0x01) == 0x00;
		data.rocket2Present = (receive_feature_buffer[1] & 0x02) == 0x00;
		data.rocket3Present = (receive_feature_buffer[1] & 0x40) == 0x00;
		data.right = (receive_feature_buffer[1] & 0x10) == 0x00;

		while (true)
		{
			if (!sendFeature(0x5E, 0x00, 0x00, 0x00, 0x00))
			{
				setDeviceOnline(false);
				return last;
			}
			if (!getFeature(receive_feature_buffer))
			{
				setDeviceOnline(false);
				return last;
			}
			if (receive_feature_buffer[0] == 0x5E) break;
			//std::cout << "*";
		}
		//std::cout << std::endl;

		// Interpret data
		data.left = (receive_feature_buffer[1] & 0x20) == 0x00;
		data.top = (receive_feature_buffer[1] & 0x80) == 0x00;
		data.bottom = (receive_feature_buffer[1] & 0x40) == 0x00;

		return data;
	}
	bool RocketLauncherDeviceReal::writeDevice(CommandData data)
	{
		unsigned char cmd = 0x00;
		if (data.pan == POSITIVE) cmd += 0x04;
		if (data.pan == NEGATIVE) cmd += 0x08;
		if (data.tilt == POSITIVE) cmd += 0x02;
		if (data.tilt == NEGATIVE) cmd += 0x01;
		if (data.fire) cmd += 0x10;
		if (!sendFeature(0x5F, cmd, 0xE0, 0xFF, 0xFE))
		{
			setDeviceOnline(false);
			return false;
		}

		setDeviceOnline(true);
		return true;
	}


	bool RocketLauncherDeviceReal::sendFeature(unsigned char chr1, unsigned char chr2, unsigned char chr3, unsigned char chr4, unsigned char chr5)
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

	bool RocketLauncherDeviceReal::getFeature(unsigned char *receive_feature_buffer)
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

	extern "C" __declspec(dllexport) IRocketLauncherDevice* APIENTRY rocketlauncher::GetRealRocketLauncher()
	{
		return new RocketLauncherDeviceReal();
	}

} // namespace rocketlauncher