﻿#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Golf_Chip_WinRT::implementation
{
    void MainPage::deviceSettingsMode_Click(Windows::Foundation::IInspectable const& /*sender*/, Windows::UI::Xaml::RoutedEventArgs const& /*args*/ )
    {
        deviceSettingsMode().Content(box_value(L"Clicked"));
    }
    void MainPage::freeSwingMode_Click(Windows::Foundation::IInspectable const& /*sender*/, Windows::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        freeSwingMode().Content(box_value(L"Clicked"));
    }
    void MainPage::swingAnalysisMode_Click(Windows::Foundation::IInspectable const& /*sender*/, Windows::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        swingAnalysisMode().Content(box_value(L"Clicked"));
    }
    void MainPage::trainingMode_Click(Windows::Foundation::IInspectable const& /*sender*/, Windows::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        trainingMode().Content(box_value(L"Clicked"));
    }
    void MainPage::calibrationMode_Click(Windows::Foundation::IInspectable const& /*sender*/, Windows::UI::Xaml::RoutedEventArgs const& /*args*/)
    {
        calibrationMode().Content(box_value(L"Clicked"));
    }
}
