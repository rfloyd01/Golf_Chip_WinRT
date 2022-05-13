#include "pch.h"
#include "IMU.h"

#include "Sensors/Sensor.h"

void IMU::setSensor(std::string sensorName, winrt::Windows::Storage::Streams::DataReader sensorSettings)
{
	//Envoke the static SensorFactory() method to build the sensor.
	//The factory works by looking at the name of the sensor that's
	//stored in the BLE Sensor Information Characteristic
	std::shared_ptr<Sensor> sensor = Sensor::SensorFactory(sensorName, sensorSettings);

	//Delete any existing sensors of the same type before saving the
	//new sensor
	SensorType newSensorType = sensor->getSensorType();

	if (sensors[newSensorType] != nullptr) sensors[newSensorType].reset();
	sensors[newSensorType] = sensor;
}

std::vector<SensorSettings> IMU::getSensorSettings(SensorType sensorType)
{
	return sensors[sensorType]->getSensorSettings();
}

std::shared_ptr<Sensor> IMU::getSensor(SensorType sensorType)
{ 
	return sensors[sensorType];
}