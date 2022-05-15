#pragma once

#include "SensorSettings.h"

enum SensorType
{
	ACCELEROMETER = 0,
	GYROSCOPE = 1,
	MAGNETOMETER = 2
};

//An abstract class that all sensors derive from
class Sensor
{
public:
	virtual double getConversionFactor() = 0;
	SensorSettingOptions getCurrentSettingOption(SensorSettingType sensorSetting);
	static std::shared_ptr<Sensor> SensorFactory(std::string sensorName, winrt::Windows::Storage::Streams::DataReader sensorSettings);
	static std::shared_ptr<Sensor> SensorFactory(std::string sensorName, uint8_t* sensorSettings);
	SensorType getSensorType() { return sensorType; }
	uint8_t* getRawSettings() { return raw_settings; }
	virtual int getRawSettingLocation(SensorSettingType sensorSetting) = 0;
	std::vector<SensorSettings> getSensorSettings();
	std::string getName() { return name; }
	virtual bool optionCascade(SensorSettingType sensorSetting) = 0;
	virtual uint8_t getByte(SensorSettingOptions sensorOption) = 0;

protected:
	virtual SensorSettingOptions getRawSetting(SensorSettingType sensorSetting) = 0; //Every sensor may have a slightly different raw settings byte array so will need a different way to read it

	uint8_t raw_settings[18]; //Most sensors don't have 18 different settings to configure, but it's better to have too much than too little storage
	uint8_t I2C_Address = 0; //If I ever want to implement sensor switching on the fly, the I2C address will be necessary 
	SensorType sensorType;
	std::string name;
	std::vector<SensorSettings> sensorSettings;
};