#pragma once
#include "GolfChipSettings.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct GolfChipSettings : GolfChipSettingsT<GolfChipSettings>
    {
        GolfChipSettings()
        {

        }

        void mainPageNavigate(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> KnownDevices()
        {
            return m_knownDevices;
        }
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> DeviceSettings()
        {
            return m_deviceSettings;
        }

        void EnumerateButton_Click();
        fire_and_forget ConnectButton_Click();
        void DisconnectButton_Click();
        bool Not(bool value) { return !value; }

        void NotifyUser(hstring const& strMessage, winrt::Golf_Chip_WinRT::NotifyType const& type);
        void UpdateStatus(const hstring& strMessage, NotifyType type);

    private:
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_knownDevices = single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_deviceSettings = single_threaded_observable_vector<Windows::Foundation::IInspectable>();

        std::vector<Windows::Devices::Enumeration::DeviceInformation> UnknownDevices;
        Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
        event_token deviceWatcherAddedToken;
        event_token deviceWatcherUpdatedToken;
        event_token deviceWatcherRemovedToken;
        event_token deviceWatcherEnumerationCompletedToken;
        event_token deviceWatcherStoppedToken;

        void DisplaySearchMode();
        void DisplaySettingsMode();
        void SetSettingVectors();

        void StartBleDeviceWatcher();
        void StopBleDeviceWatcher();
        std::tuple<Golf_Chip_WinRT::BluetoothLEDeviceDisplay, uint32_t> FindBluetoothLeDeviceDisplay(hstring const& id);
        std::vector<Windows::Devices::Enumeration::DeviceInformation>::iterator FindUnknownDevices(hstring const& id);

        fire_and_forget DeviceWatcher_Added(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformation deviceInfo);
        fire_and_forget DeviceWatcher_Updated(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget DeviceWatcher_Removed(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget DeviceWatcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&);
        fire_and_forget DeviceWatcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&);

        uint64_t getBLEAddress(const hstring& unformattedAddress);
    };
}
namespace winrt::Golf_Chip_WinRT::factory_implementation
{
    struct GolfChipSettings : GolfChipSettingsT<GolfChipSettings, implementation::GolfChipSettings>
    {
    };
}
