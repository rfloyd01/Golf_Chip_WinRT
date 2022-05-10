#include "pch.h"
#include "GolfChipSettings.h"
#include "GolfChipSettings.g.cpp"
#include "BluetoothLEDeviceDisplay.h"

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation::Peers;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

namespace winrt
{
    hstring to_hstring(DevicePairingResultStatus status)
    {
        switch (status)
        {
        case DevicePairingResultStatus::Paired: return L"Paired";
        case DevicePairingResultStatus::NotReadyToPair: return L"NotReadyToPair";
        case DevicePairingResultStatus::NotPaired: return L"NotPaired";
        case DevicePairingResultStatus::AlreadyPaired: return L"AlreadyPaired";
        case DevicePairingResultStatus::ConnectionRejected: return L"ConnectionRejected";
        case DevicePairingResultStatus::TooManyConnections: return L"TooManyConnections";
        case DevicePairingResultStatus::HardwareFailure: return L"HardwareFailure";
        case DevicePairingResultStatus::AuthenticationTimeout: return L"AuthenticationTimeout";
        case DevicePairingResultStatus::AuthenticationNotAllowed: return L"AuthenticationNotAllowed";
        case DevicePairingResultStatus::AuthenticationFailure: return L"AuthenticationFailure";
        case DevicePairingResultStatus::NoSupportedProfiles: return L"NoSupportedProfiles";
        case DevicePairingResultStatus::ProtectionLevelCouldNotBeMet: return L"ProtectionLevelCouldNotBeMet";
        case DevicePairingResultStatus::AccessDenied: return L"AccessDenied";
        case DevicePairingResultStatus::InvalidCeremonyData: return L"InvalidCeremonyData";
        case DevicePairingResultStatus::PairingCanceled: return L"PairingCanceled";
        case DevicePairingResultStatus::OperationAlreadyInProgress: return L"OperationAlreadyInProgress";
        case DevicePairingResultStatus::RequiredHandlerNotRegistered: return L"RequiredHandlerNotRegistered";
        case DevicePairingResultStatus::RejectedByHandler: return L"RejectedByHandler";
        case DevicePairingResultStatus::RemoteDeviceHasAssociation: return L"RemoteDeviceHasAssociation";
        case DevicePairingResultStatus::Failed: return L"Failed";
        }
        return L"Code " + to_hstring(static_cast<int>(status));
    }
}

namespace winrt::Golf_Chip_WinRT::implementation
{
#pragma region UI Code
    void GolfChipSettings::mainPageNavigate(IInspectable const&, RoutedEventArgs const&)
    {
        //Navigate back to the main page
        this->Frame().Navigate(xaml_typename<MainPage>());
    }

    void GolfChipSettings::OnNavigatedFrom(NavigationEventArgs const&)
    {
        //When leaving the page, stop the device watcher if it's currently running
        StopBleDeviceWatcher();
    }

    void GolfChipSettings::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        //When we navigate to the page, check to see if we're currently connected to
        //a BLE device and then display the approriate view.
        /*if (winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->getConnectedCharacteristic() != nullptr) DisplaySettingsMode();
        else DisplaySearchMode();*/

        DisplaySearchMode(); //TODO: Update this when more of the page is filled out
    }

    void GolfChipSettings::EnumerateButton_Click()
    {
        if (deviceWatcher == nullptr)
        {
            StartBleDeviceWatcher();
            EnumerateButton().Content(box_value(L"Stop enumerating"));
            NotifyUser(L"Device watcher started.", NotifyType::StatusMessage);
        }
        else
        {
            StopBleDeviceWatcher();
            EnumerateButton().Content(box_value(L"Start enumerating"));
            NotifyUser(L"Device watcher stopped.", NotifyType::StatusMessage);
        }
    }

    void GolfChipSettings::DisconnectButton_Click()
    {
        //TODO: Add more stuff later
        
        //After disonnecting to the device, change the visuals back to "search mode"
        DisplaySearchMode();
    }

    void GolfChipSettings::NotifyUser(hstring const& strMessage, winrt::Golf_Chip_WinRT::NotifyType const& type)
    {
        if (Dispatcher().HasThreadAccess())
        {
            UpdateStatus(strMessage, type);
        }
        else
        {
            Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [strMessage, type, this]()
                {
                    UpdateStatus(strMessage, type);
                });
        }
    }

    void GolfChipSettings::UpdateStatus(const hstring& strMessage, NotifyType type)
    {
        switch (type)
        {
        case NotifyType::StatusMessage:
            StatusBorder().Background(SolidColorBrush(Windows::UI::Colors::Green()));
            break;
        case NotifyType::ErrorMessage:
            StatusBorder().Background(SolidColorBrush(Windows::UI::Colors::Red()));
            break;
        default:
            break;
        }

        StatusBlock().Text(strMessage);

        // Collapse the StatusBlock if it has no text to conserve real estate.
        if (!strMessage.empty())
        {
            StatusBorder().Visibility(Windows::UI::Xaml::Visibility::Visible);
            StatusPanel().Visibility(Windows::UI::Xaml::Visibility::Visible);
        }
        else
        {
            StatusBorder().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
            StatusPanel().Visibility(Windows::UI::Xaml::Visibility::Collapsed);
        }

        // Raise an event if necessary to enable a screen reader to announce the status update.
        auto peer = FrameworkElementAutomationPeer::FromElement(StatusBlock()).as<FrameworkElementAutomationPeer>();
        if (peer != nullptr)
        {
            peer.RaiseAutomationEvent(AutomationEvents::LiveRegionChanged);
        }
    }

    void GolfChipSettings::DisplaySearchMode()
    {
        //When we aren't currently connected to a BLE device then this page should
        //show all of the buttons and lists necessary for us to connect to a device
        DisconnectButton().Visibility(Visibility::Collapsed);
        SensorSettings().Visibility(Visibility::Collapsed);

        ConnectButton().Visibility(Visibility::Visible);
        BLEScanner().Visibility(Visibility::Visible);
        EnumerateButton().Visibility(Visibility::Visible);

    }

    void GolfChipSettings::DisplaySettingsMode()
    {
        //When we're connected to a BLE device then this page should
        //show all of the settings associated with the current device
        DisconnectButton().Visibility(Visibility::Visible);
        SensorSettings().Visibility(Visibility::Visible);

        ConnectButton().Visibility(Visibility::Collapsed);
        BLEScanner().Visibility(Visibility::Collapsed);
        EnumerateButton().Visibility(Visibility::Collapsed);

        //Populate the settings arrays
        //PopulateSettingsVectors();
    }
#pragma endregion

#pragma region Device Discovery
    void GolfChipSettings::StartBleDeviceWatcher()
    {
        // Additional properties we would like about the device.
        // Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
        auto requestedProperties = single_threaded_vector<hstring>({ L"System.Devices.Aep.DeviceAddress", L"System.Devices.Aep.IsConnected", L"System.Devices.Aep.Bluetooth.Le.IsConnectable" });

        // BT_Code: Example showing paired and non-paired in a single query.
        hstring aqsAllBluetoothLEDevices = L"(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

        deviceWatcher =
            Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(
                aqsAllBluetoothLEDevices,
                requestedProperties,
                DeviceInformationKind::AssociationEndpoint);

        //Register event handlers before starting the watcher.
        deviceWatcherAddedToken = deviceWatcher.Added({ get_weak(), &GolfChipSettings::DeviceWatcher_Added });
        deviceWatcherUpdatedToken = deviceWatcher.Updated({ get_weak(), &GolfChipSettings::DeviceWatcher_Updated });
        deviceWatcherRemovedToken = deviceWatcher.Removed({ get_weak(), &GolfChipSettings::DeviceWatcher_Removed });
        deviceWatcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ get_weak(), &GolfChipSettings::DeviceWatcher_EnumerationCompleted });
        deviceWatcherStoppedToken = deviceWatcher.Stopped({ get_weak(), &GolfChipSettings::DeviceWatcher_Stopped });

        //Start over with an empty collection.
        m_knownDevices.Clear();

        //Start the watcher. Active enumeration is limited to approximately 30 seconds.
        //This limits power usage and reduces interference with other Bluetooth activities.
        //To monitor for the presence of Bluetooth LE devices for an extended period,
        //use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
        //sample for an example.
        deviceWatcher.Start();
    }

    void GolfChipSettings::StopBleDeviceWatcher()
    {
        if (deviceWatcher != nullptr)
        {
            //Unregister the event handlers
            deviceWatcher.Added(deviceWatcherAddedToken);
            deviceWatcher.Updated(deviceWatcherUpdatedToken);
            deviceWatcher.Removed(deviceWatcherRemovedToken);
            deviceWatcher.EnumerationCompleted(deviceWatcherEnumerationCompletedToken);
            deviceWatcher.Stopped(deviceWatcherStoppedToken);

            //Stop the watcher
            deviceWatcher.Stop();
            deviceWatcher = nullptr;
        }
    }

    std::tuple<Golf_Chip_WinRT::BluetoothLEDeviceDisplay, uint32_t> GolfChipSettings::FindBluetoothLeDeviceDisplay(hstring const& id)
    {
        uint32_t size = m_knownDevices.Size();
        for (uint32_t index = 0; index < size; index++)
        {
            auto bleDeviceDisplay = m_knownDevices.GetAt(index).as<Golf_Chip_WinRT::BluetoothLEDeviceDisplay>();
            if (bleDeviceDisplay.Id() == id)
            {
                return { bleDeviceDisplay, index };
            }
        }
        
        return { nullptr, 0 - 1U };
    }

    std::vector<Windows::Devices::Enumeration::DeviceInformation>::iterator GolfChipSettings::FindUnknownDevices(hstring const& id)
    {
        return std::find_if(UnknownDevices.begin(), UnknownDevices.end(), [&](auto&& bleDeviceInfo)
            {
                return bleDeviceInfo.Id() == id;
            }
        );
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Added(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformation deviceInfo)
    {
        ////We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        //Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            //Make sure device isn't already present in the list.
            if (std::get<0>(FindBluetoothLeDeviceDisplay(deviceInfo.Id())) == nullptr)
            {
                if (!deviceInfo.Name().empty())
                {
                    //If device has a friendly name display it immediately.
                    m_knownDevices.Append(make<BluetoothLEDeviceDisplay>(deviceInfo));

                    //Create an hstring representing the properties set forth in the creation of the device watcher
                    std::string deviceProperties = "";

                    OutputDebugStringW((L"Added " + deviceInfo.Id() + L" " + deviceInfo.Name() + L"\n").c_str());
                }
                else
                {
                    //Add it to a list in case the name gets updated later.
                    UnknownDevices.push_back(deviceInfo);
                }
            }
        }
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Updated(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate)
    {
        ////We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        ////Protect against race condition if the task runs after the app stopped the deviceWatcher.
        //if (sender == deviceWatcher)
        //{
        //    Golf_Chip_WinRT::BluetoothLEDeviceDisplay bleDeviceDisplay = std::get<0>(FindBluetoothLeDeviceDisplay(deviceInfoUpdate.Id()));
        //    if (bleDeviceDisplay != nullptr)
        //    {
        //        //Device is already being displayed - update UX.
        //        bleDeviceDisplay.Update(deviceInfoUpdate);
        //        co_return;
        //    }

        //    auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
        //    if (deviceInfo != UnknownDevices.end())
        //    {
        //        deviceInfo->Update(deviceInfoUpdate);
        //        //If device has been updated with a friendly name it's no longer unknown.
        //        if (!deviceInfo->Name().empty())
        //        {
        //            m_knownDevices.Append(make<BluetoothLEDeviceDisplay>(*deviceInfo));
        //            UnknownDevices.erase(deviceInfo);
        //            OutputDebugStringW((L"Added " + deviceInfoUpdate.Id() + L"\n").c_str());
        //        }
        //    }
        //}
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Removed(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate)
    {
        //// We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        //// Protect against race condition if the task runs after the app stopped the deviceWatcher.
        //if (sender == deviceWatcher)
        //{
        //    //Find the corresponding DeviceInformation in the collection and remove it.
        //    auto [bleDeviceDisplay, index] = FindBluetoothLeDeviceDisplay(deviceInfoUpdate.Id());
        //    if (bleDeviceDisplay != nullptr)
        //    {
        //        m_knownDevices.RemoveAt(index);
        //        OutputDebugStringW((L"Removed " + deviceInfoUpdate.Id() + L" " + bleDeviceDisplay.Name() + L"\n").c_str());
        //    }

        //    auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
        //    if (deviceInfo != UnknownDevices.end())
        //    {
        //        UnknownDevices.erase(deviceInfo);
        //    }
        //}
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&)
    {
        //// Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        //// Protect against race condition if the task runs after the app stopped the deviceWatcher.
        //if (sender == deviceWatcher)
        //{
        //    /*rootPage.NotifyUser(to_hstring(m_knownDevices.Size()) + L" devices found. Enumeration completed.",
        //        NotifyType::StatusMessage);*/
        //}
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&)
    {
        //// Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        //// Protect against race condition if the task runs after the app stopped the deviceWatcher.
        //if (sender == deviceWatcher)
        //{
        //    /*rootPage.NotifyUser(L"No longer watching for devices.",
        //        sender.Status() == DeviceWatcherStatus::Aborted ? NotifyType::ErrorMessage : NotifyType::StatusMessage);*/
        //}
    }
#pragma endregion

#pragma region Device Connection
    void GolfChipSettings::ConnectButton_Click()
    {
        //auto bleDeviceDisplay = ResultsListView().SelectedItem().as<Golf_Chip_WinRT::BluetoothLEDeviceDisplay>();

        ////TODO: There's a method in the BluetoothLEDeviceDisplay class called Address() that should just return the address,
        ////but for somereason the function can't be seen from here? It's a public function so I'm not sure about the reason for this.
        //hstring deviceAddress = unbox_value<hstring>(bleDeviceDisplay.Properties().TryLookup(L"System.Devices.Aep.DeviceAddress"));

        ////The address for the BLE device needs to be properly converted from a string into a uint64_t
        //uint64_t formattedAddress = getBLEAddress(deviceAddress);

        //BluetoothLEDevice nano33BLE = co_await BluetoothLEDevice::FromBluetoothAddressAsync(formattedAddress);

        //if (nano33BLE != nullptr)
        //{
        //    //The device has been found, to create the physical connection we subscribe to the device's GattService
        //    GenericAttributeProfile::GattDeviceServicesResult servicesResult = co_await nano33BLE.GetGattServicesAsync(BluetoothCacheMode::Uncached);

        //    //TODO: uncomment below block when ready to connect to device
        //    //if (servicesResult.Status() == GenericAttributeProfile::GattCommunicationStatus::Success)
        //    //{
        //    //    //TODO: I happen to just know the desired service and characteristic, in the future I should have some better
        //    //    //way to verify this
        //    //    auto desiredServiceUUID = BluetoothUuidHelper::FromShortId(0x180C);
        //    //    int serviceLocation = 0;
        //    //    for (int i = 0; i < servicesResult.Services().Size(); i++)
        //    //    {
        //    //        uint32_t s = servicesResult.Services().GetAt(i).Uuid().Data1;
        //    //        if (s == desiredServiceUUID.Data1) serviceLocation = i;
        //    //    }

        //    //    GenericAttributeProfile::GattDeviceService service = servicesResult.Services().GetAt(serviceLocation);

        //    //    //Grab the characteristics from the GettService
        //    //    GenericAttributeProfile::GattCharacteristicsResult characteristicResult = co_await service.GetCharacteristicsAsync();

        //    //    uint32_t desiredCharacteristic = 0x00002A58;
        //    //    int characteristicLocation = 0, testCharacteristicLocation = 0;
        //    //    for (int i = 0; i < characteristicResult.Characteristics().Size(); i++)
        //    //    {
        //    //        uint32_t c = characteristicResult.Characteristics().GetAt(i).Uuid().Data1;
        //    //        if (c == Constants::GolfChipSensorDataCharacteristicUuid) characteristicLocation = i;
        //    //        else if (c == Constants::GolfChipSensorTestCharacteristicUuid) testCharacteristicLocation = i;
        //    //    }

        //    //    //Connect to the data characteristic
        //    //    if (winrt::Golf_Chip::SampleState::m_golfChip != nullptr)
        //    //    {
        //    //        if (winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice() != nullptr)
        //    //        {
        //    //            //Save the characteristic info in the shared section of the code
        //    //            winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->setConnectedCharacteristic(characteristicResult.Characteristics().GetAt(characteristicLocation));
        //    //            NotifyUser(L"Succesfully connected to the device! Chatacteristic UUID is " + winrt::to_hstring(winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->getConnectedCharacteristic().Uuid()), NotifyType::StatusMessage);

        //    //            //We need to configure the characteristic to notify us when it changes (and considering the sensor will be constantly reading data, this is a lot)
        //    //            SetupCharacteristicNotification(winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->getConnectedCharacteristic());

        //    //            //Connect to the test characteristic as well
        //    //            winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->setTestCharacteristic(characteristicResult.Characteristics().GetAt(testCharacteristicLocation));

        //    //            //After setting the connectionCharacteristic, get sensor data from the Golf Chip to set up the IMU object
        //    //            auto read = co_await winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->getTestCharacteristic().ReadValueAsync();

        //    //            //winrt::Golf_Chip::SampleState::m_golfChip->getBLEDevice()->setupIMUSensor("LSM9DS1_ACC", read.Value().data());

        //    //            //After onnecting to the device, set the display to "settings mode" and stop the device watcher from enumerating.
        //    //            DisplaySettingsMode();
        //    //            StopBleDeviceWatcher();
        //    //        }
        //    //        else NotifyUser(L"Succesfully connected to the device! But the BLEDevice was nullptr...", NotifyType::ErrorMessage);
        //    //    }
        //    //    else NotifyUser(L"Succesfully connected to the device! But the m_golfChip was nullptr...", NotifyType::ErrorMessage);

        //    //}
        //    //else
        //    //{
        //    //    NotifyUser(L"The connection to the device was refused", NotifyType::StatusMessage);
        //    //}
        //}
        //else
        //    NotifyUser(L"Couldn't connect to the BLE device with address " + winrt::to_hstring(formattedAddress), NotifyType::ErrorMessage);
        ////The Arduino Nano BLE 33 doesn't have the capability to pair, so we attempt to connect to it directly

    }
#pragma endregion

    uint64_t GolfChipSettings::getBLEAddress(const hstring& unformattedAddress)
    {
        //The addresses for the found BLE devices are in the form ##:##:##:##:##:##. To convert this style into a useable
        //64-bit number the following formula is used
        //uint64_t formattedAddress = 0;

        ////TODO: I'm not sure if the letters in the address will always be lowercase or not, may need to look into this
        ////in the future
        //for (wchar_t c : unformattedAddress)
        //{
        //    if (c == ':') continue;
        //    formattedAddress <<= 4;

        //    if (c >= 'a') formattedAddress += (c - 'a' + 10);
        //    else formattedAddress += (c - '0');
        //}

        //return formattedAddress;
        return 0;
    }

}
