#pragma once

//Forward Declarations
class Sensor;
enum SensorType;
struct SensorSettings;

class IMU
{
public:
	IMU() {}
	void setSensor(std::string sensorName, winrt::Windows::Storage::Streams::DataReader sensorSettings);
	std::vector<SensorSettings> getSensorSettings(SensorType sensorType);
	std::shared_ptr<Sensor> getSensor(SensorType sensorType);

private:
	std::shared_ptr<Sensor> sensors[3]{ nullptr, nullptr, nullptr };
};