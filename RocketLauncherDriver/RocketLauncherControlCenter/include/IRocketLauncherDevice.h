#pragma once

#ifndef IROCKETLAUNCHERDEVICE_HPP_
#define IROCKETLAUNCHERDEVICE_HPP_

#include <windows.h>

namespace rocketlauncher
{
	enum Direction
	{
		POSITIVE,
		NEGATIVE,
		NONE
	};

	class IRocketLauncherDevice
	{
	
	public:

		virtual void setMovement(Direction pan, Direction tilt) = 0;

		virtual void setFiring(bool enabled) = 0;

		virtual bool isLeft() const = 0;
		virtual bool isRight() const = 0;
		virtual bool isTop() const = 0;
		virtual bool isBottom() const = 0;
		virtual bool isRocket1Present() const = 0;
		virtual bool isRocket2Present() const = 0;
		virtual bool isRocket3Present() const = 0;
		virtual bool isPanCalibrated() const = 0;
		virtual bool isTiltCalibrated() const = 0;
		virtual double getEstimatedPanPosition() const = 0;
		virtual double getEstimatedTiltPosition() const = 0;

		virtual void setDeviceOnline(bool online) = 0;
		virtual bool getDeviceOnline() const = 0;
	};

	extern "C" __declspec(dllexport) IRocketLauncherDevice* APIENTRY GetSimulatedRocketLauncher();
	extern "C" __declspec(dllexport) IRocketLauncherDevice* APIENTRY GetRealRocketLauncher();

} // namespace rocketlauncher
#endif

