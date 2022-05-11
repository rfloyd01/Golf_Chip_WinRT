#pragma once

//Forward Declarations
enum class SensorSettingOptions;
enum class SensorSettingType;
struct SensorSettings;

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
	static std::shared_ptr<Sensor> SensorFactory(std::string sensorName, uint8_t* sensorSettings);
	SensorType getSensorType() { return sensorType; }

protected:
	virtual SensorSettingOptions getRawSetting(SensorSettingType sensorSetting) = 0; //Every sensor may have a slightly different raw settings byte array so will need a different way to read it

	uint8_t* rawSettings;
	uint8_t I2C_Address = 0; //If I ever want to implement sensor switching on the fly, the I2C address will be necessary 
	SensorType sensorType;
	std::string name;
	std::vector<SensorSettings> sensorSettings;
};