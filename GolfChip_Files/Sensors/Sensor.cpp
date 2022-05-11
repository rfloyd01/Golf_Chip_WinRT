#include "pch.h"
#include "Sensor.h"

#include "LSM9DS1.h"

std::shared_ptr<Sensor> Sensor::SensorFactory(std::string sensorName, uint8_t* sensorSettings)
{
	if (sensorName == "LSM9DS1_ACC")
	{
		return std::make_shared<LSM9DS1_ACC>(sensorSettings);
	}
}