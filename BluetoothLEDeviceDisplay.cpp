#include "pch.h"
#include "BluetoothLEDeviceDisplay.h"
#include "BluetoothLEDeviceDisplay.g.cpp"

namespace winrt::Golf_Chip_WinRT::implementation
{
    winrt::Windows::Devices::Enumeration::DeviceInformation BluetoothLEDeviceDisplay::DeviceInformation()
    {
        throw hresult_not_implemented();
    }
    hstring BluetoothLEDeviceDisplay::Id()
    {
        throw hresult_not_implemented();
    }
    hstring BluetoothLEDeviceDisplay::Name()
    {
        throw hresult_not_implemented();
    }
    bool BluetoothLEDeviceDisplay::IsPaired()
    {
        throw hresult_not_implemented();
    }
    bool BluetoothLEDeviceDisplay::IsConnected()
    {
        throw hresult_not_implemented();
    }
    bool BluetoothLEDeviceDisplay::IsConnectable()
    {
        throw hresult_not_implemented();
    }
    hstring BluetoothLEDeviceDisplay::Address()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::Foundation::Collections::IMapView<hstring, winrt::Windows::Foundation::IInspectable> BluetoothLEDeviceDisplay::Properties()
    {
        throw hresult_not_implemented();
    }
    winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage BluetoothLEDeviceDisplay::GlyphBitmapImage()
    {
        throw hresult_not_implemented();
    }
    void BluetoothLEDeviceDisplay::Update(winrt::Windows::Devices::Enumeration::DeviceInformationUpdate const& deviceInfoUpdate)
    {
        throw hresult_not_implemented();
    }
    winrt::event_token BluetoothLEDeviceDisplay::PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        throw hresult_not_implemented();
    }
    void BluetoothLEDeviceDisplay::PropertyChanged(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
}