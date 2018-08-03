// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class  CHW
#if defined(USE_DX10) || defined(USE_DX11) || defined(USE_DX12)
	:	public pureAppActivate, 
		public pureAppDeactivate
#endif	//	USE_DX10
{
//	Functions section
public:
	CHW();
	~CHW();

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw, bool move_window);

	void					DestroyDevice			();

	void					Reset					(HWND hw);

	void					selectResolution		(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed);
	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	u32						selectPresentInterval	();
	u32						selectGPU				();
	u32						selectRefresh			(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);
	void					updateWindowProps		(HWND hw);
	BOOL					support					(D3DFORMAT fmt, DWORD type, DWORD usage);

#ifdef DEBUG
#if defined(USE_DX10) || defined(USE_DX11) || defined(USE_DX12)
	void					Validate				(void){};
#else	//	USE_DX10
	void					Validate				(void){	VERIFY(pDevice); VERIFY(pD3D); };
#endif	//	USE_DX10
#else
	void					Validate				(void){};
#endif

    ComPtr < IDXGIFactory2 >				m_pFactory;
//	Variables section
#if defined(USE_DX12)
public:
#ifdef DEBUG
	ComPtr < ID3D12Debug >					pDebugControl;
#endif
    ComPtr < IDXGIAdapter1 >                m_pAdapter;
	ComPtr < IDXGISwapChain1 >				m_pSwapChain;
    ComPtr < ID3D12Device >					pDevice;
	ComPtr < ID3D12CommandList >			pCPUCommandList;		// For 
    ComPtr < ID3D12CommandQueue >           pCommandQueue;
    ComPtr < ID3D12CommandAllocator >       pCommandAllocator;
    ComPtr < ID3D12GraphicsCommandList >    pCommandList;			// Main frame Cmd list
    ComPtr < ID3D12GraphicsCommandList >    pTextureLoadCmdList;	// Command list for texture load
    ComPtr < ID3D12GraphicsCommandList >    pMeshLoadCmdList;		// Command list for model load
    ComPtr < ID3D12DescriptorHeap >         m_rtvHeap;
    ComPtr < ID3D12DescriptorHeap >         m_dsvHeap;
    ComPtr < ID3D12DescriptorHeap >         m_samplerHeap;
    ComPtr < ID3D12DescriptorHeap >         m_cbvSrvHeap;
	ComPtr < ID3D12PipelineState >			pPipelineState;
	ComPtr < ID3D12PipelineState >			pDepthOnlyPipelineState;
    ComPtr < ID3D12Resource >				pResource;
    ComPtr < ID3D12Resource >		        pBaseRT;
    ComPtr < ID3D12Resource >		        pBaseRT2;
    ComPtr < ID3D12RootSignature >          pRootSignature;

    ComPtr < ID3D12Resource >		        pBaseZB;

	//-----------------------------------
	CHWCaps							Caps;
	//-----------------------------------

	D3D_DRIVER_TYPE					m_DriverType;	//	DevT equivalent
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_FullscreenDesc;
    DXGI_SWAP_CHAIN_DESC1			m_ChainDesc;	//	DevPP equivalent

	D3D12_FEATURE					FeatureLevel;

    UINT							m_rtvDescSize;
#elif  defined(USE_DX11)	//	USE_DX10
public:
    ComPtr < IDXGIAdapter >					m_pAdapter;	//	pD3D equivalent
    ComPtr < ID3D11Device >					pDevice;	//	combine with DX9 pDevice via typedef
    ComPtr < ID3D11DeviceContext >			pContext;	//	combine with DX9 pDevice via typedef
    ComPtr < IDXGISwapChain >				m_pSwapChain;
    ComPtr < ID3D11RenderTargetView >		pBaseRT;	//	combine with DX9 pBaseRT via typedef
    ComPtr < ID3D11DepthStencilView >		pBaseZB;

	CHWCaps							Caps;

	D3D_DRIVER_TYPE					m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC			m_ChainDesc;	//	DevPP equivalent
	D3D_FEATURE_LEVEL				FeatureLevel;
#elif defined(USE_DX10)
public:
	ComPtr < IDXGIAdapter >					m_pAdapter;	//	pD3D equivalent
	ComPtr < ID3D10Device1 >       			pDevice1;	//	combine with DX9 pDevice via typedef
	ComPtr < ID3D10Device >        			pDevice;	//	combine with DX9 pDevice via typedef
	ComPtr < ID3D10Device1 >       			pContext1;	//	combine with DX9 pDevice via typedef
	ComPtr < ID3D10Device >        			pContext;	//	combine with DX9 pDevice via typedef
	ComPtr < IDXGISwapChain >				m_pSwapChain;
	ComPtr < ID3D10RenderTargetView >		pBaseRT;	//	combine with DX9 pBaseRT via typedef
	ComPtr < ID3D10DepthStencilView >		pBaseZB;

	CHWCaps					Caps;

	D3D10_DRIVER_TYPE		m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC	m_ChainDesc;	//	DevPP equivalent
	D3D_FEATURE_LEVEL		FeatureLevel;
#else
private:
	HINSTANCE 				hD3D;

public:

	ComPtr < IDirect3D9 > 				pD3D;		// D3D
	ComPtr < IDirect3DDevice9 >			pDevice;	// render device

	ComPtr < IDirect3DSurface9 >		pBaseRT;
	ComPtr < IDirect3DSurface9 >		pBaseZB;

	CHWCaps					Caps;

	UINT					DevAdapter;
	D3DDEVTYPE				DevT;
	D3DPRESENT_PARAMETERS	DevPP;
#endif	//	USE_DX10

#ifndef _MAYA_EXPORT
	stats_manager			stats_manager;
#endif
#if defined(USE_DX10) || defined(USE_DX11) || defined(USE_DX12)
	void					UpdateViews				( );
	DXGI_RATIONAL			selectRefresh			(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

	virtual	void			OnAppActivate			( );
	virtual void			OnAppDeactivate			( );
#endif	//	USE_DX10

#ifdef USE_DX12
    void CreateRootSignature();
#endif

private:
	bool					m_move_window;
};

extern ECORE_API CHW		HW;

#endif // !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
