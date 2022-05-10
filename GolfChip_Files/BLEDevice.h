#pragma once

#include "IMU.h"

//Forward Declarations of classes
class IMU;

class BLEDevice
{
public:
	BLEDevice(std::shared_ptr<IMU> imu) 
	{ 
		m_IMU = imu;
	}
	
	winrt::Windows::Devices::Bluetooth::BluetoothLEDevice getBLEDevice() { return m_BLEDevice; }
	void setBLEDevice(winrt::Windows::Devices::Bluetooth::BluetoothLEDevice bleDevice) { m_BLEDevice = bleDevice; }

private:
	std::shared_ptr<IMU> m_IMU;
	winrt::Windows::Devices::Bluetooth::BluetoothLEDevice m_BLEDevice{ 0 };
	//winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService m_dataService;
};