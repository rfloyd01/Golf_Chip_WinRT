#pragma once

//The sample I'm following puts all directX functions in the DX namespace, I don't think it's necessary
//but makes things a little more clear
namespace DX
{
	//Provides and interface for an application that owns DeviceResources to be notified of the device being lost
	//or created.
	interface IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	//A class that controls all the DirectX device resources
	class DirectXDeviceResources
	{
	public:
		//CONSTRUCTORS
		DirectXDeviceResources();

		//PUBLIC FUNCTIONS
		void UpdateStereoState(); //this is used for stereo graphics, probably don't need
		void ValidateDevice();
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
		void Trim();
		void Present();

		//SETTERS
		void SetSwapChainPanel(winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& panel);
		void SetLogicalSize(winrt::Windows::Foundation::Size logicalSize);
		void SetCurrentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations currentOrientation); //this is for when a something smart phone is flipped horizontally, probably don't need
		void SetDpi(float dpi);
		void SetCompositionScale(float compositionScaleX, float compositionScaleY);

		//GETTERS
		//Device Getters
		winrt::Windows::Foundation::Size GetOutputSize() const { return m_outputSize; }
		winrt::Windows::Foundation::Size GetLogicalSize() const { return m_logicalSize; }
		winrt::Windows::Foundation::Size GetRenderTargetSize() const { return m_d3dRenderTargetSize; }
		bool                             GetStereoState() const { return m_stereoEnabled; }
		winrt::Windows::UI::Xaml::Controls::SwapChainPanel GetSwapChainPanel() const { return m_swapChainPanel; }

		//Direct3D Getters
		ID3D11Device2*                   GetD3DDevice() const { return m_d3dDevice.get(); }
		ID3D11DeviceContext2*            GetD3DDeviceContext() const { return m_d3dContext.get(); }
		IDXGISwapChain1*                 GetSwapChain() const { return m_swapChain.get(); }
		D3D_FEATURE_LEVEL                GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		ID3D11RenderTargetView*          GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.get(); }
		ID3D11RenderTargetView*          GetBackBufferRenderTargetViewRight() const { return m_d3dRenderTargetViewRight.get(); }
		ID3D11DepthStencilView*          GetDepthStencilView() const { return m_d3dDepthStencilView.get(); }
		D3D11_VIEWPORT                   GetScreenViewport() const { return m_screenViewport; }
		DirectX::XMFLOAT4X4              GetOrientationTransform3D() const { return m_orientationTransform3D; }

		//Direct2D Getters
		ID2D1Factory1*                   GetD2DFactory() const { return m_d2dFactory.get(); }
		ID2D1Device*                     GetD2DDevice() const { return m_d2dDevice.get(); }
		ID2D1DeviceContext*              GetD2DDeviceContext() const { return m_d2dContext.get(); }
		ID2D1Bitmap1*                    GetD2DTargetBitmap() const { return m_d2dTargetBitmap.get(); }
		ID2D1Bitmap1*                    GetD2DTargetBitmapRight() const { return m_d2dTargetBitmapRight.get(); }
		IDWriteFactory*                  GetDWriteFactory() const { return m_dwriteFactory.get(); }
		IWICImagingFactory*              GetWicImagingFactory() const { return m_wicFactory.get(); }
		D2D1::Matrix3x2F                 GetOrientationTransform2D() const { return m_orientationTransform2D; }

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		DXGI_MODE_ROTATION ComputeDisplayRotation(); //If this is for rotation of a physical device then I probably don't need
		void CheckStereoEnabledStatus(); //Probably don't need, this is for some stereo 3d graphics thing

		//Direct3D objects
		winrt::com_ptr<ID3D11Device2>                          m_d3dDevice;
		winrt::com_ptr<ID3D11DeviceContext2>                   m_d3dContext;
		winrt::com_ptr<IDXGISwapChain1>                        m_swapChain;

		//Direct3D rendering objects (required for 3D)
		winrt::com_ptr<ID3D11RenderTargetView>                 m_d3dRenderTargetView;
		winrt::com_ptr<ID3D11RenderTargetView>                 m_d3dRenderTargetViewRight;
		winrt::com_ptr<ID3D11DepthStencilView>                 m_d3dDepthStencilView;
		D3D11_VIEWPORT                                         m_screenViewport;

		//Direct2D drawing components
		winrt::com_ptr<ID2D1Factory1>                          m_d2dFactory; //the guide uses factory2 here and not factory1, but I don't have it as an option
		winrt::com_ptr<ID2D1Device>                            m_d2dDevice; //the guide uses device1 here and not device, but I don't have it as an option
		winrt::com_ptr<ID2D1DeviceContext>                     m_d2dContext; //the guide uses devicecontext 1 here and not devicecontext, but I don't have it as an option
		winrt::com_ptr<ID2D1Bitmap1>                           m_d2dTargetBitmap;
		winrt::com_ptr<ID2D1Bitmap1>                           m_d2dTargetBitmapRight;

		//DirectWrite drawing components (I think this must handle writing text)
		winrt::com_ptr<IDWriteFactory>                         m_dwriteFactory; //the guide uses WriteFactory2 here and not WriteFactory but I don't have it as an option
		winrt::com_ptr<IWICImagingFactory>                     m_wicFactory; //the guide uses factory2 here and not factory but I don't have it as an option

		//Cached reference to the XAML swap chain panel
		//Note: the guide only uses a single XAML page so this value never changes, however, my app will have different pages
		//that have different swap chain panels so this variable will be changed periodically. I should take note that this
		//doesn't mess anything up.
		winrt::Windows::UI::Xaml::Controls::SwapChainPanel    m_swapChainPanel; //TODO: Does this need to become a pointer?

		//Cached device properties
		D3D_FEATURE_LEVEL                                      m_d3dFeatureLevel;
		winrt::Windows::Foundation::Size                       m_d3dRenderTargetSize;
		winrt::Windows::Foundation::Size                       m_outputSize;
		winrt::Windows::Foundation::Size                       m_logicalSize;
		winrt::Windows::Graphics::Display::DisplayOrientations m_nativeOrientation; //used for physical device rotation, I probably don't need
		winrt::Windows::Graphics::Display::DisplayOrientations m_currentOrientation; //used for physical device rotation, I probably don't need
		float                                                  m_dpi;
		float                                                  m_compositionScaleX;
		float                                                  m_compositionScaleY;
		bool                                                   m_stereoEnabled; //for 3d stereo imaging, probably don't need

		//Transforms used for display orientation
		//Note: this is for rotating of a physical device (like a smartphone or tablet), not the Direct3D device. For now
		//I don't really need this as this app will just be for desktops for now, however, I'll keep it in.
		D2D1::Matrix3x2F                                       m_orientationTransform2D;
		DirectX::XMFLOAT4X4                                    m_orientationTransform3D;

		//The IDeviceNotify can be held directly as it owns the DirectXDeviceResources.
		IDeviceNotify*                                         m_deviceNotify;
	};
}

namespace winrt::Golf_Chip_WinRT
{
	struct GlobalDirectXDeviceResources
	{
		//This struct holds the one global instance of DirextXDeviceResources. It's created as a shared pointer
		//so that it can be accessed by multiple pages simultaneously. This global instance
		//is created upon startup of the app.
		static std::shared_ptr<DX::DirectXDeviceResources> m_deviceResources;
	};
}