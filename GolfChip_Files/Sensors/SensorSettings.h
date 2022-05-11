#pragma once

//This file helps to define the different settings that are utilized by
//different IMU's. Included are helper classes to convert the enum values
//to physical numbers when necessary.

enum class SensorSettingType
{
	OUTPUT_DATA_RATE = 0x000,
	ACC_FULLSCALE_RANGE = 0x100,
	GYR_FULLSCALE_RANGE = 0x110,
	MAG_FULLSCALE_RANGE = 0x120,
	FILTER_SETTINGS = 0x200,
	HIGH_PASS_FILTER_FREQ = 0x300,
	LOW_PASS_FILTER_FREQ = 0x400,
	OPERATING_MODE = 0x500,
};

enum class SensorSettingOptions
{
	//This enum class represents all of the possible sensor settings. We use
	//"enum chaining" to combine this with the SensorSettings enum.

	//Output Data Rate Options
	//(*note) if there are more than 256 ODR settings then the (int) values
	//in the SensorSettings enum class will need to be updated
	ODR_N_A = (int)SensorSettingType::OUTPUT_DATA_RATE,
	ODR_0_625_HZ,
	ODR_1_25_HZ,
	ODR_2_5_HZ,
	ODR_5_HZ,
	ODR_10_HZ,
	ODR_14_9_HZ,
	ODR_20_HZ,
	ODR_40_HZ,
	ODR_50_HZ,
	ODR_59_5_HZ,
	ODR_80_HZ,
	ODR_119_HZ,
	ODR_238_HZ,
	ODR_476_HZ,
	ODR_952_HZ,

	//Accelerometer Fullscale Range Options
	//(*note) if there are more than 16 ODR settings then the (int) values
	//in the SensorSettings enum class will need to be updated
	ACC_FSR_N_A = (int)SensorSettingType::ACC_FULLSCALE_RANGE,
	ACC_FSR_2_G,
	ACC_FSR_4_G,
	ACC_FSR_8_G,
	ACC_FSR_16_G,

	//Gyroscope Fullscale Range Options
	//(*note) if there are more than 16 ODR settings then the (int) values
	//in the SensorSettings enum class will need to be updated
	GYR_FSR_N_A = (int)SensorSettingType::GYR_FULLSCALE_RANGE,
	GYR_FSR_245_DS,
	GYR_FSR_500_DS,
	GYR_FSR_2000_DS,

	//Magnetometer Fullscale Range Options
	//(*note) if there are more than 16 ODR settings then the (int) values
	//in the SensorSettings enum class will need to be updated
	MAG_FSR_N_A = (int)SensorSettingType::MAG_FULLSCALE_RANGE,
	MAG_FSR_4_GA,
	MAG_FSR_8_GA,
	MAG_FSR_12_GA,
	MAG_FSR_16_GA,

	//Filter General Settings
	//A somewhat complex category as different sensors can have vastly different kinds
	//of filter settings.

	//LSM9DS1 Accelerometer Filter Settings
	//(For more information about each of these settings see LSM9DS1.cpp)
	FS_LSM9DS1_ACC_AUTO_NO_FILTERS = (int)SensorSettingType::FILTER_SETTINGS,
	FS_LSM9DS1_ACC_MANUAL_NO_FILTERS,
	FS_LSM9DS1_ACC_AUTO_LPF2,
	FS_LSM9DS1_ACC_MANUAL_LPF2,
	FS_LSM9DS1_ACC_AUTO_HPF,
	FS_LSM9DS1_ACC_MANUAL_HPF,

	//High Pass Filter Frequency Setting
	//(*note) if there are more than 256 HPF settings then the (int) values
	//in the SensorSettings enum class will need to be updated

	//The below 4 frequencies are dependent on current ODR settings
	HPF_N_A = (int)SensorSettingType::HIGH_PASS_FILTER_FREQ,
	HPF_ODR_OVER_50_HZ,
	HPF_ODR_OVER_100_HZ,
	HPF_ODR_OVER_9_HZ,
	HPF_ODR_OVER_400_HZ,

	//Low Pass Filter Frequency Setting
	//(*note) if there are more than 256 LPF settings then the (int) values
	//in the SensorSettings enum class will need to be updated

	//The below 4 frequencies are dependent on current ODR settings
	LPF_N_A = (int)SensorSettingType::LOW_PASS_FILTER_FREQ,
	LPF_408_HZ,
	LPF_211_HZ,
	LPF_105_HZ,
	LPF_50_HZ,

	//Operating Mode Options
	//A somewhat complex category as different sensors can have vastly different kinds
	//of operating modes.

	//LSM9DS1 Accelerometer Operating Modes
	OM_ACC_ONLY = (int)SensorSettingType::OPERATING_MODE,
	OM_ACC_AND_GYR,

	//Very last option (used to terminate list, as well as act as a null value)
	LAST
};

struct SensorSettings {
	SensorSettingType sensorSettingType;

	SensorSettingOptions currentSettingOption;
	std::vector<SensorSettingOptions> possibleSettingOptions;

	/*double getODR(ODR odr);
	int getFSR(FullscaleRange fsr);*/
};