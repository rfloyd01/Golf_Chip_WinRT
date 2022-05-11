#pragma once

//An abstract class that all sensors derive from

enum SensorType
{
	ACCELEROMETER = 0,
	GYROSCOPE = 1,
	MAGNETOMETER = 2
};

class Sensor
{
public:
	virtual double getConversionFactor() = 0;
	static std::shared_ptr<Sensor> SensorFactory(std::string sensorName, uint8_t* sensorSettings);
};