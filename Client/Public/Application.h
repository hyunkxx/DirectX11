#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;

#ifdef _DEBUG
class CNavigation;
#endif // _DEBUG
END

BEGIN(Client)

class CApplication final: public CBase
{
private:
	explicit CApplication();
	virtual ~CApplication() = default;

public:
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	HRESULT Render();

private:
	HRESULT Open_Level(LEVEL_ID eLevelID);
	HRESULT Ready_Prototype_Static_Component();
	HRESULT Ready_Prototype_Static_GameObject();

private:
	HRESULT InitializeManager();
	void DestroyManager();
	
	HRESULT Ready_Static_Effect();
	HRESULT Add_Effect(HWND _hWnd , const _tchar* EffectTag , const char* TextureTag);
public:
	static CApplication* Create();
	virtual void Free() override;

private:
	short iTickCount = 0;

	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11DepthStencilView*		m_pDSV = { nullptr };

	CGameInstance*				m_pGameInstance = { nullptr };

	CRenderer*					m_pRenderer = { nullptr };

	class CAnimToolManager*		m_pAnimToolManager = { nullptr };

#ifdef _DEBUG
	CNavigation*				m_pNavigation = { nullptr };
	_bool						m_Render_Navigation = { false };
#endif // _DEBUG

#ifdef _DEBUG
	class CGUIManager*			m_pGUIManager = { nullptr };
#endif
	
};

END