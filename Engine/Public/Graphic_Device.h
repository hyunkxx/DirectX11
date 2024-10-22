#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	explicit CGraphic_Device() = default;
	virtual ~CGraphic_Device() = default;

public:
	enum VIEWPORT_TYPE { VIEWPORT_DEFAULT, VIEWPORT_SHADOWDEPTH, VIEWPORT_SMALL, VIEWPORT_MIDDLE, VIEWPORT_END };

	ID3D11DepthStencilView* GetStaticShadowDepthStencilView() const {
		return m_pStaticShadowDepthStencilView;
	}
	ID3D11DepthStencilView* GetShadowDepthStencilView() const {
		return m_pShadowDepthStencilView;
	}
	ID3D11DepthStencilView* GetSmallDepthStencilView() const {
		return m_pSmallDepthStencilView;
	}
	ID3D11DepthStencilView* GetMiddleDepthStencilView() const {
		return m_pMiddleDepthStencilView;
	}

	HRESULT Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out);
	HRESULT Clear_RenderTargetView(_float4 vClearColor);
	HRESULT Clear_DepthStencilView();
	HRESULT Clear_ShadowDepthStencilView();
	HRESULT Clear_SmallDepthStencilView();
	HRESULT Present();

	const D3D11_VIEWPORT* GetViewport(VIEWPORT_TYPE eViewportType) const { return &viewPortDesc[eViewportType]; };
private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_RenderTargetView();
	HRESULT Ready_DepthStencilView(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_StaticShadowDepthStencilView(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_ShadowDepthStencilView(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_SmallDepthStencilView(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_MiddleDepthStencilView(_uint iWinSizeX, _uint iWinSizeY);

public:
	virtual void Free() override;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	IDXGISwapChain*				m_pSwapChain = { nullptr };

	/* pRTV & pDSV */
	ID3D11RenderTargetView*		m_pRenderTargetView = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };

	ID3D11DepthStencilView*		m_pStaticShadowDepthStencilView = { nullptr };
	ID3D11DepthStencilView*		m_pShadowDepthStencilView = { nullptr };
	ID3D11DepthStencilView*		m_pSmallDepthStencilView = { nullptr };
	ID3D11DepthStencilView*		m_pMiddleDepthStencilView = { nullptr };

	D3D11_VIEWPORT viewPortDesc[VIEWPORT_END];
};

END