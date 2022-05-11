#pragma once

//Forward Declarations
class Sensor;
enum SensorType;
struct SensorSettings;

class IMU
{
public:
	IMU() {}
	void setSensor(std::string sensorName, uint8_t* sensorSettings);
	std::vector<SensorSettings> getSensorSettings(SensorType sensorType);

private:
	std::shared_ptr<Sensor> sensors[3]{ nullptr, nullptr, nullptr };
};