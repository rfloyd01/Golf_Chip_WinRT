#include "pch.h"
#include "GolfChip.h"

//Create the global instance of the GolfChip class
std::shared_ptr<GolfChip> winrt::Golf_Chip_WinRT::GlobalGolfChip::m_golfChip{ std::make_shared<GolfChip>() };

GolfChip::GolfChip()
{
	//The golf chip is defined by two distinct devices. First it has an Inertial Measurement Unit (IMU)
	//which is comprised of sensors like accelerometers, gyroscopes and magnetometers that gather data 
	//about movement and position. Secondly, it has a Bluetooth Low Energy (BLE) module which is used
	//to send data from the chip to this app.

	m_IMU = std::make_shared<IMU>();
}

void GolfChip::Disconnect()
{
	//set appropriate variables to null pointers to terminate the connection
	m_bleDevice.reset();
	informationCharacteristic = nullptr;

	m_IMU.reset();
}