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

class CResonatorUI 
	: public CGameObject
	, public IOnClick
	, public IActivate
{
public:
	enum RESONATOR_BTN { BTN_STATE, BTN_WEAPON, BTN_ECHO, BTN_RESONANCE, BTN_WUTERIDE, BTN_END };

public:
	explicit CResonatorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CResonatorUI(const CResonatorUI& rhs);
	virtual ~CResonatorUI() = default;

public://GameObject
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI() override;

public://Interface
	virtual void OnClick() override;

	virtual void SetActive(_bool bValue) override;
	virtual void SetRender(_bool bValue) override;
private:
	HRESULT addComponents();

	void elemAlphaUpdate(_double TimeDelta);
	void elemAlphaReset(_uint iStartIndex = 0);

	void stateActive();
	void stateDisable();
	void stateUpdate(_double TimeDelta);
	_uint getCurSlotText(RESONATOR_BTN eButton);
	_uint getCurSlotIcon(RESONATOR_BTN eButton);
	_uint getStateImage(_uint iIndex);

	HRESULT defaultSlotRender();
	void keyInput(_double TimeDelta);

	// State Detail
	HRESULT stateRender();
	HRESULT stateData();
	void stateKeyInput(_double TimeDelta);
	void upgradeCharacter(_uint iCharacterType);

	// 

public:
	static CResonatorUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;

private:
	class CPlayerState* m_pPlayerState = nullptr;
	class CTerminalUI* m_pTerminal = nullptr;
	class CInventory* m_pInven = nullptr;
private: //Ortho Desc
	_float4x4 m_ViewMatrix, m_ProjMatrix;

private:
	ORTHO_DESC m_OrthoBackground;
	_float m_fBackgroundAlpha = 1.f;

	// Smooth UI
	enum { SMOOTH_MAX = 10 };
	_float m_fElemAlpha[SMOOTH_MAX];
	_bool m_bElemAlphaStart[SMOOTH_MAX];

	// Title Text
	ORTHO_DESC m_OrthoTitleText;

	// Button
	RESONATOR_BTN m_eCurButton = BTN_STATE;
	ORTHO_DESC m_OrthoButtons[BTN_END];
	ORTHO_DESC m_OrthoButtonIcons[BTN_END];
	_float m_fButtonAlpha[BTN_END];
	_bool m_bButtonActive[BTN_END];
	
#pragma region StateDetail
	//title name level
	ORTHO_DESC m_OrthoPlayerText;
	ORTHO_DESC m_OrthoLevelText;
	ORTHO_DESC m_OrthoLevel[5];
	ORTHO_DESC m_OrthoExp;

	// State
	enum { STATE_HP, STATE_ATTACK, STATE_DEFENSE, STATE_CRITRATE, STATE_END };
	ORTHO_DESC m_OrthoState[STATE_END];				//능력치 배경
	ORTHO_DESC m_OrthoStateData[STATE_END][6];		//능력치 수치

	// Character Icons
	_uint m_iCurCharacter = 0;
	ORTHO_DESC m_OrthoCharBack[3];
	ORTHO_DESC m_OrthoCharIcon[3];

	// Upgrade
	ORTHO_DESC m_OrthoUpgradeSlot[4];
	ORTHO_DESC m_OrthoUpgradeCapsule[4];

	// Capsule Amount ,, Flash
	ORTHO_DESC m_OrthoCapsuleAmount[4][4]; //수량 4자리수
	_uint m_iUseAmount[4] = { 0, };
	_bool m_bFlashStart[4] = { false, };
	_float m_fFlashCount[4] = {0.f, };
	_float m_fFlashAcc[4] = { 0.f, };
	_bool m_bSlotRenderFinish = false; //Alpha 1될떄 Flash

	// Upgrade Button
	ORTHO_DESC m_OrthoUpgradeText;
	ORTHO_DESC m_OrthoUpgradeBtn;
	_float m_fPushAcc = 0.f;
	_bool m_bUpgradeConfirm = false;

	// Consume & Own Text (Coin)
	ORTHO_DESC m_OrthoCoinBack;
	ORTHO_DESC m_OrthoCoinIcon;
	ORTHO_DESC m_OrthoConsumeText;
	ORTHO_DESC m_OrthoOwnText;

	// Need Coin
	enum { DRAW_MAX = 8 };
	_uint m_iCurCoin = 0;
	_uint m_iTotalConsume = 0;
	_float m_fTotalExp = 0.f;
	ORTHO_DESC m_OrthoConsumeCoin[DRAW_MAX];
	ORTHO_DESC m_OrthoOwnCoin[DRAW_MAX];

#pragma endregion

private:
	_bool m_bActive = false;
	_bool m_bRender = false;

};

END