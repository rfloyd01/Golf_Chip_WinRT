#include "pch.h"
#include "CalibrationMode.h"
#if __has_include("CalibrationMode.g.cpp")
#include "CalibrationMode.g.cpp"
#endif
#include "Graphics/DirectXDeviceResources.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::Golf_Chip_WinRT::implementation
{
	CalibrationMode::CalibrationMode()
	{
		//swapChainPanel().CompositionScaleChanged({ this, &CalibrationMode::swapChainPanel_CompositionScaleChanged });
		//OutputDebugStringW(L"Calibration mode page has been initialized");
	}

	void CalibrationMode::swapChainPanel_CompositionScaleChanged(winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& sender, winrt::Windows::Foundation::IInspectable const& args)
	{
		GlobalDirectXDeviceResources::m_deviceResources->SetCompositionScale(sender.CompositionScaleX(), sender.CompositionScaleY());
	}

	void CalibrationMode::MainPageButton_Click()
	{
		this->Frame().Navigate(xaml_typename<MainPage>());
	}

	void CalibrationMode::OnNavigatedFrom(NavigationEventArgs const&)
	{
		//When leaving the page, stop the device watcher if it's currently running
		OutputDebugStringW(L"Just left the calibration page\n");
	}

	void CalibrationMode::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		//When we navigate to the page, check to see if we're currently connected to
		//a BLE device and then display the approriate view.
		OutputDebugStringW(L"Just navigated to the calibration page.\n");

		//Need to set up the SwapChainPanel in the DirectXDeviceResources to point to the SwapChainPanel
		//on this Xaml page
		GlobalDirectXDeviceResources::m_deviceResources->SetSwapChainPanel(swapChainPanel());
	}

	/*void CalibrationMode::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e)
	{
		OutputDebugStringW(L"Calibration page has loaded, starting on infinite loop.\n");
	}*/

	//fire_and_forget CalibrationMode::animationLoop()
	//{
	//	//create a timer and try to print something once every second
	//	auto timer = std::chrono::steady_clock::now();
	//	double fpsClock = 0.00;
	//	while (true)
	//	{
	//		fpsClock = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - timer).count() / 1000000000.0;
	//		if (fpsClock >= 1.0)
	//		{
	//			OutputDebugStringW(L"One second has passed.\n");
	//			timer = std::chrono::steady_clock::now();
	//			fpsClock = 0.00;
	//		}
	//	}
	//}

	void CalibrationMode::AccelerometerButton_Click()
	{
		//Create a device for rendering of 3D Graphics
		uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		//Create the Direct3D Device
		winrt::com_ptr<::ID3D11Device> d3dDevice;
		D3D_FEATURE_LEVEL supportedFeatureLevel;
		winrt::check_hresult(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			d3dDevice.put(),
			&supportedFeatureLevel,
			nullptr
		));

		//Get the Direct3D device.
		winrt::com_ptr<::IDXGIDevice> dxgiDevice{ d3dDevice.as<::IDXGIDevice>() };

		//Create Device for Rendering of 2D Graphics (like text)
		//Create the Direct2D device and a corresponding context
		winrt::com_ptr<::ID2D1Device> d2dDevice;
		::D2D1CreateDevice(dxgiDevice.get(), nullptr, d2dDevice.put());

		winrt::com_ptr<::ID2D1DeviceContext> d2dDeviceContext;
		winrt::check_hresult(
			d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				d2dDeviceContext.put()
			)
		);

		//Get the DXGI adapter
		winrt::com_ptr<::IDXGIAdapter> dxgiAdapter;
		dxgiDevice->GetAdapter(dxgiAdapter.put());

		//Get the DXGI factory
		winrt::com_ptr<::IDXGIFactory2> dxgiFactory;
		dxgiFactory.capture(dxgiAdapter, &IDXGIAdapter::GetParent);

		//Define the properties for swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{ 0 };
		swapChainDesc.Width = 500;
		swapChainDesc.Height = 500;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //This is the most common swapcahin format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1; //Don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; //We recommend using the swap effect for all aplications.
		swapChainDesc.Flags = 0;

		//Create the swap chain from the above defined properties
		winrt::com_ptr<::IDXGISwapChain1> swapChain;
		dxgiFactory->CreateSwapChainForComposition(
			d3dDevice.get(),
			&swapChainDesc,
			nullptr,
			swapChain.put()
		);

		//Get the native interface for the swap chain panel from the XAML page
		auto panelNative{ swapChainPanel().as<ISwapChainPanelNative>() };
		winrt::check_hresult(panelNative->SetSwapChain(swapChain.get()));

		//Create a Direct2D target bitmap associated with the swap chain back buffer,
		//and set it as the current target.
		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
				96.f,
				96.f
			);

		winrt::com_ptr<::IDXGISurface> dxgiBackBuffer;
		swapChain->GetBuffer(0, __uuidof(dxgiBackBuffer), dxgiBackBuffer.put_void());

		winrt::com_ptr<::ID2D1Bitmap1> targetBitmap;
		winrt::check_hresult(
			d2dDeviceContext->CreateBitmapFromDxgiSurface(
				dxgiBackBuffer.get(),
				&bitmapProperties,
				targetBitmap.put()
			)
		);

		d2dDeviceContext->SetTarget(targetBitmap.get());

		//Draw using Direct2D context
		d2dDeviceContext->BeginDraw();
		d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Orange));

		winrt::com_ptr<::ID2D1SolidColorBrush> brush;
		winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Chocolate),
			D2D1::BrushProperties(0.8f),
			brush.put()
		));

		D2D1_SIZE_F const size{ 1000,500 };
		D2D1_RECT_F const rect{ 100.0f, 100.0f, size.width - 100.0f, size.height - 100.0f };
		d2dDeviceContext->DrawRectangle(rect, brush.get(), 100.0f);

		d2dDeviceContext->EndDraw();
		swapChain->Present(1, 0);
	}

}


