#include "pch.h"
#include "GolfChipSettingsDisplay.h"
#include "GolfChipSettingsDisplay.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml::Data;

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
		case SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_ONLY: return L"LPF1 Only";
		case SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2: return L"LPF1 and LPF2";
		case SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_HPF: return L"LPF1 and HPF";
		case SensorSettingOptions::FS_LSM9DS1_GYR_LPF1_LPF2_HPF: return L"LPF1, LPF2 and HPF";

		//High Pass Filter Frequency Options
		case SensorSettingOptions::HPF_N_A: return L"N/A";
		case SensorSettingOptions::HPF_ODR_OVER_50_HZ: return L"ODR/50 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_100_HZ: return L"ODR/100 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_9_HZ: return L"ODR/9 Hz";
		case SensorSettingOptions::HPF_ODR_OVER_400_HZ: return L"ODR/400 Hz";
		case SensorSettingOptions::HPF_0_001_HZ: return L"0.001 Hz";
		case SensorSettingOptions::HPF_0_002_HZ: return L"0.002 Hz";
		case SensorSettingOptions::HPF_0_005_HZ: return L"0.005 Hz";
		case SensorSettingOptions::HPF_0_01_HZ: return L"0.01 Hz";
		case SensorSettingOptions::HPF_0_02_HZ: return L"0.02 Hz";
		case SensorSettingOptions::HPF_0_05_HZ: return L"0.05 Hz";
		case SensorSettingOptions::HPF_0_1_HZ: return L"0.1 Hz";
		case SensorSettingOptions::HPF_0_2_HZ: return L"0.2 Hz";
		case SensorSettingOptions::HPF_0_5_HZ: return L"0.5 Hz";
		case SensorSettingOptions::HPF_1_HZ: return L"1 Hz";
		case SensorSettingOptions::HPF_2_HZ: return L"2 Hz";
		case SensorSettingOptions::HPF_4_HZ: return L"4 Hz";
		case SensorSettingOptions::HPF_8_HZ: return L"8 Hz";
		case SensorSettingOptions::HPF_15_HZ: return L"15 Hz";
		case SensorSettingOptions::HPF_30_HZ: return L"30 Hz";
		case SensorSettingOptions::HPF_57_HZ: return L"57 Hz";

		//Low Pass Filter Frequency Options
		case SensorSettingOptions::LPF_N_A: return L"N/A";
		case SensorSettingOptions::LPF_14_HZ: return L"14 Hz";
		case SensorSettingOptions::LPF_16_HZ: return L"16 Hz";
		case SensorSettingOptions::LPF_21_HZ: return L"21 Hz";
		case SensorSettingOptions::LPF_28_HZ: return L"28 Hz";
		case SensorSettingOptions::LPF_29_HZ: return L"29 Hz";
		case SensorSettingOptions::LPF_31_HZ: return L"31 Hz";
		case SensorSettingOptions::LPF_33_HZ: return L"33 Hz";
		case SensorSettingOptions::LPF_40_HZ: return L"40 Hz";
		case SensorSettingOptions::LPF_50_HZ: return L"50 Hz";
		case SensorSettingOptions::LPF_57_HZ: return L"57 Hz";
		case SensorSettingOptions::LPF_58_HZ: return L"58 Hz";
		case SensorSettingOptions::LPF_63_HZ: return L"63 Hz";
		case SensorSettingOptions::LPF_78_HZ: return L"78 Hz";
		case SensorSettingOptions::LPF_100_HZ: return L"100 Hz";
		case SensorSettingOptions::LPF_105_HZ: return L"105 Hz";
		case SensorSettingOptions::LPF_211_HZ: return L"211 Hz";
		case SensorSettingOptions::LPF_408_HZ: return L"408 Hz";

		//Operating Mode Options
		case SensorSettingOptions::OM_ACC_ONLY: return L"Accelerometer Only";
		case SensorSettingOptions::OM_ACC_AND_GYR: return L"Accelerometer + Gyroscope";
		case SensorSettingOptions::OM_GYR_REGULAR: return L"Normal Mode";
		case SensorSettingOptions::OM_GYR_LOW: return L"Low Power Mode";
		case SensorSettingOptions::OM_MAG_LPXYZ_CC: return L"X/Y/Z Low Power, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_LPXYZ_SC: return L"X/Y/Z Low Power, Single Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_MPZ_CC: return L"X/Y Low Power, Z Medium Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_MPZ_SC: return L"X/Y Low Power, Z Medium Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_HPZ_CC: return L"X/Y Low Power, Z High Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_HPZ_SC: return L"X/Y Low Power, Z High Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_UPZ_CC: return L"X/Y Low Power, Z Ultra Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_LPXY_UPZ_SC: return L"X/Y Low Power, Z Ultra Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_LPZ_CC: return L"X/Y Medium Performance, Z Low Power, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_LPZ_SC: return L"X/Y Medium Performance, Z Low Power, Single Conversion";
		case SensorSettingOptions::OM_MAG_MPXYZ_CC: return L"X/Y/Z Medium Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_MPXYZ_SC: return L"X/Y/Z Medium Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_HPZ_CC: return L"X/Y Medium Performance, Z High Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_HPZ_SC: return L"X/Y Medium Performance, Z High Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_UPZ_CC: return L"X/Y Medium Performance, Z Ultra Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_MPXY_UPZ_SC: return L"X/Y Medium Performance, Z Ultra Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_LPZ_CC: return L"X/Y High Performance, Z Low Power, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_LPZ_SC: return L"X/Y High Performance, Z Low Power, Single Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_MPZ_CC: return L"X/Y High Performance, Z Medium Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_MPZ_SC: return L"X/Y High Performance, Z Medium Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_HPXYZ_CC: return L"X/Y/Z High Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_HPXYZ_SC: return L"X/Y/Z High Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_UPZ_CC: return L"X/Y High Performance, Z Ultra Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_HPXY_UPZ_SC: return L"X/Y High Performance, Z Ultra Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_LPZ_CC: return L"X/Y Ultra Performance, Z Low Power, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_LPZ_SC: return L"X/Y Ultra Performance, Z Low Power, Single Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_MPZ_CC: return L"X/Y Ultra Performance, Z Medium Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_MPZ_SC: return L"X/Y Ultra Performance, Z Medium Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_HPZ_CC: return L"X/Y Ultra Performance, Z High Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_UPXY_HPZ_SC: return L"X/Y Ultra Performance, Z High Performance, Single Conversion";
		case SensorSettingOptions::OM_MAG_UPXYZ_CC: return L"X/Y/Z Ultra Performance, Continuous Conversion";
		case SensorSettingOptions::OM_MAG_UPXYZ_SC: return L"X/Y/Z Ultra Performance, Single Conversion";
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

	void GolfChipSettingsDisplay::OnPropertyChanged(param::hstring const& property)
	{
		m_propertyChanged(*this, PropertyChangedEventArgs(property));
	}
}
