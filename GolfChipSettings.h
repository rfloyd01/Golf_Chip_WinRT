#pragma once
#include "GolfChipSettings.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct GolfChipSettings : GolfChipSettingsT<GolfChipSettings>
    {
        GolfChipSettings() = default;

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> KnownDevices();
        winrt::Windows::UI::Xaml::Controls::ListView ResultsListView();
        void EnumerateButton_Click();
        void ConnectButton_Click();
        void DisconnectButton_Click();
        bool Not(bool value);
        void NotifyUser(hstring const& strMessage, winrt::Golf_Chip_WinRT::NotifyType const& type);
    };
}
namespace winrt::Golf_Chip_WinRT::factory_implementation
{
    struct GolfChipSettings : GolfChipSettingsT<GolfChipSettings, implementation::GolfChipSettings>
    {
    };
}
