#pragma once

#include "Sensor.h"

class LSM9DS1_ACC : public Sensor
{
public:
	LSM9DS1_ACC(uint8_t* defaultSettings) {};
	double getConversionFactor() { return 0; }; //this makes the Sensor class abstract

private:
	/*SensorSettingOptions getRawSetting(SensorSettingType sensorSetting);
	bool GyroActive()*/;

};