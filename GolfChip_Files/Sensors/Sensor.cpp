#include "pch.h"
#include "Sensor.h"

#include "LSM9DS1.h"

std::shared_ptr<Sensor> Sensor::SensorFactory(std::string sensorName, winrt::Windows::Storage::Streams::DataReader sensorSettings)
{
	if (sensorName == "LSM9DS1_ACC") return std::make_shared<LSM9DS1_ACC>(sensorSettings);
	else if (sensorName == "LSM9DS1_GYR") return std::make_shared<LSM9DS1_GYR>(sensorSettings);
	else if (sensorName == "LSM9DS1_MAG") return std::make_shared<LSM9DS1_MAG>(sensorSettings);
}

std::shared_ptr<Sensor> Sensor::SensorFactory(std::string sensorName, uint8_t* sensorSettings)
{
	//Same as the above method, however, takes in a pointer to a standard array
	if (sensorName == "LSM9DS1_ACC") return std::make_shared<LSM9DS1_ACC>(sensorSettings);
	else if (sensorName == "LSM9DS1_GYR") return std::make_shared<LSM9DS1_GYR>(sensorSettings);
	else if (sensorName == "LSM9DS1_MAG") return std::make_shared<LSM9DS1_MAG>(sensorSettings);
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

std::vector<SensorSettings> Sensor::getSensorSettings()
{
	return sensorSettings;
}