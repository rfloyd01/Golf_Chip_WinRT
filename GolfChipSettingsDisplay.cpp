#include "pch.h"
#include "GolfChipSettingsDisplay.h"
#include "GolfChipSettingsDisplay.g.cpp"

namespace winrt::Golf_Chip_WinRT::implementation
{
    //These functions exist to convert the sensor setting enums into readable strings
    hstring GolfChipSettingsDisplay::to_hstring(SensorSettingType type)
    {
        switch (type)
        {
        case SensorSettingType::ACC_FULLSCALE_RANGE: return L"Full Scale Range";
        case SensorSettingType::GYR_FULLSCALE_RANGE: return L"Full Scale Range";
        case SensorSettingType::MAG_FULLSCALE_RANGE: return L"Full Scale Range";
        case SensorSettingType::FILTER_SETTINGS: return L"Filter Settings";
        case SensorSettingType::HIGH_PASS_FILTER_FREQ: return L"High Pass Filter Frequency Cutoff";
        case SensorSettingType::LOW_PASS_FILTER_FREQ: return L"Low Pass Filter Frequency Cutoff";
        case SensorSettingType::OPERATING_MODE: return L"Operating Mode";
        case SensorSettingType::OUTPUT_DATA_RATE: return L"Output Data Rate";
        }
        return L"Sensor Setting Type Not Found";
    }

    hstring GolfChipSettingsDisplay::to_hstring(SensorSettingOptions option)
    {
        switch (option)
        {
        //Output Data Rate Options
		case SensorSettingOptions::ODR_N_A: return L"N/A";
		case SensorSettingOptions::ODR_0_625_HZ: return L"0.625 Hz";
		case SensorSettingOptions::ODR_1_25_HZ: return L"1.25 Hz";
		case SensorSettingOptions::ODR_2_5_HZ: return L"2.5 Hz";
		case SensorSettingOptions::ODR_5_HZ: return L"5 Hz";
		case SensorSettingOptions::ODR_10_HZ: return L" 10 Hz";
		case SensorSettingOptions::ODR_14_9_HZ: return L"14.9 Hz";
		case SensorSettingOptions::ODR_20_HZ: return L"20 Hz";
		case SensorSettingOptions::ODR_40_HZ: return L"40 Hz";
		case SensorSettingOptions::ODR_50_HZ: return L"50 Hz";
		case SensorSettingOptions::ODR_59_5_HZ: return L"59.5 Hz";
		case SensorSettingOptions::ODR_80_HZ: return L"80 Hz";
		case SensorSettingOptions::ODR_119_HZ: return L"119 Hz";
		case SensorSettingOptions::ODR_238_HZ: return L"238 Hz";
		case SensorSettingOptions::ODR_476_HZ: return L"476 Hz";
		case SensorSettingOptions::ODR_952_HZ: return L"952 Hz";

		//Accelerometer Fullscale Range Options
		case SensorSettingOptions::ACC_FSR_N_A: return L"N/A";
		case SensorSettingOptions::ACC_FSR_2_G: return L"+/- 2 G";
		case SensorSettingOptions::ACC_FSR_4_G: return L"+/- 4 G";
		case SensorSettingOptions::ACC_FSR_8_G: return L"+/- 8 G";
		case SensorSettingOptions::ACC_FSR_16_G: return L"+/- 16 G";

		//Gyroscope Fullscale Range Options
		case SensorSettingOptions::GYR_FSR_N_A: return L"N/A";
		case SensorSettingOptions::GYR_FSR_245_DS: return L"+/- 245 Deg/s";
		case SensorSettingOptions::GYR_FSR_500_DS: return L"+/- 500 Deg/s";
		case SensorSettingOptions::GYR_FSR_2000_DS: return L"+/- 2000 Deg/s";

		//Magnetometer Fullscale Range Options
		case SensorSettingOptions::MAG_FSR_N_A: return L"N/A";
		case SensorSettingOptions::MAG_FSR_4_GA: return L"+/- 4 Gauss";
		case SensorSettingOptions::MAG_FSR_8_GA: return L"+/- 8 Gauss";
		case SensorSettingOptions::MAG_FSR_12_GA: return L"+/- 12 Gauss";
		case SensorSettingOptions::MAG_FSR_16_GA: return L"+/- 16 Gauss";

		//Filter General Settings
		case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_NO_FILTERS: return L"Auto mode, No additional Filters";
		case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_NO_FILTERS: return L"Manual mode, No additional Filters";
		case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_LPF2: return L"Auto mode, LPF2 Active";
		case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_LPF2: return L"Manual mode, LPF2 Active";
		case SensorSettingOptions::FS_LSM9DS1_ACC_AUTO_HPF: return L"Auto mode, HPF Active";
		case SensorSettingOptions::FS_LSM9DS1_ACC_MANUAL_HPF: return L"Manual mode, HPF Active";

		//High Pass Filter Frequency Options
		case SensorSettingOptions::HPF_N_A: return L"N/A";
		case SensorSettingOptions::HPF_ODR_OVER_50_HZ: return L"ODR/50 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_100_HZ: return L"ODR/100 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_9_HZ: return L"ODR/9 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_400_HZ: return L"ODR/400 Hz";

		//Low Pass Filter Frequency Options
		case SensorSettingOptions::LPF_N_A: return L"N/A";
		case SensorSettingOptions::LPF_408_HZ: return L"408 Hz";
		case SensorSettingOptions::LPF_211_HZ: return L"211 Hz";
		case SensorSettingOptions::LPF_105_HZ: return L"105 Hz";
		case SensorSettingOptions::LPF_50_HZ: return L"50 Hz";

		//Operating Mode Options
		case SensorSettingOptions::OM_ACC_ONLY: return L"Accelerometer Only";
		case SensorSettingOptions::OM_ACC_AND_GYR: return L"Accelerometer + Gyroscope";
        }
        return L"Sensor Option Not Found";
    }
}

namespace winrt::Golf_Chip_WinRT::implementation
{
    winrt::event_token GolfChipSettingsDisplay::PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }
    void GolfChipSettingsDisplay::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
