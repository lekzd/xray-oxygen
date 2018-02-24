#include "stdafx.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, message, wParam, lParam );
}
#ifdef USE_DX11
typedef HRESULT (__stdcall *FuncPtrD3D11CreateDeviceAndSwapChain)(
    IDXGIAdapter* pAdapter,
    D3D_DRIVER_TYPE DriverType,
    HMODULE Software,
    UINT Flags,
    CONST D3D_FEATURE_LEVEL* pFeatureLevels,
    UINT FeatureLevels,
    UINT SDKVersion,
    CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain,
    ID3D11Device** ppDevice,
    D3D_FEATURE_LEVEL* pFeatureLevel,
    ID3D11DeviceContext** ppImmediateContext );
#endif
#ifdef USE_DX12

typedef HRESULT (__stdcall *FuncPtrD3D12CreateDevice)(
    IUnknown          *pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID            riid,
    void              **ppDevice
);

#endif
#ifdef USE_DX11
bool TestDX11Present()
{
	HMODULE hD3D11 = LoadLibrary("d3d11.dll");

	if (!hD3D11)
	{
		Msg("* DX11: failed to load d3d11.dll");
		return false;
	}
	
	FuncPtrD3D11CreateDeviceAndSwapChain pD3D11CreateDeviceAndSwapChain = 
		(FuncPtrD3D11CreateDeviceAndSwapChain)GetProcAddress(hD3D11, "D3D11CreateDeviceAndSwapChain");

	if (!pD3D11CreateDeviceAndSwapChain)
	{
		Msg("* DX11: failed to get address of D3D11CreateDeviceAndSwapChain");
		return false;
	}

	// Register class
	WNDCLASSEX wcex;
    std::memset(&wcex,0,sizeof(wcex));
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.lpszClassName = "TestDX11WindowClass";
	if( !RegisterClassEx( &wcex ) )
	{
		Msg("* DX11: failed to register window class");
		return false;
	}

	// Create window
	HWND hWnd = CreateWindow( "TestDX11WindowClass", "",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL );

	DXGI_SWAP_CHAIN_DESC sd;

	if (!hWnd)
	{
		Msg("* DX11: failed to create window");
		return false;
	}

	HRESULT hr = E_FAIL;

    std::memset(&sd,0,sizeof( sd ));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height = 600;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL pFeatureLevels[] = {D3D_FEATURE_LEVEL_11_0};
	D3D_FEATURE_LEVEL FeatureLevel;

	ID3D11Device*           pd3dDevice = NULL;
	ID3D11DeviceContext*    pContext = NULL;
	IDXGISwapChain*         pSwapChain = NULL;

	hr = pD3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, pFeatureLevels, 1,
		D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &FeatureLevel, &pContext );

	if (FAILED(hr))
		Msg("* D3D11: device creation failed with hr=0x%08x", hr);

	if (pContext) pContext->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pd3dDevice) pd3dDevice->Release();
	
	FreeLibrary(hD3D11);
	
	DestroyWindow(hWnd);

	return SUCCEEDED(hr);
}
#endif

#ifdef USE_DX12
bool TestDX12Present()
{
	HMODULE hD3D12 = LoadLibrary("d3d12.dll");

	if (!hD3D12)
	{
		Msg("* DX12: failed to load d3d12.dll");
		return false;
	}

    FuncPtrD3D12CreateDevice pD3D12CreateDevice =
		(FuncPtrD3D12CreateDevice)GetProcAddress(hD3D12, "D3D12CreateDevice");

	if (!pD3D12CreateDevice)
	{
		Msg("* DX12: failed to get address of D3D12CreateDevice");
		return false;
	}

	// Register class
	WNDCLASSEX wcex;
	std::memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.lpszClassName = "TestDX12WindowClass";
	if (!RegisterClassEx(&wcex))
	{
		Msg("* DX12: failed to register window class");
		return false;
	}

	// Create window
	HWND hWnd = CreateWindow("TestDX12WindowClass", "",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL);


	if (!hWnd)
	{
		Msg("* DX12: failed to create window");
		return false;
	}

    ComPtr<IDXGIFactory4> DXGIFactory;
    if (!SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory))))
    {
        FreeLibrary(hD3D12);

        DestroyWindow(hWnd);
        return false;
    }

    ComPtr<IDXGIAdapter1> HardwareAdapter;
    bool bCreatedDevice = false;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory->EnumAdapters1(adapterIndex, &HardwareAdapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        HardwareAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(HardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            bCreatedDevice = true;
            break;
        }
    }

    if (HardwareAdapter.Get() == nullptr || !bCreatedDevice)
    {
        FreeLibrary(hD3D12);

        DestroyWindow(hWnd);
        return false;
    }


	FreeLibrary(hD3D12);

	DestroyWindow(hWnd);

	return true;
}
#endif
BOOL	xrRender_test_hw		()
{	//#VERTVER: The part of XR3DA code (2003-2005 years) have been deleted
#if defined(USE_DX12)
	return	TestDX12Present();					//SUCCEEDED(hr);
#else
	return	TestDX11Present();
#endif 
}
