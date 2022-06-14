#pragma once

#include "DirectXPage.g.h"

namespace winrt::Golf_Chip_WinRT::implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage>
    {
        DirectXPage() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::Golf_Chip_WinRT::factory_implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage, implementation::DirectXPage>
    {
    };
}
