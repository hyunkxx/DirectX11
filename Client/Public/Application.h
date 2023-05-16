#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;

#ifdef _DEBUG
class CNavigation;
#endif
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
	HRESULT Ready_Item_Image();
	HRESULT Ready_Item_Data();
	HRESULT Ready_UI_Data();

private:
	HRESULT InitializeManager();
	void DestroyManager();
	
	
	HRESULT Ready_Static_Effect();
	HRESULT Ready_Fonts();

#ifdef _DEBUG
private: /* For.Fps */
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumRender = { 0 };
	_double					m_TimeAcc = { 0.0 };
#endif

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


	class CItemDB*				m_pItemDB = { nullptr };
	class CAnimToolManager*		m_pAnimToolManager = { nullptr };
#ifdef _DEBUG
	class CGUIManager*			m_pGUIManager = { nullptr };
#endif
	
};

END