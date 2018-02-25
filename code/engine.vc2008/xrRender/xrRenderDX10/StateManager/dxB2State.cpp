#include "stdafx.h"
#include "dxB2State.h"

//#include "dx10RSManager.h"
#include "dxB2StateCache.h"
#if !defined(USE_DX12)
B1State::B1State() : 
	m_pRasterizerState(0),
	m_pDepthStencilState(0),
	m_pBlendState(0),
	m_uiStencilRef(UINT(-1)),
	m_uiAlphaRef(0)
{
}

B1State::~B1State()
{
	//	m_pRasterizerState is a weak link
	//	m_pDepthStencilState is a weak link
	//	m_pBlendState is a weak link
}

B1State* B1State::Create(SimulatorStates& state_code)
{
	B1State	*pState = xr_new<B1State>();

	state_code.UpdateState(*pState);

	pState->m_pRasterizerState = RSManager.GetState(state_code);
	pState->m_pDepthStencilState = DSSManager.GetState(state_code);
	pState->m_pBlendState = BSManager.GetState(state_code);
	//ID3DxxDevice::CreateSamplerState

	//	Create samplers here
	{
		InitSamplers( pState->m_VSSamplers, state_code, CTexture::rstVertex);
		InitSamplers( pState->m_PSSamplers, state_code, CTexture::rstPixel);
		InitSamplers( pState->m_GSSamplers, state_code, CTexture::rstGeometry);
#if defined(USE_DX11) || defined(USE_DX12)
		InitSamplers( pState->m_HSSamplers, state_code, CTexture::rstHull);
		InitSamplers( pState->m_DSSamplers, state_code, CTexture::rstDomain);
		InitSamplers( pState->m_CSSamplers, state_code, CTexture::rstCompute);
#endif
	}


	return pState;
}

HRESULT B1State::Apply()
{
	VERIFY(m_pRasterizerState);
	StateManager.SetRasterizerState(m_pRasterizerState);
	VERIFY(m_pDepthStencilState);
	StateManager.SetDepthStencilState(m_pDepthStencilState);
	if( m_uiStencilRef != -1 )
		StateManager.SetStencilRef(m_uiStencilRef);
	VERIFY(m_pBlendState);
	StateManager.SetBlendState(m_pBlendState);
	StateManager.SetAlphaRef(m_uiAlphaRef);

	SSManager.GSApplySamplers(m_GSSamplers);
	SSManager.VSApplySamplers(m_VSSamplers);
	SSManager.PSApplySamplers(m_PSSamplers);
#if defined(USE_DX11) || defined(USE_DX12)
	SSManager.HSApplySamplers(m_HSSamplers);
	SSManager.DSApplySamplers(m_DSSamplers);
	SSManager.CSApplySamplers(m_CSSamplers);
#endif // USE_DX12


//	static const FLOAT BlendFactor[4] = {0.000f, 0.000f, 0.000f, 0.000f};
//	static const UINT SampleMask = 0xffffffff;

//	VERIFY(m_pRasterizerState);
//	HW.pDevice->RSSetState(m_pRasterizerState);
//	VERIFY(m_pDepthStencilState);
//	HW.pDevice->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
//	VERIFY(m_pBlendState);
//	HW.pDevice->OMSetBlendState(m_pBlendState, BlendFactor, SampleMask);

	return S_OK;
}

void B1State::Release()
{
	B1State	*pState = this;
	xr_delete<B1State> (pState);
}

void B1State::InitSamplers( tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
	D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];

	for (int i=0; i<D3D_COMMONSHADER_SAMPLER_SLOT_COUNT; ++i)
	{
		SamplerUsed[i] = false;
		dx10StateUtils::ResetDescription(descArray[i]);
	}

	state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

	int iMaxSampler = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
	for ( ;iMaxSampler>-1; --iMaxSampler)
	{
		if (SamplerUsed[iMaxSampler])
			break;
	}

	if (iMaxSampler>-1)
	{
		SamplerArray.reserve(iMaxSampler+1);
		for ( int i=0; i<=iMaxSampler; ++i )
		{
			if (SamplerUsed[i])
				SamplerArray.push_back(SSManager.GetState(descArray[i]));
			else
				SamplerArray.push_back(u32(dx10SamplerStateCache::hInvalidHandle));
		}
	}
}
#else
B2State::B2State() :
	m_pRasterizerState(0),
	m_pDepthStencilState(0),
	m_pBlendState(0),
	m_uiStencilRef(UINT(-1)),
	m_uiAlphaRef(0)
{
}

B2State::~B2State()
{
	//	m_pRasterizerState is a weak link
	//	m_pDepthStencilState is a weak link
	//	m_pBlendState is a weak link
}

B2State* B2State::Create(SimulatorStates& state_code)
{
	B2State	*pState = xr_new<B2State>();

	state_code.UpdateState(*pState);

	pState->m_pRasterizerState = RSManager.GetState(state_code);
	pState->m_pDepthStencilState = DSSManager.GetState(state_code);
	pState->m_pBlendState = BSManager.GetState(state_code);
	//ID3DxxDevice::CreateSamplerState

	//	Create samplers here
	{
		InitSamplers(pState->m_VSSamplers, state_code, CTexture::rstVertex);
		InitSamplers(pState->m_PSSamplers, state_code, CTexture::rstPixel);
		InitSamplers(pState->m_GSSamplers, state_code, CTexture::rstGeometry);
#if defined(USE_DX11) || defined(USE_DX12)
		InitSamplers(pState->m_HSSamplers, state_code, CTexture::rstHull);
		InitSamplers(pState->m_DSSamplers, state_code, CTexture::rstDomain);
		InitSamplers(pState->m_CSSamplers, state_code, CTexture::rstCompute);
#endif
	}


	return pState;
}

HRESULT B2State::Apply()
{
	VERIFY(m_pRasterizerState);
	StateManager.SetRasterizerState(m_pRasterizerState);
	VERIFY(m_pDepthStencilState);
	StateManager.SetDepthStencilState(m_pDepthStencilState);
	if (m_uiStencilRef != -1)
		StateManager.SetStencilRef(m_uiStencilRef);
	VERIFY(m_pBlendState);
	StateManager.SetBlendState(m_pBlendState);
	StateManager.SetAlphaRef(m_uiAlphaRef);

	SSManager.GSApplySamplers(m_GSSamplers);
	SSManager.VSApplySamplers(m_VSSamplers);
	SSManager.PSApplySamplers(m_PSSamplers);
#if defined(USE_DX11) || defined(USE_DX12)
	SSManager.HSApplySamplers(m_HSSamplers);
	SSManager.DSApplySamplers(m_DSSamplers);
	SSManager.CSApplySamplers(m_CSSamplers);
#endif // USE_DX12


	//	static const FLOAT BlendFactor[4] = {0.000f, 0.000f, 0.000f, 0.000f};
	//	static const UINT SampleMask = 0xffffffff;

	//	VERIFY(m_pRasterizerState);
	//	HW.pDevice->RSSetState(m_pRasterizerState);
	//	VERIFY(m_pDepthStencilState);
	//	HW.pDevice->OMSetDepthStencilState(m_pDepthStencilState, m_uiStencilRef);
	//	VERIFY(m_pBlendState);
	//	HW.pDevice->OMSetBlendState(m_pBlendState, BlendFactor, SampleMask);

	return S_OK;
}

void B2State::Release()
{
	B2State	*pState = this;
	xr_delete<B2State>(pState);
}

void B2State::InitSamplers(tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
	D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
	bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];

	for (int i = 0; i < D3D_COMMONSHADER_SAMPLER_SLOT_COUNT; ++i)
	{
		SamplerUsed[i] = false;
		dx10StateUtils::ResetDescription(descArray[i]);
	}

	state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

	int iMaxSampler = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
	for (; iMaxSampler > -1; --iMaxSampler)
	{
		if (SamplerUsed[iMaxSampler])
			break;
	}

	if (iMaxSampler > -1)
	{
		SamplerArray.reserve(iMaxSampler + 1);
		for (int i = 0; i <= iMaxSampler; ++i)
		{
			if (SamplerUsed[i])
				SamplerArray.push_back(SSManager.GetState(descArray[i]));
			else
				SamplerArray.push_back(u32(dx10SamplerStateCache::hInvalidHandle));
		}
	}
}
#endif