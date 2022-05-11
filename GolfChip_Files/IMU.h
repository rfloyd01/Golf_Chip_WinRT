#pragma once

#include "Sensors/Sensor.h"

//Forward Class Declerations
class Sensor;

class IMU
{
public:
	IMU() {}

private:
	std::shared_ptr<Sensor> sensors[3]{ nullptr, nullptr, nullptr };
};