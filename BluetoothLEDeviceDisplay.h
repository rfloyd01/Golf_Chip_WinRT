#pragma once
#include "BluetoothLEDeviceDisplay.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct BluetoothLEDeviceDisplay : BluetoothLEDeviceDisplayT<BluetoothLEDeviceDisplay>
    {
        BluetoothLEDeviceDisplay() = default;

        winrt::Windows::Devices::Enumeration::DeviceInformation DeviceInformation();
        hstring Id();
        hstring Name();
        bool IsPaired();
        bool IsConnected();
        bool IsConnectable();
        hstring Address();
        winrt::Windows::Foundation::Collections::IMapView<hstring, winrt::Windows::Foundation::IInspectable> Properties();
        winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage GlyphBitmapImage();
        void Update(winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate);
        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
    };
}