#pragma once

#include "IMU.h"

//Forward Declarations of classes
class IMU;

class BLEDevice
{
public:
	BLEDevice(std::shared_ptr<IMU> imu) { m_IMU = imu; }

private:
	std::shared_ptr<IMU> m_IMU;
};