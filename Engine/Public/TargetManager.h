#pragma once

#include "Base.h"
#include "Graphic_Device.h"

BEGIN(Engine)

class CTargetManager final : public CBase
{
	DECLARE_SINGLETON(CTargetManager)

public:
	explicit CTargetManager() = default;
	virtual ~CTargetManager() = default;
	virtual void Free() override;

public:
	void SetupStaticShadowDepthStencilView(ID3D11DepthStencilView* pDepthStencil) {
		m_pStaticShadowDepthStencilView = pDepthStencil;
	};
	void SetupShadowDepthStencilView(ID3D11DepthStencilView* pDepthStencil) {
		m_pShadowDepthStencilView = pDepthStencil;
	};
	void SetupSmallDepthStencilView(ID3D11DepthStencilView* pDepthStencil) {
		m_pSmallDepthStencilView = pDepthStencil;
	};

	HRESULT AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTargetTag, _float iWidth, _float iHeight, DXGI_FORMAT eFormat, _float4 vColor);
	HRESULT AddMRT(const _tchar* pMRTTag, const _tchar* pTargetTag);

	HRESULT Begin(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	HRESULT SmallBegin(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	HRESULT ShadowBegin(ID3D11DeviceContext* pContext, const _tchar* pMRTTag);
	HRESULT BeginTarget(ID3D11DeviceContext* pContext, class CRenderTarget* pTarget);
	HRESULT End(ID3D11DeviceContext* pContext);

	HRESULT Set_ShaderResourceView(class CShader* pShader, const _tchar* pTargetTag, const char* pContantName);

	class CRenderTarget* FindTarget(const _tchar* pTargetTag);
private:
	class list<class CRenderTarget*>* FindMRT(const _tchar* pMRTTag);


#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
#endif
public:
	HRESULT Render(const _tchar* pMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	unordered_map<const _tchar*, class CRenderTarget*>		 m_RTVs;
	unordered_map<const _tchar*, list<class CRenderTarget*>> m_MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11DepthStencilView* m_pStaticShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView* m_pShadowDepthStencilView = nullptr;
	ID3D11DepthStencilView* m_pSmallDepthStencilView = nullptr;
};

END