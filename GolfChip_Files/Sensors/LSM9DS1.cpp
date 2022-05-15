#include "pch.h"
#include "LSM9DS1.h"
#include "SensorSettings.h"

//Accelerometer Functions
LSM9DS1_ACC::LSM9DS1_ACC(winrt::Windows::Storage::Streams::DataReader inputData)
{
	//This is where all of the default settings for the sensor go
	sensorType = SensorType::ACCELEROMETER;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = inputData.ReadByte();
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

LSM9DS1_ACC::LSM9DS1_ACC(uint8_t* inputData)
{
	//same as the above constructor but takes a normal array of numbers
	sensorType = SensorType::ACCELEROMETER;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = *(inputData + i);
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

void LSM9DS1_ACC::updateSettingVectors()
{
	//First, clear out the existing vector if it isn't empty
	if (sensorSettings.size() > 0) sensorSettings.clear();

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
	FilterSettings.possibleSettingOptions = { SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS, SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2, SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF, SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF };
	FilterSettings.currentSettingOption = getRawSetting(SensorSettingType::FILTER_SETTINGS);
	sensorSettings.push_back(FilterSettings);

	//High-pass Filter Frequency Setting (the options depend on wheter or not the HPF is actually turned on)
	SensorSettings HPFFreq;
	HPFFreq.sensorSettingType = SensorSettingType::HIGH_PASS_FILTER_FREQ;
	if (raw_settings[2] & 0b1)
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
	if (raw_settings[2] & 0b100)
	{
		LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_408_HZ, SensorSettingOptions::LPF_211_HZ, SensorSettingOptions::LPF_105_HZ, SensorSettingOptions::LPF_50_HZ };
	}
	else
	{
		//when in automatic mode, there's only one frequency option and it depends on the current ODR.
		switch (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE))
		{
		case SensorSettingOptions::ODR_476_HZ:
		{
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_211_HZ };
			break;
		}
		case SensorSettingOptions::ODR_238_HZ:
		{
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_105_HZ };
			break;
		}
		case SensorSettingOptions::ODR_119_HZ:
		{
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_50_HZ };
			break;
		}
		default:
		{
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_408_HZ };
			break;
		}
		}
	}
	LPFFreq.currentSettingOption = getRawSetting(SensorSettingType::LOW_PASS_FILTER_FREQ);
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
			switch (raw_settings[0])
			{
			case 0b001: return SensorSettingOptions::ODR_10_HZ;
			case 0b010: return SensorSettingOptions::ODR_50_HZ;
			case 0b011: return SensorSettingOptions::ODR_119_HZ;
			case 0b100: return SensorSettingOptions::ODR_238_HZ;
			case 0b101: return SensorSettingOptions::ODR_476_HZ;
			case 0b110: return SensorSettingOptions::ODR_952_HZ;
			default: return SensorSettingOptions::ODR_N_A;
			}
		}
		else
		{
			switch (raw_settings[0])
			{
			case 0b001: return SensorSettingOptions::ODR_14_9_HZ;
			case 0b010: return SensorSettingOptions::ODR_59_5_HZ;
			case 0b011: return SensorSettingOptions::ODR_119_HZ;
			case 0b100: return SensorSettingOptions::ODR_238_HZ;
			case 0b101: return SensorSettingOptions::ODR_476_HZ;
			case 0b110: return SensorSettingOptions::ODR_952_HZ;
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

		switch (raw_settings[1])
		{
		case 0b00: return SensorSettingOptions::ACC_FSR_2_G;
		case 0b01: return SensorSettingOptions::ACC_FSR_16_G;
		case 0b10: return SensorSettingOptions::ACC_FSR_4_G;
		case 0b11: return SensorSettingOptions::ACC_FSR_8_G;
		default: return SensorSettingOptions::ACC_FSR_N_A;
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

		switch (raw_settings[2])
		{
		case 0b000: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS;
		case 0b100: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS;
		case 0b010: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2;
		case 0b110: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2;
		case 0b001: return SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF;
		case 0b101: return SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF;
		}
	}
	else if (sensorSetting == SensorSettingType::HIGH_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the optional high pass
		//filter for the LSM9DS1 accelerometer. This filter needs to be turned on
		//to actual select one of the below options. Furthermore, we don't explicitly
		//get to choose the frequency, it is ultimately defined as a function of the
		//current ODR.
		switch (raw_settings[3])
		{
		case 0b00: return SensorSettingOptions::HPF_ODR_OVER_50_HZ;
		case 0b01: return SensorSettingOptions::HPF_ODR_OVER_100_HZ;
		case 0b10: return SensorSettingOptions::HPF_ODR_OVER_9_HZ;
		case 0b11: return SensorSettingOptions::HPF_ODR_OVER_400_HZ;
		}
	}
	else if (sensorSetting == SensorSettingType::LOW_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the mandatory low pass anti-aliasing
		//filter for the LSM9DS1 accelerometer. This filter needs to be set into
		//manual mode to actual select one of the below options
		switch (raw_settings[4])
		{
		case 0b00: return SensorSettingOptions::LPF_408_HZ;
		case 0b01: return SensorSettingOptions::LPF_211_HZ;
		case 0b10: return SensorSettingOptions::LPF_105_HZ;
		case 0b11: return SensorSettingOptions::LPF_50_HZ;
		}
	}
	else if (sensorSetting == SensorSettingType::OPERATING_MODE)
	{
		//The LSM9DS1 accelerometer has two different operating modes (for now),
		//with Gyroscope, or without Gyroscope
		switch (raw_settings[5])
		{
		case 0b0: return SensorSettingOptions::OM_ACC_ONLY;
		case 0b1: return SensorSettingOptions::OM_ACC_AND_GYR;
		}
	}
}

bool LSM9DS1_ACC::optionCascade(SensorSettingType sensorSetting)
{
	//this function looks at the incoming sensorSetting (which is being changed from elsewhere)
	//and returns true if changing this setting will cause the options for other settings to change.
	//For example, turning off a high pass filter in Filter Settings will cause the HPF_Freq options
	//to change.
	switch(sensorSetting)
	{
	case SensorSettingType::ACC_FULLSCALE_RANGE: return false; //TODO: In the future, change this to true if low power mode gets implemented
	case SensorSettingType::FILTER_SETTINGS: return true;
	case SensorSettingType::OPERATING_MODE: return true; //This won't actually effect the ACC itself, but can effect the Gyroscope
	case SensorSettingType::OUTPUT_DATA_RATE: return true; //This won't actually effect the ACC itself, but can effect the Gyroscope ODR
	default: return false;
	}
}

int LSM9DS1_ACC::getRawSettingLocation(SensorSettingType sensorSetting)
{
	//returns the location in the raw settings array of the given setting
	switch (sensorSetting)
	{
	case SensorSettingType::OUTPUT_DATA_RATE: return 0;
	case SensorSettingType::ACC_FULLSCALE_RANGE: return 1;
	case SensorSettingType::FILTER_SETTINGS: return 2;
	case SensorSettingType::HIGH_PASS_FILTER_FREQ: return 3;
	case SensorSettingType::LOW_PASS_FILTER_FREQ: return 4;
	case SensorSettingType::OPERATING_MODE: return 5;
	default: return -1; //incorrect setting type
	}
}

uint8_t LSM9DS1_ACC::getByte(SensorSettingOptions sensorOption)
{
	//this function is the opposite of the getRawSetting function. It takes in the
	//desired setting option and returns the raw option in byte form
	switch (sensorOption)
	{
		//ODR Options
	case SensorSettingOptions::ODR_N_A: return 0b000;
	case SensorSettingOptions::ODR_10_HZ: //fall through to below option
	case SensorSettingOptions::ODR_14_9_HZ: return 0b001;
	case SensorSettingOptions::ODR_50_HZ: //fall through to below option
	case SensorSettingOptions::ODR_59_5_HZ: return 0b010;
	case SensorSettingOptions::ODR_119_HZ: return 0b011;
	case SensorSettingOptions::ODR_238_HZ: return 0b100;
	case SensorSettingOptions::ODR_476_HZ: return 0b101;
	case SensorSettingOptions::ODR_952_HZ: return 0b110;

		//Fullscale Range Options
	case SensorSettingOptions::ACC_FSR_2_G: return 0b00;
	case SensorSettingOptions::ACC_FSR_16_G: return 0b001;
	case SensorSettingOptions::ACC_FSR_4_G: return 0b10;
	case SensorSettingOptions::ACC_FSR_8_G: return 0b11;
	
		//Filter Settings
	case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS: return 0b000;
	case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS: return 0b100;
	case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2: return 0b010;
	case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2: return 0b110;
	case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF: return 0b001;
	case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF: return 0b101;

		//Highpass Filter Frequency Options
	case SensorSettingOptions::HPF_ODR_OVER_50_HZ: return 0b00;
	case SensorSettingOptions::HPF_ODR_OVER_100_HZ: return 0b01;
	case SensorSettingOptions::HPF_ODR_OVER_9_HZ: return 0b10;
	case SensorSettingOptions::HPF_ODR_OVER_400_HZ: return 0b11;

		//Lowpass Filter Frequency Options
	case SensorSettingOptions::LPF_408_HZ: return 0b00;
	case SensorSettingOptions::LPF_211_HZ: return 0b01;
	case SensorSettingOptions::LPF_105_HZ: return 0b10;
	case SensorSettingOptions::LPF_50_HZ: return 0b11;

		//Operating Mode Options
	case SensorSettingOptions::OM_ACC_ONLY: return 0b0;
	case SensorSettingOptions::OM_ACC_AND_GYR: return 0b1;
	}
	return 0xF; //this will indicate an error, none of the above return codes match 0b1111
}

bool LSM9DS1_ACC::GyroActive()
{
	if (getRawSetting(SensorSettingType::OPERATING_MODE) == SensorSettingOptions::OM_ACC_AND_GYR) return true;
	return false;
}

//Gyroscope Functions
LSM9DS1_GYR::LSM9DS1_GYR(winrt::Windows::Storage::Streams::DataReader inputData)
{
	//This is where all of the default settings for the sensor go
	sensorType = SensorType::GYROSCOPE;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = inputData.ReadByte();
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

LSM9DS1_GYR::LSM9DS1_GYR(uint8_t* inputData)
{
	//Same as the above constructor, but takes a standard array
	sensorType = SensorType::GYROSCOPE;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = *(inputData + i);
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

void LSM9DS1_GYR::updateSettingVectors()
{
	//First, clear out the existing vector if it isn't empty
	if (sensorSettings.size() > 0) sensorSettings.clear();

	//TODO: Currently I'm manually setting all of the possibleSettingOptions vectors. I should utilize the chained
	//enums to do this manually

	//setting creation
	//Operating Mode (*note, this must be set up before the Output Data Rate is the odr depends on the current operating mode)
	SensorSettings OperatingMode;
	OperatingMode.sensorSettingType = SensorSettingType::OPERATING_MODE;
	OperatingMode.possibleSettingOptions = { SensorSettingOptions::OM_GYR_REGULAR, SensorSettingOptions::OM_GYR_LOW };
	OperatingMode.currentSettingOption = getRawSetting(SensorSettingType::OPERATING_MODE);
	sensorSettings.push_back(OperatingMode);

	//Output Date Rate
	SensorSettings OutputDataRate;
	OutputDataRate.sensorSettingType = SensorSettingType::OUTPUT_DATA_RATE;
	OutputDataRate.possibleSettingOptions = { SensorSettingOptions::ODR_14_9_HZ, SensorSettingOptions::ODR_59_5_HZ, SensorSettingOptions::ODR_119_HZ, SensorSettingOptions::ODR_238_HZ, SensorSettingOptions::ODR_476_HZ, SensorSettingOptions::ODR_952_HZ };
	OutputDataRate.currentSettingOption = getRawSetting(SensorSettingType::OUTPUT_DATA_RATE);
	sensorSettings.push_back(OutputDataRate);

	//Full-scale Range
	SensorSettings FullscaleRange;
	FullscaleRange.sensorSettingType = SensorSettingType::GYR_FULLSCALE_RANGE;
	FullscaleRange.possibleSettingOptions = { SensorSettingOptions::GYR_FSR_245_DS, SensorSettingOptions::GYR_FSR_500_DS, SensorSettingOptions::GYR_FSR_2000_DS };
	FullscaleRange.currentSettingOption = getRawSetting(SensorSettingType::GYR_FULLSCALE_RANGE);
	sensorSettings.push_back(FullscaleRange);

	//Filter General Settings
	SensorSettings FilterSettings;
	FilterSettings.sensorSettingType = SensorSettingType::FILTER_SETTINGS;
	FilterSettings.possibleSettingOptions = { SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_ONLY, SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2, SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_HPF, SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2_HPF };
	FilterSettings.currentSettingOption = getRawSetting(SensorSettingType::FILTER_SETTINGS);
	sensorSettings.push_back(FilterSettings);

	//High-pass Filter Frequency Setting (the options depend on wheter or not the HPF is actually turned on)
	SensorSettings HPFFreq;
	HPFFreq.sensorSettingType = SensorSettingType::HIGH_PASS_FILTER_FREQ;
	if (raw_settings[2] & 0b100)
	{
		//The cutoff rate for the HPF depends on the current ODR
		switch (OutputDataRate.currentSettingOption)
		{
		case SensorSettingOptions::ODR_14_9_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_001_HZ, SensorSettingOptions::HPF_0_002_HZ, SensorSettingOptions::HPF_0_005_HZ, SensorSettingOptions::HPF_0_01_HZ, SensorSettingOptions::HPF_0_02_HZ, SensorSettingOptions::HPF_0_05_HZ, SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ };
			break;
		case SensorSettingOptions::ODR_59_5_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_005_HZ, SensorSettingOptions::HPF_0_01_HZ, SensorSettingOptions::HPF_0_02_HZ, SensorSettingOptions::HPF_0_05_HZ, SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ, SensorSettingOptions::HPF_2_HZ, SensorSettingOptions::HPF_4_HZ };
			break;
		case SensorSettingOptions::ODR_119_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_01_HZ, SensorSettingOptions::HPF_0_02_HZ, SensorSettingOptions::HPF_0_05_HZ, SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ, SensorSettingOptions::HPF_2_HZ, SensorSettingOptions::HPF_4_HZ, SensorSettingOptions::HPF_8_HZ };
			break;
		case SensorSettingOptions::ODR_238_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_02_HZ, SensorSettingOptions::HPF_0_05_HZ, SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ, SensorSettingOptions::HPF_2_HZ, SensorSettingOptions::HPF_4_HZ, SensorSettingOptions::HPF_8_HZ, SensorSettingOptions::HPF_15_HZ };
			break;
		case SensorSettingOptions::ODR_476_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_05_HZ, SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ, SensorSettingOptions::HPF_2_HZ, SensorSettingOptions::HPF_4_HZ, SensorSettingOptions::HPF_8_HZ, SensorSettingOptions::HPF_15_HZ, SensorSettingOptions::HPF_30_HZ };
			break;
		case SensorSettingOptions::ODR_952_HZ:
			HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_0_1_HZ, SensorSettingOptions::HPF_0_2_HZ, SensorSettingOptions::HPF_0_5_HZ, SensorSettingOptions::HPF_1_HZ, SensorSettingOptions::HPF_2_HZ, SensorSettingOptions::HPF_4_HZ, SensorSettingOptions::HPF_8_HZ, SensorSettingOptions::HPF_15_HZ, SensorSettingOptions::HPF_30_HZ, SensorSettingOptions::HPF_57_HZ };
			break;
		}

		HPFFreq.currentSettingOption = getRawSetting(SensorSettingType::HIGH_PASS_FILTER_FREQ);
	}
	else
	{
		//The filter isn't turned on so the only option is N/A
		HPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_N_A };
		HPFFreq.currentSettingOption = SensorSettingOptions::HPF_N_A;
	}
	sensorSettings.push_back(HPFFreq);

	//Low-pass Filter 2 Frequency Setting (the options depend on wheter or not the LPF2 is actually turned on)
	SensorSettings LPFFreq;
	LPFFreq.sensorSettingType = SensorSettingType::LOW_PASS_FILTER_FREQ;
	if (raw_settings[2] & 0x10)
	{
		//The cutoff rate for the LPF2 depends on the current ODR
		switch (OutputDataRate.currentSettingOption)
		{
		case SensorSettingOptions::ODR_14_9_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_N_A };
			break;
		case SensorSettingOptions::ODR_59_5_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_16_HZ };
			break;
		case SensorSettingOptions::ODR_119_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_14_HZ, SensorSettingOptions::LPF_31_HZ };
			break;
		case SensorSettingOptions::ODR_238_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_14_HZ, SensorSettingOptions::LPF_29_HZ, SensorSettingOptions::LPF_63_HZ, SensorSettingOptions::LPF_78_HZ };
			break;
		case SensorSettingOptions::ODR_476_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_21_HZ, SensorSettingOptions::LPF_28_HZ, SensorSettingOptions::LPF_57_HZ, SensorSettingOptions::LPF_100_HZ };
			break;
		case SensorSettingOptions::ODR_952_HZ:
			LPFFreq.possibleSettingOptions = { SensorSettingOptions::LPF_33_HZ, SensorSettingOptions::LPF_40_HZ, SensorSettingOptions::LPF_58_HZ, SensorSettingOptions::LPF_100_HZ };
			break;
		}
		LPFFreq.currentSettingOption = getRawSetting(SensorSettingType::LOW_PASS_FILTER_FREQ);
	}
	else
	{
		//The filter isn't turned on so the only option is N/A
		LPFFreq.possibleSettingOptions = { SensorSettingOptions::HPF_N_A };
		LPFFreq.currentSettingOption = SensorSettingOptions::HPF_N_A;
	}
	sensorSettings.push_back(LPFFreq);
}

double LSM9DS1_GYR::getConversionFactor()
{
	//Returns the Acceleration sensitivity in dps/LSB, used by the IMU class
	if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::GYR_FSR_245_DS) return .00875;
	else if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::GYR_FSR_500_DS) return .0175;
	else if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::GYR_FSR_2000_DS) return .07;
	else return -1; //shouldn't be possible to have another option, so return this to indicate an error
}

SensorSettingOptions LSM9DS1_GYR::getRawSetting(SensorSettingType sensorSetting)
{
	//The LSM9DS1 Gyroscope raw setting byte array has the following form:
	//byte 0: ODR setting
	//byte 1: Full-scale range setting
	//byte 2: Filter General Settings
	//byte 3: High Pass Filter Settings
	//byte 4: Low Pass Filter Settings
	//byte 5: Operating Mode

	if (sensorSetting == SensorSettingType::OUTPUT_DATA_RATE)
	{
		//The ODR options will depend on whether or not the Gyroscope of the LSM9DS1 is also active
		switch (raw_settings[0])
		{
		case 0b001: return SensorSettingOptions::ODR_14_9_HZ;
		case 0b010: return SensorSettingOptions::ODR_59_5_HZ;
		case 0b011: return SensorSettingOptions::ODR_119_HZ;
		case 0b100: return SensorSettingOptions::ODR_238_HZ;
		case 0b101: return SensorSettingOptions::ODR_476_HZ;
		case 0b110: return SensorSettingOptions::ODR_952_HZ;
		default: return SensorSettingOptions::ODR_N_A;
		}
	}
	else if (sensorSetting == SensorSettingType::GYR_FULLSCALE_RANGE)
	{
		//The LSM9DS1 gyroscope has 4 different options for the full-scale range.

		switch (raw_settings[1])
		{
		case 0b00: return SensorSettingOptions::GYR_FSR_245_DS;
		case 0b01: return SensorSettingOptions::GYR_FSR_500_DS;
		case 0b11: return SensorSettingOptions::GYR_FSR_2000_DS;
		default: return SensorSettingOptions::ACC_FSR_N_A;
		}
	}
	else if (sensorSetting == SensorSettingType::FILTER_SETTINGS)
	{
		//The LSM9DS1 gyroscope has 3 filters built into it:
		//1. A (mandatory) low-pass filter LPF1. The cutoff frequency for this filter
		//   depends on the current ODR. 14.9 Hz ODR -> 5 HZ, 59.9 HZ ODR -> 19 HZ,
		//   119 HZ ODR -> 38 HZ, 238 HZ ODR -> 76 HZ, 476 HZ ODR -> 100 HZ, 
		//   952 HZ ODR -> 100 HZ,
		//2. A second (optional) low pass filter LPF2.
		//3. An (optional) high pass filter HPF.
		//
		//The user can decide what combination of filters they want to use, there
		//are four options in total: LPF1 only, LPF1 + LPF2, LPF1 + HPF and
		//LPF1 + LPF2 + HPF

		switch (raw_settings[2])
		{
		case 0b010: return SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2;
		case 0b101: return SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_HPF;
		case 0b111: return SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2_HPF;
		default: return SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_ONLY;
		}
	}
	else if (sensorSetting == SensorSettingType::HIGH_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the optional high pass
		//filter for the LSM9DS1 gyroscope. This filter needs to be turned on
		//to actual select one of the below options. The raw data code depends
		//on the current ODR of the gyroscope.

		if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_14_9_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_1_HZ;
			case 0b0001: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b0010: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b0011: return SensorSettingOptions::HPF_0_1_HZ;
			case 0b0100: return SensorSettingOptions::HPF_0_05_HZ;
			case 0b0101: return SensorSettingOptions::HPF_0_02_HZ;
			case 0b0110: return SensorSettingOptions::HPF_0_01_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_005_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_002_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_001_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_59_5_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_4_HZ;
			case 0b0001: return SensorSettingOptions::HPF_2_HZ;
			case 0b0010: return SensorSettingOptions::HPF_1_HZ;
			case 0b0011: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b0100: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b0101: return SensorSettingOptions::HPF_0_1_HZ;
			case 0b0110: return SensorSettingOptions::HPF_0_05_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_02_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_01_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_005_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_119_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_8_HZ;
			case 0b0001: return SensorSettingOptions::HPF_4_HZ;
			case 0b0010: return SensorSettingOptions::HPF_2_HZ;
			case 0b0011: return SensorSettingOptions::HPF_1_HZ;
			case 0b0100: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b0101: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b0110: return SensorSettingOptions::HPF_0_1_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_05_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_02_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_01_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_238_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_15_HZ;
			case 0b0001: return SensorSettingOptions::HPF_8_HZ;
			case 0b0010: return SensorSettingOptions::HPF_4_HZ;
			case 0b0011: return SensorSettingOptions::HPF_2_HZ;
			case 0b0100: return SensorSettingOptions::HPF_1_HZ;
			case 0b0101: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b0110: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_1_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_05_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_02_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_476_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_30_HZ;
			case 0b0001: return SensorSettingOptions::HPF_15_HZ;
			case 0b0010: return SensorSettingOptions::HPF_8_HZ;
			case 0b0011: return SensorSettingOptions::HPF_4_HZ;
			case 0b0100: return SensorSettingOptions::HPF_2_HZ;
			case 0b0101: return SensorSettingOptions::HPF_1_HZ;
			case 0b0110: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_1_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_05_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_952_HZ)
		{
			switch (raw_settings[3])
			{
			default: return SensorSettingOptions::HPF_57_HZ;
			case 0b0001: return SensorSettingOptions::HPF_30_HZ;
			case 0b0010: return SensorSettingOptions::HPF_15_HZ;
			case 0b0011: return SensorSettingOptions::HPF_8_HZ;
			case 0b0100: return SensorSettingOptions::HPF_4_HZ;
			case 0b0101: return SensorSettingOptions::HPF_2_HZ;
			case 0b0110: return SensorSettingOptions::HPF_1_HZ;
			case 0b0111: return SensorSettingOptions::HPF_0_5_HZ;
			case 0b1000: return SensorSettingOptions::HPF_0_2_HZ;
			case 0b1001: return SensorSettingOptions::HPF_0_1_HZ;
			}
		}
	}
	else if (sensorSetting == SensorSettingType::LOW_PASS_FILTER_FREQ)
	{
		//This represents the frequency of the optional LPF2
		//for the LSM9DS1 gyroscope. The value of the raw byte
		//will change with the current ODR of the gyroscope
		if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_14_9_HZ)
		{
			return SensorSettingOptions::LPF_N_A;
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_59_5_HZ)
		{
			return SensorSettingOptions::LPF_16_HZ;
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_119_HZ)
		{
			switch (raw_settings[4])
			{
			case 0b00: return SensorSettingOptions::LPF_14_HZ;
			default: return SensorSettingOptions::LPF_31_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_119_HZ)
		{
			switch (raw_settings[4])
			{
			default: return SensorSettingOptions::LPF_14_HZ;
			case 0b01: return SensorSettingOptions::LPF_29_HZ;
			case 0b10: return SensorSettingOptions::LPF_63_HZ;
			case 0b11: return SensorSettingOptions::LPF_78_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_476_HZ)
		{
			switch (raw_settings[4])
			{
			default: return SensorSettingOptions::LPF_21_HZ;
			case 0b01: return SensorSettingOptions::LPF_28_HZ;
			case 0b10: return SensorSettingOptions::LPF_57_HZ;
			case 0b11: return SensorSettingOptions::LPF_100_HZ;
			}
		}
		else if (getCurrentSettingOption(SensorSettingType::OUTPUT_DATA_RATE) == SensorSettingOptions::ODR_952_HZ)
		{
			switch (raw_settings[4])
			{
			default: return SensorSettingOptions::LPF_33_HZ;
			case 0b01: return SensorSettingOptions::LPF_40_HZ;
			case 0b10: return SensorSettingOptions::LPF_58_HZ;
			case 0b11: return SensorSettingOptions::LPF_100_HZ;
			}
		}
		
	}
	else if (sensorSetting == SensorSettingType::OPERATING_MODE)
	{
		//The LSM9DS1 accelerometer has two different operating modes (for now),
		//with Gyroscope, or without Gyroscope
		switch (raw_settings[5])
		{
		case 0b0: return SensorSettingOptions::OM_GYR_REGULAR;
		case 0b1: return SensorSettingOptions::OM_GYR_LOW;
		}
	}
}

bool LSM9DS1_GYR::optionCascade(SensorSettingType sensorSetting)
{
	//TODO: Need to implement
	return false;
}

int LSM9DS1_GYR::getRawSettingLocation(SensorSettingType sensorSetting)
{
	//returns the location in the raw settings array of the given setting
	switch (sensorSetting)
	{
	case SensorSettingType::OUTPUT_DATA_RATE: return 0;
	case SensorSettingType::ACC_FULLSCALE_RANGE: return 1;
	case SensorSettingType::FILTER_SETTINGS: return 2;
	case SensorSettingType::HIGH_PASS_FILTER_FREQ: return 3;
	case SensorSettingType::LOW_PASS_FILTER_FREQ: return 4;
	case SensorSettingType::OPERATING_MODE: return 5;
	default: return -1; //incorrect setting type
	}
}

uint8_t LSM9DS1_GYR::getByte(SensorSettingOptions sensorOption)
{
	//TODO: Need to implement
	return 0;
}

//Magnetometer Functions
LSM9DS1_MAG::LSM9DS1_MAG(winrt::Windows::Storage::Streams::DataReader inputData)
{
	//This is where all of the default settings for the sensor go
	sensorType = SensorType::MAGNETOMETER;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = inputData.ReadByte();
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

LSM9DS1_MAG::LSM9DS1_MAG(uint8_t* inputData)
{
	//Same as the above constructor, but takes a standard array
	sensorType = SensorType::MAGNETOMETER;

	//First copy and persist the raw data
	for (int i = 0; i < 18; i++) raw_settings[i] = *(inputData + i);
	name = "LSM9DS1"; //Set the name

	updateSettingVectors();
}

void LSM9DS1_MAG::updateSettingVectors()
{
	//First, clear out the existing vector if it isn't empty
	if (sensorSettings.size() > 0) sensorSettings.clear();

	//TODO: Currently I'm manually setting all of the possibleSettingOptions vectors. I should utilize the chained
	//enums to do this manually

	//setting creation
	//Operating Mode (*note, there are a lot of operating modes for the magnetometer. Ultimately I may lock X, Y and Z axis
	//performance to be equal which will greatly cut down on the options)
	SensorSettings OperatingMode;
	OperatingMode.sensorSettingType = SensorSettingType::OPERATING_MODE;
	OperatingMode.possibleSettingOptions = { SensorSettingOptions::OM_MAG_LPXYZ_CC, SensorSettingOptions::OM_MAG_LPXYZ_SC, SensorSettingOptions::OM_MAG_LPXY_MPZ_CC, SensorSettingOptions::OM_MAG_LPXY_MPZ_SC, SensorSettingOptions::OM_MAG_LPXY_HPZ_CC, SensorSettingOptions::OM_MAG_LPXY_HPZ_SC,
		SensorSettingOptions::OM_MAG_LPXY_UPZ_CC, SensorSettingOptions::OM_MAG_LPXY_UPZ_SC, SensorSettingOptions::OM_MAG_MPXY_LPZ_CC, SensorSettingOptions::OM_MAG_MPXY_LPZ_SC, SensorSettingOptions::OM_MAG_MPXYZ_CC, SensorSettingOptions::OM_MAG_MPXYZ_SC,
		SensorSettingOptions::OM_MAG_MPXY_HPZ_CC, SensorSettingOptions::OM_MAG_MPXY_HPZ_SC, SensorSettingOptions::OM_MAG_MPXY_UPZ_CC, SensorSettingOptions::OM_MAG_MPXY_UPZ_SC, SensorSettingOptions::OM_MAG_HPXY_LPZ_CC, SensorSettingOptions::OM_MAG_HPXY_LPZ_SC,
		SensorSettingOptions::OM_MAG_HPXY_MPZ_CC, SensorSettingOptions::OM_MAG_HPXY_MPZ_SC, SensorSettingOptions::OM_MAG_HPXYZ_CC, SensorSettingOptions::OM_MAG_HPXYZ_SC, SensorSettingOptions::OM_MAG_HPXY_UPZ_CC, SensorSettingOptions::OM_MAG_HPXY_UPZ_SC,
		SensorSettingOptions::OM_MAG_UPXY_LPZ_CC, SensorSettingOptions::OM_MAG_UPXY_LPZ_SC, SensorSettingOptions::OM_MAG_UPXY_MPZ_CC, SensorSettingOptions::OM_MAG_UPXY_MPZ_SC, SensorSettingOptions::OM_MAG_UPXY_HPZ_CC, SensorSettingOptions::OM_MAG_UPXY_HPZ_SC,
		SensorSettingOptions::OM_MAG_UPXYZ_CC, SensorSettingOptions::OM_MAG_UPXYZ_SC };
	OperatingMode.currentSettingOption = getRawSetting(SensorSettingType::OPERATING_MODE);
	sensorSettings.push_back(OperatingMode);

	//Output Date Rate
	SensorSettings OutputDataRate;
	OutputDataRate.sensorSettingType = SensorSettingType::OUTPUT_DATA_RATE;
	OutputDataRate.possibleSettingOptions = { SensorSettingOptions::ODR_0_625_HZ, SensorSettingOptions::ODR_1_25_HZ, SensorSettingOptions::ODR_2_5_HZ, SensorSettingOptions::ODR_5_HZ, SensorSettingOptions::ODR_10_HZ, SensorSettingOptions::ODR_20_HZ, SensorSettingOptions::ODR_40_HZ, SensorSettingOptions::ODR_80_HZ };
	OutputDataRate.currentSettingOption = getRawSetting(SensorSettingType::OUTPUT_DATA_RATE);
	sensorSettings.push_back(OutputDataRate);

	//Full-scale Range
	SensorSettings FullscaleRange;
	FullscaleRange.sensorSettingType = SensorSettingType::MAG_FULLSCALE_RANGE;
	FullscaleRange.possibleSettingOptions = { SensorSettingOptions::MAG_FSR_N_A, SensorSettingOptions::MAG_FSR_4_GA, SensorSettingOptions::MAG_FSR_8_GA, SensorSettingOptions::MAG_FSR_12_GA, SensorSettingOptions::MAG_FSR_16_GA };
	FullscaleRange.currentSettingOption = getRawSetting(SensorSettingType::MAG_FULLSCALE_RANGE);
	sensorSettings.push_back(FullscaleRange);

	//The magnetometer doesn't have any filter settings that can be altered
}

double LSM9DS1_MAG::getConversionFactor()
{
	//Returns the Acceleration sensitivity in gauss/LSB, used by the IMU class
	if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::MAG_FSR_4_GA) return .00014;
	else if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::MAG_FSR_8_GA) return .00029;
	else if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::MAG_FSR_12_GA) return .00043;
	else if (getCurrentSettingOption(SensorSettingType::GYR_FULLSCALE_RANGE) == SensorSettingOptions::MAG_FSR_16_GA) return .00058;
	else return -1; //shouldn't be possible to have another option, so return this to indicate an error
}

SensorSettingOptions LSM9DS1_MAG::getRawSetting(SensorSettingType sensorSetting)
{
	//The LSM9DS1 Gyroscope raw setting byte array has the following form:
	//byte 0: ODR setting
	//byte 1: Full-scale range setting
	//byte 2: Not used
	//byte 3: Not Used
	//byte 4: Not Used
	//byte 5: Operating Mode

	if (sensorSetting == SensorSettingType::OUTPUT_DATA_RATE)
	{
		//The ODR options will depend on whether or not the Gyroscope of the LSM9DS1 is also active
		switch (raw_settings[0])
		{
		case 0b0000: return SensorSettingOptions::ODR_0_625_HZ;
		case 0b0001: return SensorSettingOptions::ODR_1_25_HZ;
		case 0b0010: return SensorSettingOptions::ODR_2_5_HZ;
		case 0b0011: return SensorSettingOptions::ODR_5_HZ;
		case 0b0100: return SensorSettingOptions::ODR_10_HZ;
		case 0b0101: return SensorSettingOptions::ODR_20_HZ;
		case 0b0110: return SensorSettingOptions::ODR_40_HZ;
		case 0b0111: return SensorSettingOptions::ODR_80_HZ;
		case 0b1000: return SensorSettingOptions::ODR_FAST;
		default: return SensorSettingOptions::ODR_N_A;
		}
	}
	else if (sensorSetting == SensorSettingType::MAG_FULLSCALE_RANGE)
	{
		//The LSM9DS1 magnetometer has 4 different options for the full-scale range.

		switch (raw_settings[1])
		{
		case 0b00: return SensorSettingOptions::MAG_FSR_4_GA;
		case 0b01: return SensorSettingOptions::MAG_FSR_8_GA;
		case 0b10: return SensorSettingOptions::MAG_FSR_12_GA;
		case 0b11: return SensorSettingOptions::MAG_FSR_16_GA;
		default: return SensorSettingOptions::MAG_FSR_N_A;
		}
	}
	else if (sensorSetting == SensorSettingType::OPERATING_MODE)
	{
		//The LSM9DS1 magnetometer has 32 different operating modes. The reason for this
		//is because the X+Y axes can have their power level set independently from the Z
		//axis, and each axis set has 4 power levels. Furthermore, the magnetometer can
		//be set to read in Single or Continuous conversion mode. 4 * 4 * 2 = 32. In the
		//future I might force the X, Y and Z axes to all share the same power level which
		//would bring the total amount of options all the ways down from 32 to 8 which is
		//more reasonable.
		switch (raw_settings[5])
		{
		default: return SensorSettingOptions::OM_MAG_LPXYZ_CC;
		case 0x000001: return SensorSettingOptions::OM_MAG_LPXYZ_SC;
		case 0x000100: return SensorSettingOptions::OM_MAG_LPXY_MPZ_CC;
		case 0x000101: return SensorSettingOptions::OM_MAG_LPXY_MPZ_SC;
		case 0x001000: return SensorSettingOptions::OM_MAG_LPXY_HPZ_CC;
		case 0x001001: return SensorSettingOptions::OM_MAG_LPXY_HPZ_SC;
		case 0x001100: return SensorSettingOptions::OM_MAG_LPXY_UPZ_CC;
		case 0x001101: return SensorSettingOptions::OM_MAG_LPXY_UPZ_SC;
		case 0x010000: return SensorSettingOptions::OM_MAG_MPXY_LPZ_CC;
		case 0x010001: return SensorSettingOptions::OM_MAG_MPXY_LPZ_SC;
		case 0x010100: return SensorSettingOptions::OM_MAG_MPXYZ_CC;
		case 0x010101: return SensorSettingOptions::OM_MAG_MPXYZ_SC;
		case 0x011000: return SensorSettingOptions::OM_MAG_MPXY_HPZ_CC;
		case 0x011001: return SensorSettingOptions::OM_MAG_MPXY_HPZ_SC;
		case 0x011100: return SensorSettingOptions::OM_MAG_MPXY_UPZ_CC;
		case 0x011101: return SensorSettingOptions::OM_MAG_MPXY_UPZ_SC;
		case 0x100000: return SensorSettingOptions::OM_MAG_HPXY_LPZ_CC;
		case 0x100001: return SensorSettingOptions::OM_MAG_HPXY_LPZ_SC;
		case 0x100100: return SensorSettingOptions::OM_MAG_HPXY_MPZ_CC;
		case 0x100101: return SensorSettingOptions::OM_MAG_HPXY_MPZ_SC;
		case 0x101000: return SensorSettingOptions::OM_MAG_HPXYZ_CC;
		case 0x101001: return SensorSettingOptions::OM_MAG_HPXYZ_SC;
		case 0x101100: return SensorSettingOptions::OM_MAG_HPXY_UPZ_CC;
		case 0x101101: return SensorSettingOptions::OM_MAG_HPXY_UPZ_SC;
		case 0x110000: return SensorSettingOptions::OM_MAG_UPXY_LPZ_CC;
		case 0x110001: return SensorSettingOptions::OM_MAG_UPXY_LPZ_SC;
		case 0x110100: return SensorSettingOptions::OM_MAG_UPXY_MPZ_CC;
		case 0x110101: return SensorSettingOptions::OM_MAG_UPXY_MPZ_SC;
		case 0x111000: return SensorSettingOptions::OM_MAG_UPXY_HPZ_CC;
		case 0x111001: return SensorSettingOptions::OM_MAG_UPXY_HPZ_SC;
		case 0x111100: return SensorSettingOptions::OM_MAG_UPXYZ_CC;
		case 0x111101: return SensorSettingOptions::OM_MAG_UPXYZ_SC;
		}
	}
}

bool LSM9DS1_MAG::optionCascade(SensorSettingType sensorSetting)
{
	//TODO: Need to implement
	return false;
}

int LSM9DS1_MAG::getRawSettingLocation(SensorSettingType sensorSetting)
{
	//returns the location in the raw settings array of the given setting
	switch (sensorSetting)
	{
	case SensorSettingType::OUTPUT_DATA_RATE: return 0;
	case SensorSettingType::ACC_FULLSCALE_RANGE: return 1;
	case SensorSettingType::FILTER_SETTINGS: return 2;
	case SensorSettingType::HIGH_PASS_FILTER_FREQ: return 3;
	case SensorSettingType::LOW_PASS_FILTER_FREQ: return 4;
	case SensorSettingType::OPERATING_MODE: return 5;
	default: return -1; //incorrect setting type
	}
}

uint8_t LSM9DS1_MAG::getByte(SensorSettingOptions sensorOption)
{
	//TODO: Need to implement
	return 0;
}