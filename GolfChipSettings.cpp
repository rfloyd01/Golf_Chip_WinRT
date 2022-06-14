#include "pch.h"
#include "GolfChipSettings.h"
#include "GolfChipSettings.g.cpp"
#include "BluetoothLEDeviceDisplay.h"
#include "GolfChipSettingsDisplay.h"
#include "GolfChip_Files/GolfChip.h"
#include "GolfChip_Files/Sensors/Sensor.h"
#include "Graphics/DirectXDeviceResources.h"

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
        
        if (GlobalGolfChip::m_golfChip->getBLEDevice() != nullptr)
        {
            //create the display sensors from the existing connected sensors
            /*auto currentAcc = GlobalGolfChip::m_golfChip->getIMU()->getSensor(SensorType::ACCELEROMETER);
            m_display_sensors[SensorType::ACCELEROMETER] = Sensor::SensorFactory(currentAcc->getName(), );
            auto gyrSettings = GlobalGolfChip::m_golfChip->getIMU()->getSensorSettings(SensorType::GYROSCOPE);
            auto magSettings = GlobalGolfChip::m_golfChip->getIMU()->getSensorSettings(SensorType::MAGNETOMETER);*/
            DisplaySettingsMode();
        }
        else
        {
            DisplaySearchMode();
        }
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
        UpdateSettingsButton().Visibility(Visibility::Collapsed);

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
        UpdateSettingsButton().Visibility(Visibility::Visible);

        ConnectButton().Visibility(Visibility::Collapsed);
        BLEScanner().Visibility(Visibility::Collapsed);
        EnumerateButton().Visibility(Visibility::Collapsed);

        //Populate the settings arrays
        SetSettingVectors();
    }

    void GolfChipSettings::SetSettingVectors()
    {
        //Accelerometer first
        auto accSettings = m_display_sensors[SensorType::ACCELEROMETER]->getSensorSettings();
        auto gyrSettings = m_display_sensors[SensorType::GYROSCOPE]->getSensorSettings();
        auto magSettings = m_display_sensors[SensorType::MAGNETOMETER]->getSensorSettings();

        m_accName = winrt::to_hstring(m_display_sensors[SensorType::ACCELEROMETER]->getName());
        m_gyrName = winrt::to_hstring(m_display_sensors[SensorType::GYROSCOPE]->getName());
        m_magName = winrt::to_hstring(m_display_sensors[SensorType::MAGNETOMETER]->getName());
        
        AccName().Text(m_accName);
        GyrName().Text(m_gyrName);
        MagName().Text(m_magName);

        for (int i = 0; i < accSettings.size(); i++)  m_accelerometerSettings.Append(make<GolfChipSettingsDisplay>(accSettings[i]));
        for (int i = 0; i < gyrSettings.size(); i++)  m_gyroscopeSettings.Append(make<GolfChipSettingsDisplay>(gyrSettings[i]));
        for (int i = 0; i < magSettings.size(); i++)  m_magnetometerSettings.Append(make<GolfChipSettingsDisplay>(magSettings[i]));

        amountOfSettings = accSettings.size() + gyrSettings.size() + magSettings.size();
    }

    void GolfChipSettings::UpdateSettingsButton_Click()
    {
        std::string currentSettings = "";
        auto rawDataYo = m_display_sensors[0]->getRawSettings();
        for (int i = 0; i < 18; i++) currentSettings += std::to_string(*(rawDataYo + i)) + " ";

        NotifyUser(winrt::to_hstring("Current settings array is: " + currentSettings) , NotifyType::StatusMessage);
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
        //We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        //Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            Golf_Chip_WinRT::BluetoothLEDeviceDisplay bleDeviceDisplay = std::get<0>(FindBluetoothLeDeviceDisplay(deviceInfoUpdate.Id()));
            if (bleDeviceDisplay != nullptr)
            {
                //Device is already being displayed - update UX.
                bleDeviceDisplay.Update(deviceInfoUpdate);
                co_return;
            }

            auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
            if (deviceInfo != UnknownDevices.end())
            {
                deviceInfo->Update(deviceInfoUpdate);
                //If device has been updated with a friendly name it's no longer unknown.
                if (!deviceInfo->Name().empty())
                {
                    m_knownDevices.Append(make<BluetoothLEDeviceDisplay>(*deviceInfo));
                    UnknownDevices.erase(deviceInfo);
                    OutputDebugStringW((L"Added " + deviceInfoUpdate.Id() + L"\n").c_str());
                }
            }
        }
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Removed(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate)
    {
        // We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            //Find the corresponding DeviceInformation in the collection and remove it.
            auto [bleDeviceDisplay, index] = FindBluetoothLeDeviceDisplay(deviceInfoUpdate.Id());
            if (bleDeviceDisplay != nullptr)
            {
                m_knownDevices.RemoveAt(index);
                OutputDebugStringW((L"Removed " + deviceInfoUpdate.Id() + L" " + bleDeviceDisplay.Name() + L"\n").c_str());
            }

            auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
            if (deviceInfo != UnknownDevices.end())
            {
                UnknownDevices.erase(deviceInfo);
            }
        }
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&)
    {
        // Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            NotifyUser(to_hstring(m_knownDevices.Size()) + L" devices found. Enumeration completed.",
                NotifyType::StatusMessage);
        }
    }

    fire_and_forget GolfChipSettings::DeviceWatcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&)
    {
        // Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            NotifyUser(L"No longer watching for devices.",
                sender.Status() == DeviceWatcherStatus::Aborted ? NotifyType::ErrorMessage : NotifyType::StatusMessage);
        }
    }
#pragma endregion

#pragma region Device Connection
    fire_and_forget GolfChipSettings::ConnectButton_Click()
    {
        auto bleDeviceDisplay = ResultsListView().SelectedItem().as<Golf_Chip_WinRT::BluetoothLEDeviceDisplay>();

        //TODO: There's a method in the BluetoothLEDeviceDisplay class called Address() that should just return the address,
        //but for somereason the function can't be seen from here? It's a public function so I'm not sure about the reason for this.
        hstring deviceAddress = unbox_value<hstring>(bleDeviceDisplay.Properties().TryLookup(L"System.Devices.Aep.DeviceAddress"));

        //The address for the BLE device needs to be properly converted from a string into a uint64_t
        uint64_t formattedAddress = getBLEAddress(deviceAddress);

        BluetoothLEDevice golfChip = co_await BluetoothLEDevice::FromBluetoothAddressAsync(formattedAddress);

        if (golfChip != nullptr)
        {
            //Set the BLEDevice on the golf chip in order to maintain the connection
            GlobalGolfChip::m_golfChip->setBLEDevice(golfChip);

            //The next thing to do is to read the IMU information characteristics on the device so we can figure
            //out which sensors are currently attached, as well as their current settings. This information is
            //contained in the Golf Chip Information Characteristic
            GenericAttributeProfile::GattDeviceServicesResult servicesResult = co_await GlobalGolfChip::m_golfChip->getBLEDevice()->GetGattServicesAsync(BluetoothCacheMode::Uncached);

            //Before reading the characteristic we need to locate the Service that it's in. The Service info
            //(as well as the Characteristic info) is saved with the global golf chip constants.
            if (servicesResult.Status() == GenericAttributeProfile::GattCommunicationStatus::Success)
            {
                int serviceLocation = 0;
                for (int i = 0; i < servicesResult.Services().Size(); i++)
                {
                    uint32_t s = servicesResult.Services().GetAt(i).Uuid().Data1;
                    if (s == Constants::GolfChipSensorDataServiceUuid.Data1) serviceLocation = i;
                }

                GenericAttributeProfile::GattDeviceService service = servicesResult.Services().GetAt(serviceLocation);

                //Grab a list of all the available Characteristics from the Golf Chip GattService
                GenericAttributeProfile::GattCharacteristicsResult characteristicResult = co_await service.GetCharacteristicsAsync();

                //Cycle through the Characteristics until we find the information one.
                int AccelerometerSettingCharacteristicLocation = 0, GyroscopeSettingCharacteristicLocation = 0, MagnetometerSettingCharacteristicLocation = 0;
                for (int i = 0; i < characteristicResult.Characteristics().Size(); i++)
                {
                    uint32_t c = characteristicResult.Characteristics().GetAt(i).Uuid().Data1;
                    if (c == Constants::GolfChipAccelerometerSettingsCharacteristicUuid) AccelerometerSettingCharacteristicLocation = i;
                    else if (c == Constants::GolfChipGyroscopeSettingsCharacteristicUuid) GyroscopeSettingCharacteristicLocation = i;
                    else if (c == Constants::GolfChipMagnetometerSettingsCharacteristicUuid) MagnetometerSettingCharacteristicLocation = i;
                }

                //Save the characteristic info in the shared section of the code
                GlobalGolfChip::m_golfChip->setInformationCharacteristic(characteristicResult.Characteristics().GetAt(AccelerometerSettingCharacteristicLocation));

                NotifyUser(L"Succesfully connected to the device!", NotifyType::StatusMessage);

                //After setting the connectionCharacteristic, get sensor data from the Golf Chip to set up the IMU object,
                //as well as the SensorSettings arrays on this page.

                //TODO: Should put some confirmation that the characteristics are actually found before trying to read them
                //TODO: Probably don't need to save the acc characteristics anymore
                auto accCharacteristicValue = co_await GlobalGolfChip::m_golfChip->getInformationCharacteristic().ReadValueAsync();
                auto gyroCharacteristicValue = co_await characteristicResult.Characteristics().GetAt(GyroscopeSettingCharacteristicLocation).ReadValueAsync();
                auto magCharacteristicValue = co_await characteristicResult.Characteristics().GetAt(MagnetometerSettingCharacteristicLocation).ReadValueAsync();
                
                //Create the global instances for each sensor on the IMU, then create copies of these which are used to
                //display setting information on this page.

                //TODO: The names should be coming from the characteristics themselves, not hard coded in here.
                GlobalGolfChip::m_golfChip->getIMU()->setSensor("LSM9DS1_ACC", Windows::Storage::Streams::DataReader::FromBuffer(accCharacteristicValue.Value()));
                GlobalGolfChip::m_golfChip->getIMU()->setSensor("LSM9DS1_GYR", Windows::Storage::Streams::DataReader::FromBuffer(gyroCharacteristicValue.Value()));
                GlobalGolfChip::m_golfChip->getIMU()->setSensor("LSM9DS1_MAG", Windows::Storage::Streams::DataReader::FromBuffer(magCharacteristicValue.Value()));

                m_display_sensors[SensorType::ACCELEROMETER] = Sensor::SensorFactory("LSM9DS1_ACC", Windows::Storage::Streams::DataReader::FromBuffer(accCharacteristicValue.Value()));
                m_display_sensors[SensorType::GYROSCOPE] = Sensor::SensorFactory("LSM9DS1_GYR", Windows::Storage::Streams::DataReader::FromBuffer(gyroCharacteristicValue.Value()));
                m_display_sensors[SensorType::MAGNETOMETER] = Sensor::SensorFactory("LSM9DS1_MAG", Windows::Storage::Streams::DataReader::FromBuffer(magCharacteristicValue.Value()));

                //After onnecting to the device, set the display to "settings mode" and stop the device watcher from enumerating.
                DisplaySettingsMode();
                StopBleDeviceWatcher();
            }
            else
            {
                NotifyUser(L"The connection to the device was refused", NotifyType::StatusMessage);
            }

             
        }
        else
            NotifyUser(L"Couldn't connect to the BLE device with address " + winrt::to_hstring(formattedAddress), NotifyType::ErrorMessage);
    }

    void GolfChipSettings::DisconnectButton_Click()
    {
        if (GlobalGolfChip::m_golfChip->getBLEDevice() != nullptr) GlobalGolfChip::m_golfChip->Disconnect();

        //After disonnecting to the device, change the visuals back to "search mode"
        DisplaySearchMode();
    }
#pragma endregion

    uint64_t GolfChipSettings::getBLEAddress(const hstring& unformattedAddress)
    {
        //The addresses for the found BLE devices are in the form ##:##:##:##:##:##. To convert this style into a useable
        //64-bit number the following formula is used
        uint64_t formattedAddress = 0;

        //TODO: I'm not sure if the letters in the address will always be lowercase or not, may need to look into this
        //in the future
        for (wchar_t c : unformattedAddress)
        {
            if (c == ':') continue;
            formattedAddress <<= 4;

            if (c >= 'a') formattedAddress += (c - 'a' + 10);
            else formattedAddress += (c - '0');
        }

        return formattedAddress;
    }

}


void winrt::Golf_Chip_WinRT::implementation::GolfChipSettings::SensorOption_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    auto yo = unbox_value<Golf_Chip_WinRT::GolfChipSettingsDisplay>(sender.as<winrt::Windows::UI::Xaml::Controls::ComboBox>().DataContext()); //Debugging line to figure out what is calling this function
    SensorSettingType heeeeooo;
    if (yo != nullptr) heeeeooo = static_cast<SensorSettingType>(yo.GetUnderlyingSetting());
    if (settingsLoaded == amountOfSettings)
    {
        //Since only OneWay binding is enabled, we need to manually updated the appropriate setting array.
        
        //auto yo = sender.as<winrt::Windows::UI::Xaml::Controls::ComboBox>().DataContext(); //This line gets the bound item in its current state
        //auto yeet = unbox_value<Golf_Chip_WinRT::GolfChipSettingsDisplay>(yo).SelectedOption();
        auto newOptionIndex = sender.as<winrt::Windows::UI::Xaml::Controls::ComboBox>().SelectedIndex();
        hstring initialOption = unbox_value<hstring>(e.RemovedItems().GetAt(0));
        //OutputDebugStringW((L"Removed the following setting: " + initialOption).c_str());
        bool optionFound = false;

        //Now that we have the new setting, cycle through all the current setting arrays until we find the one that was updated
        //TODO: I'm positive there's a better way to do this, but for now I can't seem to get Two Way binding working the way I want
        for (int i = 0; i < m_accelerometerSettings.Size(); i++)
        {
            //auto setting = m_accelerometerSettings.GetAt(i).as<GolfChipSettingsDisplay>().get();
            auto setting = m_accelerometerSettings.GetAt(i).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
            if (setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>() == initialOption)
            {
                setting.SelectedOption(newOptionIndex);
                optionFound = true;
                SensorSettingType selectedSetting = static_cast<SensorSettingType>(setting.GetUnderlyingSetting());

                //Need to check and see whether or not selecting the new option will have a cascading effect
                //and cause other options to change (i.e. going from high pass filter off mode to high pass
                //filter on mode will cause the HPF freq options to change)
                if (m_display_sensors[SensorType::ACCELEROMETER]->optionCascade(selectedSetting))
                {
                    //create a brand new sensor object with the new settings and update the appropriate settings vector.
                    //First, get the current settings
                    uint8_t* currentRawSettings = m_display_sensors[SensorType::ACCELEROMETER]->getRawSettings();
                    uint8_t crs[18] = { 0 };

                    for (int i = 0; i < 18; i++) crs[i] = *(currentRawSettings + i);

                    //update the appropriate setting that was changed
                    SensorSettingOptions newOption = static_cast<SensorSettingOptions>(setting.GetUnderlyingOption(newOptionIndex));
                    crs[m_display_sensors[SensorType::ACCELEROMETER]->getRawSettingLocation(selectedSetting)] = m_display_sensors[SensorType::ACCELEROMETER]->getByte(newOption);

                    //Create a new sensor from the updated byte array and overwrite the existing one
                    m_display_sensors[SensorType::ACCELEROMETER].reset();
                    m_display_sensors[SensorType::ACCELEROMETER] = Sensor::SensorFactory("LSM9DS1_ACC", crs);

                    //update the display vector
                    m_accelerometerSettings.Clear();
                    auto newAccSettings = m_display_sensors[SensorType::ACCELEROMETER]->getSensorSettings();
                    for (int i = 0; i < newAccSettings.size(); i++)  m_accelerometerSettings.Append(make<GolfChipSettingsDisplay>(newAccSettings[i]));

                    //The m_accelerometerSettings.Clear() method causes the combo box on selection changed event handler to get called for
                    //each setting in the array. The event handler will get called again for each of these settings once it gets re-created.
                    //Because of this we need to subtract 2 * (the settings array size) from the settingsLoaded variable.
                    settingsLoaded -= (newAccSettings.size() * 2);
                }
                else
                {
                    //we need to update the raw settings vector regardless of whether or not the change cascades or not
                    uint8_t* raw_settings = m_display_sensors[SensorType::ACCELEROMETER]->getRawSettings();
                    SensorSettingOptions newOption = static_cast<SensorSettingOptions>(setting.GetUnderlyingOption(newOptionIndex));
                    *(raw_settings + m_display_sensors[SensorType::ACCELEROMETER]->getRawSettingLocation(selectedSetting)) = m_display_sensors[SensorType::ACCELEROMETER]->getByte(newOption);
                }
                
                break;
            }
        }

        if (!optionFound)
        {
            for (int i = 0; i < m_gyroscopeSettings.Size(); i++)
            {
                auto setting = m_gyroscopeSettings.GetAt(i).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
                if (setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>() == initialOption)
                {
                    setting.SelectedOption(newOptionIndex);
                    optionFound = true;
                    break;
                }
            }
        }

        if (!optionFound)
        {
            for (int i = 0; i < m_magnetometerSettings.Size(); i++)
            {
                auto setting = m_magnetometerSettings.GetAt(i).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
                hstring supBaby = setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>();
                //OutputDebugStringW((setting.SettingType() + L": ").c_str());
                //OutputDebugStringW((setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>() + L"\n").c_str());
                if (setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>() == initialOption)
                {
                    setting.SelectedOption(newOptionIndex);
                    optionFound = true;
                    break;
                }
            }
        }
        /*;
        auto comboBoxParent = sender.as<winrt::Windows::UI::Xaml::Controls::ComboBox>().Parent().as<FrameworkElement>();
        auto heehee = comboBoxParent.Parent().as<FrameworkElement>();
        
        OutputDebugStringW((L"The parent of the changed box is: " + heehee.Name() + L"\n").c_str());*/

        //Attempt to cycle through each option to see if there's a difference
       /* auto yo = m_accelerometerSettings.GetAt(2).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
        yo.SelectedOption(3);
        m_accelerometerSettings.SetAt(2, box_value(yo));
        for (int i = 0; i < m_accelerometerSettings.Size(); i++)
        {
            auto setting = m_accelerometerSettings.GetAt(i).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
            OutputDebugStringW((setting.SettingType() + L": ").c_str());
            OutputDebugStringW((setting.AvailableOptions().GetAt(setting.SelectedOption()).as<hstring>() + L"\n").c_str());
        }*/
        //amountOfSettings--;
        //auto yo = m_gyroscopeSettings.GetAt(2).as<Golf_Chip_WinRT::GolfChipSettingsDisplay>();
        ////m_accelerometerSettings.Append(box_value(yo));
        //yo.AvailableOptions().Clear();
        
    }
    else settingsLoaded++;
    
}
