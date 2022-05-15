#pragma once

#include "Sensor.h"

//Forward Declarations
enum class SensorSettingOptions;
enum class SensorSettingType;

class LSM9DS1_ACC : public Sensor
{
public:
	LSM9DS1_ACC(winrt::Windows::Storage::Streams::DataReader inputData);
	LSM9DS1_ACC(uint8_t* inputData);
	double getConversionFactor();
	bool optionCascade(SensorSettingType sensorSetting);

private:
	void updateSettingVectors();
	SensorSettingOptions getRawSetting(SensorSettingType sensorSetting);
	bool GyroActive();
};

class LSM9DS1_GYR : public Sensor
{
public:
	LSM9DS1_GYR(winrt::Windows::Storage::Streams::DataReader inputData);
	double getConversionFactor();
	bool optionCascade(SensorSettingType sensorSetting);

private:
	void updateSettingVectors();
	SensorSettingOptions getRawSetting(SensorSettingType sensorSetting);
};

class LSM9DS1_MAG : public Sensor
{
public:
	LSM9DS1_MAG(winrt::Windows::Storage::Streams::DataReader inputData);
	double getConversionFactor();
	bool optionCascade(SensorSettingType sensorSetting);

private:
	void updateSettingVectors();
	SensorSettingOptions getRawSetting(SensorSettingType sensorSetting);
};