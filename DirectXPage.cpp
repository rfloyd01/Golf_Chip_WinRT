#include "pch.h"
#include "DirectXPage.h"
#if __has_include("DirectXPage.g.cpp")
#include "DirectXPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Golf_Chip_WinRT::implementation
{
    int32_t DirectXPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void DirectXPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void DirectXPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
