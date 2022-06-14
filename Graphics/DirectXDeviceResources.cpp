#include "pch.h"
#include "DirectXDeviceResources.h"
#include <windows.ui.xaml.media.dxinterop.h>
#include "DirectXSample.h"

//Note* the guide here (https://github.com/microsoft/Windows-universal-samples/blob/main/Samples/Simple3DGameXaml/cpp/Common/DeviceResources.cpp)
//has another header file called "DirectXSample.h", not sure if I actually need this or not

using namespace winrt;
using namespace D2D1;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
//using namespace Platform; //do I actually need this one?

//Constants used to calculate screen rotations
namespace ScreenRotation
{
    // 0-degree Z-rotation
    static const XMFLOAT4X4 Rotation0(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 90-degree Z-rotation
    static const XMFLOAT4X4 Rotation90(
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 180-degree Z-rotation
    static const XMFLOAT4X4 Rotation180(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 270-degree Z-rotation
    static const XMFLOAT4X4 Rotation270(
        0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
};

//Constructor
DX::DirectXDeviceResources::DirectXDeviceResources() :
    m_screenViewport(),
    m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
    m_d3dRenderTargetSize(),
    m_outputSize(),
    m_logicalSize(),
    m_nativeOrientation(DisplayOrientations::None),
    m_currentOrientation(DisplayOrientations::None),
    m_dpi(-1.0f),
    m_compositionScaleX(1.0f),
    m_compositionScaleY(1.0f),
    m_deviceNotify(nullptr)
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

//Configure resources that aren't dependent on the Direct3D device
void DX::DirectXDeviceResources::CreateDeviceIndependentResources()
{
    //Initialze Direct2D resources
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    //if the project is in a debug build, enable Direct2D debugging via SDK layers
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    //Had issues with the below functions, found some helpful info here: https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/consume-com
    //Initialize the Direct2D Factory
    winrt::check_hresult(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory1),
            &options,
            m_d2dFactory.put_void()
        )
    );

    //Initialize the DirectWrite Factory
    winrt::check_hresult(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<::IUnknown**>(m_dwriteFactory.put())
        )
    );

    //Initialize the Windows Imaging COmponent (WIC) Factory
    winrt::check_hresult(
        CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(m_wicFactory.put())
        )
    );
    
}