#pragma once
#include "pch.h"

#include "IMU.h"
#include "BLEDevice.h"

//This file does two things. First it defines the GolfChip class, which represents the physical 
//chip that we connect to (both the BLE portion as well as the IMU portion, and associated
//calculations). Second, it creates a global instance of the GolfChip class that can be accessed
//from any page of the application.

//Forward Declaration of Classes
class BLEDevice;
class IMU;

class GolfChip
{
public:
	GolfChip();
	
	std::shared_ptr<BLEDevice> getBLEDevicePointer() { return m_bleDevice; }
	std::shared_ptr<IMU> getSensor() { return m_IMU; }

	winrt::Windows::Devices::Bluetooth::BluetoothLEDevice getBLEDevice(){ return m_bleDevice->getBLEDevice(); }
	void setBLEDevice(winrt::Windows::Devices::Bluetooth::BluetoothLEDevice bleDevice) { m_bleDevice->setBLEDevice(bleDevice); }

private:
	std::shared_ptr<BLEDevice> m_bleDevice;
	std::shared_ptr<IMU> m_IMU;
};

namespace winrt::Golf_Chip_WinRT
{
	struct Constants
	{
		//Constants used throught the application are defined here
		//Numeric Constants for calculations
		static constexpr double gravity{ 9.80665 }; //acceleration due to gravity

		//Constants for connecting to the Golf Chip
		static const int number_of_samples{ 10 }; //number of sensor samples stored in the BLE characteristic at a given time. Due to the time associated with reading BLE broadcasts, its more efficient to store multiple data points at a single time then try to read each individual point
		static constexpr guid GolfChipSensorDataServiceUuid{ 0x0000180c, 0x0000, 0x1000, { 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb } }; // {0000180c-0000-1000-8000-00805f9b34fb}
		static const uint32_t GolfChipSensorDataCharacteristicUuid{ 0x00002A58 };
		static const uint32_t GolfChipSensorTestCharacteristicUuid{ 0x00002A59 };
	};

	struct GlobalGolfChip
	{
		//This struct holds the one global instance of the golfchip object. It's created as a shared pointer
		//so that it can be accessed by multiple pages simultaneously. This global instance of the golf chip
		//is created upon startup of the app.
		static std::shared_ptr<GolfChip> m_golfChip;
	};
}