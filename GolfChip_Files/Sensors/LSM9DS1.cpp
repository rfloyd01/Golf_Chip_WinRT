#include "pch.h"
#include "LSM9DS1.h"
#include "SensorSettings.h"

LSM9DS1_ACC::LSM9DS1_ACC(uint8_t* defaultSettings)
{
	//This is where all of the default settings for the sensor go
	sensorType = SensorType::ACCELEROMETER;

	rawSettings = defaultSettings; //these are the current settings read straight from the BLE Characteristic
	name = "LSM9DS1";

	//TODO: Currently I'm manually setting all of the possibleSettingOptions vectors. I should utilize the chained
	//enums to do this manually

	//setting creation
	//Operating Mode (*note, this must be set up before the Output Data Rate is the odr depends on the current operating mode)
	SensorSettings OperatingMode;
	OperatingMode.sensorSettingType = SensorSettingType::OPERATING_MODE;
	OperatingMode.possibleSettingOptions = { SensorSettingOptions::OM_ACC_ONLY, SensorSettingOptions::OM_ACC_AND_GYR };
	OperatingMode.currentSettingOption = getRawSetting(SensorSettingType::OPERATING_MODE);
	sensorSettings.push_back(OperatingMode);

	//Output Date Rate
	SensorSettings OutputDataRate;
	OutputDataRate.sensorSettingType = SensorSettingType::OUTPUT_DATA_RATE;
	if (GyroActive()) OutputDataRate.possibleSettingOptions = { SensorSettingOptions::ODR_14_9_HZ, SensorSettingOptions::ODR_59_5_HZ, SensorSettingOptions::ODR_119_HZ, SensorSettingOptions::ODR_238_HZ, SensorSettingOptions::ODR_476_HZ, SensorSettingOptions::ODR_952_HZ };
	else OutputDataRate.possibleSettingOptions = { SensorSettingOptions::ODR_10_HZ, SensorSettingOptions::ODR_50_HZ, SensorSettingOptions::ODR_119_HZ, SensorSettingOptions::ODR_238_HZ, SensorSettingOptions::ODR_476_HZ, SensorSettingOptions::ODR_952_HZ };
	OutputDataRate.currentSettingOption = getRawSetting(SensorSettingType::OUTPUT_DATA_RATE); //read the current setting from the rawSetting array
	sensorSettings.push_back(OutputDataRate);

	//Full-scale Range
	SensorSettings FullscaleRange;
	FullscaleRange.sensorSettingType = SensorSettingType::ACC_FULLSCALE_RANGE;
	FullscaleRange.possibleSettingOptions = { SensorSettingOptions::ACC_FSR_2_G, SensorSettingOptions::ACC_FSR_4_G, SensorSettingOptions::ACC_FSR_8_G, SensorSettingOptions::ACC_FSR_16_G };
	FullscaleRange.currentSettingOption = getRawSetting(SensorSettingType::ACC_FULLSCALE_RANGE);
	sensorSettings.push_back(FullscaleRange);

	//Filter General Settings
	SensorSettings FilterSettings;
	FilterSettings.sensorSettingType = SensorSettingType::FILTER_SETTINGS;
	FilterSettings.possibleSettingOptions = { SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS, SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2, SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF, };
	FilterSettings.currentSettingOption = getRawSetting(SensorSettingType::FILTER_SETTINGS);
	sensorSettings.push_back(FilterSettings);

	//High-pass Filter Frequency Setting (the options depend on wheter or not the HPF is actually turned on)
	SensorSettings HPFFreq;
	HPFFreq.sensorSettingType = SensorSettingType::HIGH_PASS_FILTER_FREQ;
	if (*(rawSettings + 2) & 0x1)
	{
		HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_ODR_OVER_50_HZ, SensorSettingOptions::HPF_ODR_OVER_100_HZ, SensorSettingOptions::HPF_ODR_OVER_9_HZ, SensorSettingOptions::HPF_ODR_OVER_400_HZ };
		HPFFreq.currentSettingOption = getRawSetting(SensorSettingType::HIGH_PASS_FILTER_FREQ);
	}
	else
	{
		HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_N_A };
		HPFFreq.currentSettingOption = SensorSettingOptions::HPF_N_A;
	}
	sensorSettings.push_back(HPFFreq);

	//Low-pass Filter Frequency Setting (the options depend on wheter or not the LPF is in manual or auto mode)
	SensorSettings LPFFreq;
	LPFFreq.sensorSettingType = SensorSettingType::LOW_PASS_FILTER_FREQ;
	if (*(rawSettings + 2) & 0x100)
	{
		LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_408_HZ, SensorSettingOptions::LPF_211_HZ, SensorSettingOptions::LPF_105_HZ, SensorSettingOptions::LPF_50_HZ };
		LPFFreq.currentSettingOption = getRawSetting(SensorSettingType::LOW_PASS_FILTER_FREQ);
	}
	else
	{
		//when in automatic mode, there's only one frequency option and it depends on the current ODR.
		switch (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE))
		{
		case SensorSettingOptions::ODR_476_HZ: LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_211_HZ };
		case SensorSettingOptions::ODR_238_HZ: LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_105_HZ };
		case SensorSettingOptions::ODR_119_HZ: LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_50_HZ };
		default: LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_408_HZ };
		}
	}
	sensorSettings.push_back(LPFFreq);
}

double LSM9DS1_ACC::getConversionFactor()
{
	//Returns the Acceleration sensitivity in g/LSB, used by the IMU class
	if (getCurrentSettingOption(SensorSettingType::ACC_FULLSCALE_RANGE) == SensorSettingOptions::ACC_FSR_2_G) return .000061;
	else if (getCurrentSettingOption(SensorSettingType::ACC_FULLSCALE_RANGE) == SensorSettingOptions::ACC_FSR_4_G) return .000122;
	else if (getCurrentSettingOption(SensorSettingType::ACC_FULLSCALE_RANGE) == SensorSettingOptions::ACC_FSR_8_G) return .000244;
	else if (getCurrentSettingOption(SensorSettingType::ACC_FULLSCALE_RANGE) == SensorSettingOptions::ACC_FSR_16_G) return .000732;
	else return -1; //shouldn't be possible to have another option, so return this to indicate an error
}

SensorSettingOptions LSM9DS1_ACC::getRawSetting(SensorSettingType sensorSetting)
{
	//The LSM9DS1 Accelerometer raw setting byte array has the following form:
	//byte 0: ODR setting
	//byte 1: Full-scale range setting
	//byte 2: Filter General Settings
	//byte 3: High Pass Filter Settings
	//byte 4: Low Pass Filter Settings
	//byte 5: Operating Mode

	if (sensorSetting == SensorSettingType::OUTPUT_DATA_RATE)
	{
		//The ODR options will depend on whether or not the Gyroscope of the LSM9DS1 is also active
		if (GyroActive())
		{
			switch (*rawSettings)
			{
			case 0x001: return SensorSettingOptions::ODR_10_HZ;
			case 0x010: return SensorSettingOptions::ODR_50_HZ;
			case 0x011: return SensorSettingOptions::ODR_119_HZ;
			case 0x100: return SensorSettingOptions::ODR_238_HZ;
			case 0x101: return SensorSettingOptions::ODR_476_HZ;
			case 0x110: return SensorSettingOptions::ODR_952_HZ;
			default: return SensorSettingOptions::ODR_N_A;
			}
		}
		else
		{
			switch (*rawSettings)
			{
			case 0x001: return SensorSettingOptions::ODR_14_9_HZ;
			case 0x010: return SensorSettingOptions::ODR_59_5_HZ;
			case 0x011: return SensorSettingOptions::ODR_119_HZ;
			case 0x100: return SensorSettingOptions::ODR_238_HZ;
			case 0x101: return SensorSettingOptions::ODR_476_HZ;
			case 0x110: return SensorSettingOptions::ODR_952_HZ;
			default: return SensorSettingOptions::ODR_N_A;
			}
		}

	}
	else if (sensorSetting == SensorSettingType::ACC_FULLSCALE_RANGE)
	{
		//The LSM9DS1 accelerometer has 4 different options for the full-scale range.
		//Oddly enough they don't go in order, the +/- 16G setting comes inbetween 2 and 4
		//four some reason. This isn't a typo, it's how it's written in the manufacturer's
		//data sheet

		switch (*(rawSettings + 1))
		{
		case 0x00: return SensorSettingOptions::ACC_FSR_2_G;
		case 0x01: return SensorSettingOptions::ACC_FSR_16_G;
		case 0x10: return SensorSettingOptions::ACC_FSR_4_G;
		case 0x11: return SensorSettingOptions::ACC_FSR_8_G;
		}
	}
	else if (sensorSetting == SensorSettingType::FILTER_SETTINGS)
	{
		//The LSM9DS1 accelerometer has 3 filters built into it:
		//1. A (mandatory) low-pass anti-aliasing filter. This filter can be run in one of two modes,
		//   an automatic mode where the filter cutoff value is dictated by the current ODR, and a manual
		//   mode where the user can decide what the cut-off frequency should be.
		//2. A second (optional) low pass filter referred to as the High Resolution Filter.
		//3. An (optional) high pass filter.
		//
		//The user can decide if they want to use the second low pass filter, the high pass filter,
		//or neither of the optional filters. Only one of the optional filters can be utilized at a time,
		//not both. If the user opts to use one of the optional filters, the frequency for the filter can
		//be set using the Highpass or Lowpass filter settings type

		switch (*(rawSettings + 2))
		{
		case 0x000: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS;
		case 0x100: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS;
		case 0x010: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2;
		case 0x110: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2;
		case 0x001: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF;
		case 0x101: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF;
		}
	}
	else if (sensorSetting == SensorSettingType::HIGH_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the optional high pass
		//filter for the LSM9DS1 accelerometer. This filter needs to be turned on
		//to actual select one of the below options. Furthermore, we don't explicitly
		//get to choose the frequency, it is ultimately defined as a function of the
		//current ODR.
		switch (*(rawSettings + 3))
		{
		case 0x00: return SensorSettingOptions::HPF_ODR_OVER_50_HZ;
		case 0x01: return SensorSettingOptions::HPF_ODR_OVER_100_HZ;
		case 0x10: return SensorSettingOptions::HPF_ODR_OVER_9_HZ;
		case 0x11: return SensorSettingOptions::HPF_ODR_OVER_400_HZ;
		}
	}
	else if (sensorSetting == SensorSettingType::LOW_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the mandatory low pass anti-aliasing
		//filter for the LSM9DS1 accelerometer. This filter needs to be set into
		//manual mode to actual select one of the below options
		switch (*(rawSettings + 4))
		{
		case 0x00: return SensorSettingOptions::LPF_408_HZ;
		case 0x01: return SensorSettingOptions::LPF_211_HZ;
		case 0x10: return SensorSettingOptions::LPF_105_HZ;
		case 0x11: return SensorSettingOptions::LPF_50_HZ;
		}
	}
	else if (sensorSetting == SensorSettingType::OPERATING_MODE)
	{
		//The LSM9DS1 accelerometer has two different operating modes (for now),
		//with Gyroscope, or without Gyroscope
		switch (*(rawSettings + 5))
		{
		case 0x0: return SensorSettingOptions::OM_ACC_ONLY;
		case 0x1: return SensorSettingOptions::OM_ACC_AND_GYR;
		}
	}
}

bool LSM9DS1_ACC::GyroActive()
{
	if (getRawSetting(SensorSettingType::OPERATING_MODE) == SensorSettingOptions::OM_ACC_AND_GYR) return true;
	return false;
}