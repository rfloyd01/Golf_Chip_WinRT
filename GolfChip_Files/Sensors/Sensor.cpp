#include "pch.h"
#include "Sensor.h"

#include "SensorSettings.h"
#include "LSM9DS1.h"

std::shared_ptr<Sensor> Sensor::SensorFactory(std::string sensorName, uint8_t* sensorSettings)
{
	if (sensorName == "LSM9DS1_ACC")
	{
		return std::make_shared<LSM9DS1_ACC>(sensorSettings);
	}
}

SensorSettingOptions Sensor::getCurrentSettingOption(SensorSettingType sensorSetting)
{
	//Just cycle through the entire vector of Settings to find the appropriate one,
	//if it doesn't exist then just return null;
	for (auto setting : sensorSettings)
	{
		if (setting.sensorSettingType == sensorSetting)
		{
			return setting.currentSettingOption;
		}
	}

	//The equivalent of a null value
	return SensorSettingOptions::LAST;
}