/*
 * HidDevice.hpp
 *
 *  Created on: 30.07.2013
 *      Author: Ludwig Naegele
 */

#ifndef HIDDEVICE_HPP_
#define HIDDEVICE_HPP_

#define DllExport __declspec(dllexport)

#include <string>
#include "../library/hidapi/hidapi.hpp"

namespace hidapi
{
	const std::string dev_hidapi = "hidapi";

	class DllExport HidDevice
	{
	private:
		hid_device* device;
		unsigned short vendor_id;
		unsigned short product_id;
		void reconnect();

	public:
		HidDevice(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number);
		virtual ~HidDevice();

		bool setFeature(const unsigned char *data, size_t length);
		bool getFeature(unsigned char *data, size_t length);
	};

} /* namespace hidapi */
#endif /* HIDDEVICE_HPP_ */
