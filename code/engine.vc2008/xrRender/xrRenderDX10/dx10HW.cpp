// dx10HW.cpp: implementation of the DX10 specialisation of CHW.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
//#include <d3dx9.h>
//#include <d3dx10.h>
//#include <d3dx11.h>
//#include <d3d12.h>
#pragma warning(default:4995)
#include "../xrRender/HW.h"
#include "../../xrEngine/XR_IOConsole.h"
#include "../../xrCore/xrAPI.h"

#include "StateManager\dx10SamplerStateCache.h"
#include "StateManager\dx10StateCache.h"

void	fill_vid_mode_list			(CHW* _hw);
void	free_vid_mode_list			();

void	fill_render_mode_list		();
void	free_render_mode_list		();

CHW HW;

CHW::CHW() : m_pAdapter(0), pDevice(NULL), m_move_window(true)
{
	Device.seqAppActivate.Add(this);
	Device.seqAppDeactivate.Add(this);
}

CHW::~CHW()
{
	Device.seqAppActivate.Remove(this);
	Device.seqAppDeactivate.Remove(this);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
	R_CHK( CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory)));

#ifdef USE_DX12

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(adapterIndex, &m_pAdapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        m_pAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }

    R_ASSERT(m_pAdapter.Get());

#else
    if (!m_pAdapter)
        m_pFactory->EnumAdapters(0, &m_pAdapter.Get());
#endif

}

void CHW::DestroyD3D()
{
	_SHOW_REF				("refCount:m_pAdapter",m_pAdapter);
}

void CHW::CreateDevice( HWND m_hWnd, bool move_window )
{
	m_move_window			= move_window;
	CreateD3D();
	// TODO: DX10: Create appropriate initialization

	// General - select adapter and device
	BOOL  bWindowed			= !psDeviceFlags.is(rsFullscreen);

// 	m_DriverType = Caps.bForceGPU_REF ? 
// 		D3D_DRIVER_TYPE_REFERENCE : D3D_DRIVER_TYPE_HARDWARE;
		
	// Display the name of video board
	DXGI_ADAPTER_DESC Desc;
	R_CHK( m_pAdapter->GetDesc(&Desc) );
	//	Warning: Desc.Description is wide string
	Msg		("* GPU [vendor:%X]-[device:%X]: %S", Desc.VendorId, Desc.DeviceId, Desc.Description);
	Caps.id_vendor	= Desc.VendorId;
	Caps.id_device	= Desc.DeviceId;

	// Select back-buffer & depth-stencil format
	D3DFORMAT&	fTarget	= Caps.fTarget;
	D3DFORMAT&	fDepth	= Caps.fDepth;

	//	HACK: DX10: Embed hard target format.
	fTarget = D3DFMT_X8R8G8B8;	//	No match in DX10. D3DFMT_A8B8G8R8->DXGI_FORMAT_R8G8B8A8_UNORM
	fDepth = selectDepthStencil(fTarget);
	
	// Set up the presentation parameters
#ifdef USE_DX12
    DXGI_SWAP_CHAIN_DESC1& sd = m_ChainDesc;
    ZeroMemory(&sd, sizeof(sd));

    selectResolution(sd.Width, sd.Height, bWindowed);

    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.Stereo = FALSE;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.BufferCount = 2;
    sd.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC SCFullscreenDesc;
    ZeroMemory(&SCFullscreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));

    SCFullscreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    SCFullscreenDesc.Windowed = bWindowed;
#else
	DXGI_SWAP_CHAIN_DESC	&sd	= m_ChainDesc;
    ZeroMemory(&sd, sizeof(sd));


	selectResolution	(sd.BufferDesc.Width, sd.BufferDesc.Height, bWindowed);

	//	TODO: DX10: implement dynamic format selection
	//sd.BufferDesc.Format		= fTarget;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Prep for HDR10; breaks nothing
	sd.BufferCount = 1;

	// Multisample
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = bWindowed;

	if (bWindowed)
	{
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
		sd.BufferDesc.RefreshRate = selectRefresh( sd.BufferDesc.Width, sd.BufferDesc.Height, sd.BufferDesc.Format);

	//	Additional set up
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
#endif
	UINT createDeviceFlags = 0;
	HRESULT R;
#ifdef USE_DX11
    D3D_FEATURE_LEVEL pFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };

   R =  D3D11CreateDeviceAndSwapChain(0, m_DriverType, 0, createDeviceFlags, pFeatureLevels, sizeof(pFeatureLevels)/sizeof(pFeatureLevels[0]),
										  D3D11_SDK_VERSION, &sd, &m_pSwapChain, &pDevice, &FeatureLevel, &pContext);
#elif defined (USE_DX12)

    CHK_DX(D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)));

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    CHK_DX(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue)));
    CHK_DX(m_pFactory->CreateSwapChainForHwnd(pCommandQueue.Get(), m_hWnd, &m_ChainDesc, &SCFullscreenDesc, nullptr, &m_pSwapChain));
    CHK_DX(m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

    R = 0; //all ok
#else
   R =  D3DX10CreateDeviceAndSwapChain(m_pAdapter, m_DriverType, 0, createDeviceFlags, &sd, &m_pSwapChain, &pDevice );

   pContext = pDevice;
   FeatureLevel = D3D_FEATURE_LEVEL_10_0;
   if(!FAILED(R))
   {
      D3DX10GetFeatureLevel1( pDevice, &pDevice1 );
	  FeatureLevel = D3D_FEATURE_LEVEL_10_1;
   }
   pContext1 = pDevice1;
#endif

	if (FAILED(R))
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		Msg					("Failed to initialize graphics hardware.\n"
							 "Please try to restart the game.\n"
							 "CreateDevice returned 0x%08x", R
							 );
		FlushLog			();
		MessageBox			(NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
		TerminateProcess	(GetCurrentProcess(),0);
	};
	R_CHK(R);

	_SHOW_REF	("* CREATE: DeviceREF:",HW.pDevice);
	//	Create render target and depth-stencil views here
	UpdateViews();

	size_t	memory									= Desc.DedicatedVideoMemory;
	Msg		("*     Texture memory: %d M",		memory/(1024*1024));
	updateWindowProps							(m_hWnd);
	fill_vid_mode_list							(this);
}

void CHW::DestroyDevice()
{
	//	Destroy state managers

	//VERTVER: DX12 have a new feature - Pipeline State Object.
	// And yes, StateManager now is broken.
	StateManager.Reset();
	RSManager.ClearStateArray();
	DSSManager.ClearStateArray();
	BSManager.ClearStateArray();
	SSManager.ClearStateArray();

	_SHOW_REF				("refCount:pBaseZB",pBaseZB);

	_SHOW_REF				("refCount:pBaseRT",pBaseRT);
	//	Must switch to windowed mode to release swap chain
    
#ifdef USE_DX12
    if (!m_FullscreenDesc.Windowed) m_pSwapChain->SetFullscreenState(FALSE, NULL);
#else
	if (!m_ChainDesc.Windowed) m_pSwapChain->SetFullscreenState( FALSE, NULL);
#endif

	_SHOW_REF				("refCount:m_pSwapChain",m_pSwapChain);

#ifndef USE_DX11
#endif
	_SHOW_REF				("DeviceREF:",HW.pDevice);

	DestroyD3D				();

#ifndef _EDITOR
	free_vid_mode_list		();
#endif
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::Reset(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 &cd = m_ChainDesc;

	bool bWindowed = !psDeviceFlags.is(rsFullscreen);

#ifdef USE_DX12
    m_FullscreenDesc.Windowed = bWindowed;
#else
    cd.Windowed = bWindowed;
#endif

	m_pSwapChain->SetFullscreenState(!bWindowed, 0);

    UINT dwWidth = 0;
    UINT dwHeight = 0;
    
#ifdef USE_DX12
    dwWidth = m_ChainDesc.Width;
    dwHeight = m_ChainDesc.Height;
#else
	DXGI_MODE_DESC	&desc = m_ChainDesc.BufferDesc;
    dwWidth = desc.Width;
    dwHeight = desc.Height;
#endif

	selectResolution(dwWidth, dwHeight, bWindowed);

	if (bWindowed)
	{
#ifdef USE_DX12
        m_FullscreenDesc.RefreshRate.Numerator = 60;
        m_FullscreenDesc.RefreshRate.Denominator = 1;
#else
        desc.RefreshRate.Numerator = 60;
        desc.RefreshRate.Denominator = 1;
#endif
	}
    else
    {
#ifdef USE_DX12
        m_FullscreenDesc.RefreshRate = selectRefresh(dwWidth, dwHeight, m_ChainDesc.Format);
#else
		desc.RefreshRate = selectRefresh(dwWidth, dwHeight, desc.Format);
#endif
    }


#ifdef USE_DX12
    DXGI_MODE_DESC modeDesc;
    modeDesc.Width = dwWidth;
    modeDesc.Height = dwHeight;
    modeDesc.RefreshRate = m_FullscreenDesc.RefreshRate;
    modeDesc.Scaling = m_FullscreenDesc.Scaling;
    modeDesc.ScanlineOrdering = m_FullscreenDesc.ScanlineOrdering;
    CHK_DX(m_pSwapChain->ResizeTarget(&modeDesc));
#else
	CHK_DX(m_pSwapChain->ResizeTarget(&desc));
#endif


	_SHOW_REF("refCount:pBaseZB", pBaseZB);
	_SHOW_REF("refCount:pBaseRT", pBaseRT);

#ifdef USE_DX12
    DXGI_FORMAT format = m_ChainDesc.Format;
#else
    DXGI_FORMAT format = desc.Format;
#endif

	CHK_DX(m_pSwapChain->ResizeBuffers(
		cd.BufferCount,
		dwWidth,
		dwHeight,
        format,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	UpdateViews();
	updateWindowProps(hwnd);
}

D3DFORMAT CHW::selectDepthStencil	(D3DFORMAT fTarget)
{
	// R3 hack
#pragma todo("R3 need to specify depth format")
	return D3DFMT_D24S8;
}

void CHW::selectResolution( u32 &dwWidth, u32 &dwHeight, BOOL bWindowed )
{
	fill_vid_mode_list			(this);

	if(bWindowed)
	{
		dwWidth		= psCurrentVidMode[0];
		dwHeight	= psCurrentVidMode[1];
	}
	else //check
	{
		string64					buff;
		xr_sprintf					(buff,sizeof(buff),"%dx%d",psCurrentVidMode[0],psCurrentVidMode[1]);

		if(_ParseItem(buff,vid_mode_token)==u32(-1)) //not found
		{ //select safe
			xr_sprintf				(buff,sizeof(buff),"vid_mode %s",vid_mode_token[0].name);
			Console->Execute		(buff);
		}

		dwWidth						= psCurrentVidMode[0];
		dwHeight					= psCurrentVidMode[1];
	}
}

//	TODO: DX10: check if we need these
DXGI_RATIONAL CHW::selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt)
{
	DXGI_RATIONAL	res;

	res.Numerator = 60;
	res.Denominator = 1;

	float	CurrentFreq = 60.0f;

	if (psDeviceFlags.is(rsRefresh60hz))	
	{
		return res;
	}
	else
	{
		xr_vector<DXGI_MODE_DESC>	modes;

		IDXGIOutput *pOutput;
		m_pAdapter->EnumOutputs(0, &pOutput);
		VERIFY(pOutput);

		UINT num = 0;
		DXGI_FORMAT format = fmt;
		UINT flags         = 0;

		// Get the number of display modes available
		pOutput->GetDisplayModeList( format, flags, &num, 0);

		// Get the list of display modes
		modes.resize(num);
		pOutput->GetDisplayModeList( format, flags, &num, &modes.front());

		_RELEASE(pOutput);

		for (u32 i=0; i<num; ++i)
		{
			DXGI_MODE_DESC &desc = modes[i];

			if( (desc.Width == dwWidth) 
				&& (desc.Height == dwHeight)
				)
			{
				VERIFY(desc.RefreshRate.Denominator);
				float TempFreq = float(desc.RefreshRate.Numerator)/float(desc.RefreshRate.Denominator);
				if ( TempFreq > CurrentFreq )
				{
					CurrentFreq = TempFreq;
					res = desc.RefreshRate;
				}
			}
		}

		return res;
	}
}

void CHW::OnAppActivate()
{
    bool bWindowed = false;
    
#ifdef USE_DX12
    bWindowed = m_FullscreenDesc.Windowed;
#else
    bWindowed = m_ChainDesc.Windowed;
#endif

    HWND hWindow = NULL;
    CHK_DX(m_pFactory->GetWindowAssociation(&hWindow));

	if ( m_pSwapChain && !bWindowed)
	{
		ShowWindow(hWindow, SW_RESTORE );
		m_pSwapChain->SetFullscreenState( TRUE, NULL );
	}
}

void CHW::OnAppDeactivate()
{
    bool bWindowed = false;
#ifdef USE_DX12
    bWindowed = m_FullscreenDesc.Windowed;
#else
    bWindowed = m_ChainDesc.Windowed;
#endif

    HWND hWindow = NULL;
    CHK_DX(m_pFactory->GetWindowAssociation(&hWindow));

	if ( m_pSwapChain && !bWindowed)
	{
		m_pSwapChain->SetFullscreenState( FALSE, NULL );
		ShowWindow(hWindow, SW_MINIMIZE );
	}
}


BOOL CHW::support( D3DFORMAT fmt, DWORD type, DWORD usage)
{
	VERIFY(!"Implement CHW::support");
	return TRUE;
}

void CHW::updateWindowProps(HWND m_hWnd)
{
	//	BOOL	bWindowed				= strstr(Core.Params,"-dedicated") ? TRUE : !psDeviceFlags.is	(rsFullscreen);
	BOOL	bWindowed				= !psDeviceFlags.is	(rsFullscreen);

	u32		dwWindowStyle			= 0;
	// Set window properties depending on what mode were in.
	if (bWindowed)		{
		if (m_move_window) {
			if (strstr(Core.Params,"-no_dialog_header"))
				dwWindowStyle |= WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
			bool bBordersMode = strstr(Core.Params, "-draw_borders");
			dwWindowStyle = WS_VISIBLE;
			if (bBordersMode)
			dwWindowStyle |= WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;
			// When moving from fullscreen to windowed mode, it is important to
			// adjust the window size after recreating the device rather than
			// beforehand to ensure that you get the window size you want.  For
			// example, when switching from 640x480 fullscreen to windowed with
			// a 1000x600 window on a 1024x768 desktop, it is impossible to set
			// the window size to 1000x600 until after the display mode has
			// changed to 1024x768, because windows cannot be larger than the
			// desktop.

			RECT			m_rcWindowBounds;
			float fYOffset = 0.f;
			BOOL			bCenter = TRUE;
			if (strstr(Core.Params, "-no_center_screen"))	bCenter = FALSE;

            UINT dwWidth = 0;
            UINT dwHeight = 0;
#ifdef USE_DX12
            dwWidth = m_ChainDesc.Width;
            dwHeight = m_ChainDesc.Height;
#else
            dwWidth = m_ChainDesc.BufferDesc.Width;
            dwHeight = m_ChainDesc.BufferDesc.Height;
#endif

			if (bCenter) {
				RECT				DesktopRect;

				GetClientRect		(GetDesktopWindow(), &DesktopRect);
				SetRect(&m_rcWindowBounds, (DesktopRect.right - dwWidth) / 2,
				(DesktopRect.bottom - dwHeight) / 2,
				(DesktopRect.right + dwWidth) / 2,
				(DesktopRect.bottom + dwHeight) / 2);
			}
			else
			{
				if (bBordersMode) {
					fYOffset = GetSystemMetrics(SM_CYCAPTION); // size of the window title bar
				}
					SetRect(&m_rcWindowBounds, 0, 0, dwWidth, dwHeight);
				
			};

			AdjustWindowRect		(	&m_rcWindowBounds, dwWindowStyle, FALSE );

			SetWindowPos			(	m_hWnd, 
				HWND_NOTOPMOST,	
				m_rcWindowBounds.left, 
				m_rcWindowBounds.top + fYOffset,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top),
				SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME );
		}
	}
	else
	{
		SetWindowLong			( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );
	}

	ShowCursor	(FALSE);
	SetForegroundWindow( m_hWnd );
}


struct _uniq_mode
{
	_uniq_mode(LPCSTR v):_val(v){}
	LPCSTR _val;
	bool operator() (LPCSTR _other) {return !stricmp(_val,_other);}
};

void free_vid_mode_list()
{
	for( int i=0; vid_mode_token[i].name; i++ )
	{
		xr_free					(vid_mode_token[i].name);
	}
	xr_free						(vid_mode_token);
	vid_mode_token				= NULL;
}

void fill_vid_mode_list(CHW* _hw)
{
	if(vid_mode_token != NULL)		return;
	xr_vector<LPCSTR>	_tmp;
	xr_vector<DXGI_MODE_DESC>	modes;

	IDXGIOutput *pOutput;
	//_hw->m_pSwapChain->GetContainingOutput(&pOutput);
	_hw->m_pAdapter->EnumOutputs(0, &pOutput);
	VERIFY(pOutput);

	UINT num = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R10G10B10A2_UNORM;// [FX to Swartz27]: D3DX11SaveTextureToMemory don't give the value 'saved'
	UINT flags         = 0;

	// Get the number of display modes available
	pOutput->GetDisplayModeList( format, flags, &num, 0);

	// Get the list of display modes
	modes.resize(num);
	pOutput->GetDisplayModeList( format, flags, &num, &modes.front());

	_RELEASE(pOutput);

	for (u32 i=0; i<num; ++i)
	{
		DXGI_MODE_DESC &desc = modes[i];
		string32		str;

		if(desc.Width < 800)
			continue;

		xr_sprintf(str, sizeof(str), "%dx%d", desc.Width, desc.Height);

		if(_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back				(NULL);
		_tmp.back()					= xr_strdup(str);
	}
	
	u32 _cnt						= _tmp.size()+1;

	vid_mode_token					= xr_alloc<xr_token>(_cnt);

	vid_mode_token[_cnt-1].id			= -1;
	vid_mode_token[_cnt-1].name		= NULL;

#ifdef DEBUG
	Msg("Available video modes[%d]:",_tmp.size());
#endif // DEBUG
	for( u32 i=0; i<_tmp.size(); ++i )
	{
		vid_mode_token[i].id		= i;
		vid_mode_token[i].name		= _tmp[i];
#ifdef DEBUG
		Msg							("[%s]",_tmp[i]);
#endif // DEBUG
	}
}

#ifdef USE_DX12

void CHW::UpdateViews()
{
    DXGI_SWAP_CHAIN_DESC1 &sd = m_ChainDesc;
    D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
    
    RTVHeapDesc.NumDescriptors = 2;
    RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    CHK_DX(pDevice->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    m_rtvDescSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE RTVCPUHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    CHK_DX(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBaseRT)));
    pDevice->CreateRenderTargetView(pBaseRT.Get(), nullptr, RTVCPUHandle);
    RTVCPUHandle.Offset(1, m_rtvDescSize);

    CHK_DX(m_pSwapChain->GetBuffer(1, IID_PPV_ARGS(&pBaseRT2)));
    pDevice->CreateRenderTargetView(pBaseRT2.Get(), nullptr, RTVCPUHandle);
    RTVCPUHandle.Offset(1, m_rtvDescSize);

    CHK_DX (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));

    CHK_DX (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pCommandList)));
}


#else //DX11 | DX10

void CHW::UpdateViews()
{
	DXGI_SWAP_CHAIN_DESC &sd = m_ChainDesc;
	HRESULT R;

	// Create a render target view
	//R_CHK	(pDevice->GetRenderTarget			(0,&pBaseRT));
	ID3DTexture2D *pBuffer;
	R = m_pSwapChain->GetBuffer( 0, __uuidof( ID3DTexture2D ), (LPVOID*)&pBuffer );
	R_CHK(R);

	R = pDevice->CreateRenderTargetView( pBuffer, NULL, &pBaseRT);
	pBuffer->Release();
	R_CHK(R);

	//	Create Depth/stencil buffer
	//	HACK: DX10: hard depth buffer format
	//R_CHK	(pDevice->GetDepthStencilSurface	(&pBaseZB));
	ID3DTexture2D* pDepthStencil = NULL;
	D3D_TEXTURE2D_DESC descDepth;
	descDepth.Width = sd.BufferDesc.Width;
	descDepth.Height = sd.BufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D_USAGE_DEFAULT;

	descDepth.BindFlags = D3D_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	R = pDevice->CreateTexture2D( &descDepth,       // Texture desc
		NULL,                  // Initial data
		&pDepthStencil ); // [out] Texture
	R_CHK(R);

	//	Create Depth/stencil view
	R = pDevice->CreateDepthStencilView( pDepthStencil, NULL, &pBaseZB );
	R_CHK(R);

	pDepthStencil->Release();
}

#endif