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
            OutputDebugStringW((L"Added the following Setting Type: " + m_settingType + L"\n").c_str());
            for (int i = 0; i < sensorSettings.possibleSettingOptions.size(); i++)
            {
                m_availableOptions.Append(box_value(to_hstring(sensorSettings.possibleSettingOptions[i])));
                if (sensorSettings.possibleSettingOptions[i] == sensorSettings.currentSettingOption) m_selectedOption = i;
            }
        }

        hstring SettingType() { return m_settingType; }
        int32_t SelectedOption() { return m_selectedOption; }
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> AvailableOptions() { return m_availableOptions; }
        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        hstring m_settingType;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> m_availableOptions = single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        int32_t m_selectedOption{ 0 };

        hstring to_hstring(SensorSettingType type);
        hstring to_hstring(SensorSettingOptions option);

        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
