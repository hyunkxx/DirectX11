#pragma once

#include "UIInterface.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;

END

BEGIN(Client)

class CTerminalUI final 
	: public CGameObject
	, public IActivate
{
public:
	enum SLOT_TEXTURE { SLOT_ICON, SLOT_TEXT };
	enum { SLOT_RESONATORS, SLOT_BACKPACK, SLOT_TERMINAL, SLOT_TEAM, SLOT_MAP, SLOT_SETTING, SLOT_MAX };
	enum TERMINAL_UI { 
		TERMINAL_MAINFRAME, TERMINAL_LEFTCENTER, TERMINAL_LEFTSTAR, TERMINAL_LEFTHALF_OUT, TERMINAL_LEFTHALF_IN
		, TERMINAL_PLAYERIMAGE, TERMINAL_RANKTEXT, TERMINAL_RANKSUBTEXT, TERMINAL_LEVELTEXT, TERMINAL_LEVELSUBTEXT
		, TERMINAL_LEVELEXPGAGE,TERMINAL_LEVELEXPTEXT, TERMINAL_ID, TERMINAL_END };
	
public:
	explicit CTerminalUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerminalUI(const CTerminalUI& rhs);
	virtual ~CTerminalUI() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI() override;

public:
	virtual void SetActive(_bool bValue) { m_bMainActive = bValue; };
	virtual void SetRender(_bool bValue) { m_bRender = bValue; };

public:
	_bool IsActive() const { return m_bMainActive; }
	_uint AddActiveCount() { m_iActiveUICount++; }
	void PushActiveUI(struct IActivate* pActivateUI);
	void ManualShutDown();

	void SetupMouseUI(class CUI_Mouse* pMouseUI) { m_pMouse = pMouseUI; };

private:
	HRESULT addComponents();
	HRESULT addGameObjects();

	void inputKey(_double TimeDelta);
	void shutdownUI(_double TimeDelta);
	void activeCheck(_double TimeDelta);
	_bool activeDraw(TERMINAL_UI eTerminal, _double TimeDelta);
	void smoothScaleup(TERMINAL_UI eTerminal, _double TimeDelta);
	void activeSlot(_double TimeDelta);
	_int getSlotTexture(SLOT_TEXTURE eType, _int iIndex);

private: // Render
	HRESULT renderMainFrame();
	HRESULT renderLeftPivot();
	HRESULT renderSlots();	

public:
	static CTerminalUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private://Components
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;

private://GameObjects
	class CPlayerState* m_pCharacterState = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;

	class CGameObject* m_pSlotUI[SLOT_MAX];
	stack<struct IActivate*> m_pActivateList;
private: //Ortho Desc
	_float4x4 m_ViewMatrix, m_ProjMatrix;	

private:
	_bool m_bRender = false;

	_bool m_bMainActive = false;
	_bool m_bActive[TERMINAL_END];

	// MainFrame
	ORTHO_DESC m_OrthoDesc[TERMINAL_END];
	ORTHO_DESC m_OrthoLevel[2];
	ORTHO_DESC m_OrthoUnionExp[3];

	_float m_fActiveTimeAcc = 0.f;
	_float m_fAlpha[TERMINAL_END];
	_float m_fScaleAcc[TERMINAL_END];

	// Slots
	ORTHO_DESC m_OrthoSlot[SLOT_MAX];
	ORTHO_DESC m_OrthoSlotIcon[SLOT_MAX];
	ORTHO_DESC m_OrthoSlotText[SLOT_MAX];

	_bool m_bSlotActive[SLOT_MAX];
	_bool m_bSlotOnMouse[SLOT_MAX];
	_float m_fSlotAlpha[SLOT_MAX];

	class CUI_Mouse* m_pMouse = nullptr;

private:
	_uint m_iActiveUICount = 0;

private: // Test UnionExp
	_float m_fTestTimer = 0.f;
	_int m_iCount = 1;

};

END