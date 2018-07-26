#include "stdafx.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, message, wParam, lParam );
}

typedef HRESULT (__stdcall *D3D12_CREATE_DEVICE_FUNC)(
    IUnknown          *pAdapter,
    D3D_FEATURE_LEVEL MinimumFeatureLevel,
    REFIID            riid,
    void              **ppDevice
);

bool TestDX12Present()
{
	HMODULE hD3D12 = LoadLibrary("d3d12.dll");

	if (!hD3D12)
	{
		Msg("* DX12: failed to load d3d12.dll");
		return false;
	}

	D3D12_CREATE_DEVICE_FUNC pD3D12CreateDevice =
		(D3D12_CREATE_DEVICE_FUNC)GetProcAddress(hD3D12, "D3D12CreateDevice");

	if (!pD3D12CreateDevice)
	{
		Msg("* DX12: failed to get address of D3D12CreateDevice");
		return false;
	}

	// Register class
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = GetModuleHandleA(nullptr);
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

BOOL	xrRender_test_hw		()
{
	return	TestDX12Present();					//SUCCEEDED(hr);
}
