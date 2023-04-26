#include "..\Public\TargetManager.h"
#include "RenderTarget.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CTargetManager)

void CTargetManager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RTVs)
		Safe_Release(Pair.second);

	m_RTVs.clear();
}

HRESULT CTargetManager::AddRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTargetTag, _float iWidth, _float iHeight, DXGI_FORMAT eFormat, _float4 vColor)
{
	if (nullptr != FindTarget(pTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(pDevice, pContext, (_uint)iWidth, (_uint)iHeight, eFormat, vColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RTVs.emplace(pTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::AddMRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CRenderTarget* pRenderTarget = FindTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*> MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTargetManager::Begin(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;
	
	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8];
	_uint iViewCount = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iViewCount++] = pRenderTarget->Get_RTV();
	}

	pContext->OMSetRenderTargets(iViewCount, pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::ShadowBegin(ID3D11DeviceContext * pContext, const _tchar * pMRTTag)
{
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8];
	_uint iViewCount = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iViewCount++] = pRenderTarget->Get_RTV();
	}
	
	CGraphic_Device* pGrahicDevice = CGraphic_Device::GetInstance();
	pContext->OMSetRenderTargets(iViewCount, pRenderTargets, m_pShadowDepthStencilView);
	pContext->RSSetViewports(1, pGrahicDevice->GetViewport(CGraphic_Device::VIEWPORT_TYPE::VIEWPORT_SHADOWDEPTH));

	return S_OK;
}

HRESULT CTargetManager::End(ID3D11DeviceContext * pContext)
{
	CGraphic_Device* pGrahicDevice = CGraphic_Device::GetInstance();
	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);
	pContext->RSSetViewports(1, pGrahicDevice->GetViewport(CGraphic_Device::VIEWPORT_TYPE::VIEWPORT_DEFAULT));

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

HRESULT CTargetManager::Set_ShaderResourceView(CShader * pShader, const _tchar * pTargetTag, const char * pContantName)
{
	if (nullptr == pShader)
		return E_FAIL;

	CRenderTarget* pTarget = FindTarget(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Set_ShaderResourceView(pShader, pContantName);
}

CRenderTarget * CTargetManager::FindTarget(const _tchar * pTargetTag)
{
	auto iter = find_if(m_RTVs.begin(), m_RTVs.end(), CTagFinder(pTargetTag));
	if (iter == m_RTVs.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CTargetManager::FindMRT(const _tchar * pMRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

#ifdef _DEBUG
HRESULT CTargetManager::Ready_Debug(const _tchar * pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pTarget = FindTarget(pTargetTag);
	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTargetManager::Render(const _tchar * pMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = FindMRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
		pTarget->Render(pShader, pVIBuffer);

	return S_OK;
}
#endif