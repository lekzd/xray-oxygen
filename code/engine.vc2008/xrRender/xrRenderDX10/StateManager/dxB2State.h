#pragma once

class SimulatorStates;

#include "dxB2SamplerStateCache.h"
#if !defined(USE_DX12)
class B1State
{
//	Public interface
public:
	B1State							();	//	These have to be private bu xr_new/xr_delete don't support this
	~B1State						();

	static B1State* Create( SimulatorStates& state_code );

	//	DX9 unified interface
	HRESULT		Apply				();
	void		Release				();

	//	DX10 specific
	void		UpdateStencilRef	( UINT Ref )	{ m_uiStencilRef = Ref; }
	void		UpdateAlphaRef		( UINT Ref )	{ m_uiAlphaRef = Ref; }

//	User restricted interface
private:
	typedef	dx10SamplerStateCache::HArray tSamplerHArray;

private:
	static void InitSamplers		( 
			tSamplerHArray& SamplerArray,
			SimulatorStates& state_code,
			int iBaseSamplerIndex
	);

private:
	//	All states are supposed to live along all application lifetime
	ID3DRasterizerState*		m_pRasterizerState;		//	Weak link
	ID3DDepthStencilState*		m_pDepthStencilState;	//	Weak link
	ID3DBlendState*				m_pBlendState;			//	Weak link

	tSamplerHArray				m_VSSamplers;
	tSamplerHArray				m_PSSamplers;
	tSamplerHArray				m_GSSamplers;
#ifdef USE_DX11
	tSamplerHArray				m_CSSamplers;
	tSamplerHArray				m_HSSamplers;
	tSamplerHArray				m_DSSamplers;
#endif

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

//	Private data
private:
};
#else
class B2State
{
	//	Public interface
public:
	B2State();	//	These have to be private bu xr_new/xr_delete don't support this
	~B2State();

	static B2State* Create(SimulatorStates& state_code);

	//	DX9 unified interface
	HRESULT		Apply();
	void		Release();

	//	DX10 specific
	void		UpdateStencilRef(UINT Ref) { m_uiStencilRef = Ref; }
	void		UpdateAlphaRef(UINT Ref) { m_uiAlphaRef = Ref; }

	//	User restricted interface
private:
	typedef	dx10SamplerStateCache::HArray tSamplerHArray;

private:
	static void InitSamplers(
		tSamplerHArray& SamplerArray,
		SimulatorStates& state_code,
		int iBaseSamplerIndex
	);

private:
	//	All states are supposed to live along all application lifetime
	ID3DRasterizerState * m_pRasterizerState;		//	Weak link
	ID3DDepthStencilState*		m_pDepthStencilState;	//	Weak link
	ID3DBlendState*				m_pBlendState;			//	Weak link

	tSamplerHArray				m_VSSamplers;
	tSamplerHArray				m_PSSamplers;
	tSamplerHArray				m_GSSamplers;
#ifdef USE_DX11
	tSamplerHArray				m_CSSamplers;
	tSamplerHArray				m_HSSamplers;
	tSamplerHArray				m_DSSamplers;
#endif

	UINT						m_uiStencilRef;
	UINT						m_uiAlphaRef;

	//	Private data
private:
};
#endif