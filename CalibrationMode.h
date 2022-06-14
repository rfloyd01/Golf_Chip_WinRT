#pragma once

#include "CalibrationMode.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct CalibrationMode : CalibrationModeT<CalibrationMode>
    {
        CalibrationMode();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void AccelerometerButton_Click();
        void GyroscopeButton_Click() {}
        void MagnetometerButton_Click() {}
        void MainPageButton_Click();
        //void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

        //fire_and_forget animationLoop();
        void swapChainPanel_CompositionScaleChanged(winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& sender, winrt::Windows::Foundation::IInspectable const& args);
    };
}

namespace winrt::Golf_Chip_WinRT::factory_implementation
{
    struct CalibrationMode : CalibrationModeT<CalibrationMode, implementation::CalibrationMode>
    {
    };
}
