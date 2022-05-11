#pragma once

#include "Sensor.h"

//Forward Declarations
enum class SensorSettingOptions;
enum class SensorSettingType;

class LSM9DS1_ACC : public Sensor
{
public:
	LSM9DS1_ACC(uint8_t* defaultSettings);
	double getConversionFactor(); //this makes the Sensor class abstract

private:
	SensorSettingOptions getRawSetting(SensorSettingType sensorSetting);
	bool GyroActive();

};