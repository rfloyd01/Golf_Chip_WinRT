#pragma once
#include "GolfChipSettingsDisplay.g.h"

#include "GolfChip_Files/Sensors/SensorSettings.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct GolfChipSettingsDisplay : GolfChipSettingsDisplayT<GolfChipSettingsDisplay>
    {
        GolfChipSettingsDisplay(SensorSettings sensorSettings)
        {
            m_settingType = to_hstring(sensorSettings.sensorSettingType);
            underlyingType = static_cast<int32_t>(sensorSettings.sensorSettingType);

            for (int i = 0; i < sensorSettings.possibleSettingOptions.size(); i++)
            {
                underlyingOptions.push_back(static_cast<int32_t>(sensorSettings.possibleSettingOptions[i]));
                m_availableOptions.Append(box_value(to_hstring(sensorSettings.possibleSettingOptions[i])));
                if (sensorSettings.possibleSettingOptions[i] == sensorSettings.currentSettingOption) m_selectedOption = i;
            }
        }

        hstring SettingType() { return m_settingType; }
        int32_t SelectedOption() { return m_selectedOption; }
        void SelectedOption(int32_t const& value) { m_selectedOption = value; }
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> AvailableOptions() { return m_availableOptions; }
        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
        void OnPropertyChanged(param::hstring const& property);
        //void UpdateOption(int32_t const& deviceInfoUpdate);
        int32_t GetUnderlyingSetting() { return underlyingType; }
        int32_t GetUnderlyingOption(int i) { return underlyingOptions[i]; } //TODO: Need to be careful and make sure i isn't out of range when calling this

    private:
        hstring m_settingType;
        int32_t underlyingType; //the integer can be cast to the SensorSettingType enum class elsewhere
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_availableOptions = single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        std::vector<int32_t> underlyingOptions; //the integers can be cast to the SensorSettingOptions enum class elsewhere
        int32_t m_selectedOption{ 0 };

        hstring to_hstring(SensorSettingType type);
        hstring to_hstring(SensorSettingOptions option);
        //SensorSettingType to_SensorSettingType(hstring type);
        //SensorSettingOptions to_SensorSettingOptions(hstring option);

        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
