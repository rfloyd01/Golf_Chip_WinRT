#pragma once
#include "BluetoothLEDeviceDisplay.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct BluetoothLEDeviceDisplay : BluetoothLEDeviceDisplayT<BluetoothLEDeviceDisplay>
    {
        BluetoothLEDeviceDisplay(Windows::Devices::Enumeration::DeviceInformation const& deviceInfoIn);

        winrt::Windows::Devices::Enumeration::DeviceInformation DeviceInformation() { return m_deviceInformation; }
        hstring Id() { return m_deviceInformation.Id(); }
        hstring Name() { return m_deviceInformation.Name(); }
        bool IsPaired() { return m_deviceInformation.Pairing().IsPaired(); }
        bool IsConnected() { return LookupBooleanProperty(L"System.Devices.Aep.IsConnected"); }
        bool IsConnectable() { return LookupBooleanProperty(L"System.Devices.Aep.Bluetooth.Le.IsConnectable"); }
        hstring Address()
        {
            auto value = m_deviceInformation.Properties().TryLookup(L"System.Devices.Aep.DeviceAddress");
            return unbox_value<hstring>(value);
        }

        winrt::Windows::Foundation::Collections::IMapView<hstring, winrt::Windows::Foundation::IInspectable> Properties() { return m_deviceInformation.Properties(); }
        winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage GlyphBitmapImage() { return m_glyphBitmapImage; }
        void Update(winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate);

        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        Windows::Devices::Enumeration::DeviceInformation m_deviceInformation{ nullptr };
        Windows::UI::Xaml::Media::Imaging::BitmapImage m_glyphBitmapImage{ nullptr };
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        fire_and_forget UpdateGlyphBitmapImage();
        void OnPropertyChanged(param::hstring const& property);
        bool LookupBooleanProperty(param::hstring const& property);
    };
}