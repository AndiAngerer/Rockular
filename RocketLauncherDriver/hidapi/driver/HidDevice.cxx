/*
 * HidDevice.cxx
 *
 *  Created on: 30.07.2013
 *      Author: Ludwig Naegele
 */

#include "HidDevice.hpp"
#include <iostream>

namespace hidapi
{

	HidDevice::HidDevice(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number)
	{
		hid_init();

		this->vendor_id = vendor_id;
		this->product_id = product_id;
		reconnect();
	}

	void HidDevice::reconnect()
	{
		/*auto hidenum = hid_enumerate(vendor_id,product_id);
		auto firstenum = hidenum;

		while(hidenum != NULL)
		{
			if(hidenum->interface_number == 1)
			{
				device = hid_open_path(hidenum->path);
				break;
			}

			hidenum = hidenum->next;
		}


		hid_free_enumeration(firstenum);*/

		//getchar();

		//std::cout << "Reconnecting..." << std::endl;
		device = hid_open(vendor_id, product_id, 0);
		//std::cout << device << std::endl;
	}

	bool HidDevice::setFeature(const unsigned char *data, size_t length) {
		if(device == 0) reconnect();
		if(device != 0) {
			int result = hid_send_feature_report(device, data, length) == length;
			if (result < 0)
			{
				hid_close(device);
				device = 0;
				return false;
			}
			return true;
			
		}
		return false;
	}

	bool HidDevice::getFeature(unsigned char *data, size_t length) {
		if(device == 0) reconnect();
		if(device != 0) {
			int result = hid_get_feature_report(device, data, length) == length;
			if (result < 0) {
				hid_close(device);
				device = 0;
				return false;
			}
			return true;
		}
		return false;
	}

	HidDevice::~HidDevice()
	{
		if(device != 0)
			hid_close(device);
		device = 0;
	}

} /* namespace hidapi */
