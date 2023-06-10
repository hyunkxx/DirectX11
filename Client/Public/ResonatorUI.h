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

public:
	void BindUIChracter(class UICharacter* pUIChar) { m_pUICharacter = pUIChar; };

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
	_uint getWeaponText();

	HRESULT defaultSlotRender();
	void keyInput(_double TimeDelta);

	// Common
	HRESULT chooseCharacter();

	// State Detail
	HRESULT stateRender();
	HRESULT stateData();
	
	void selectCharacter(_double TimeDelta);
	void stateKeyInput(_double TimeDelta);
	void echoKeyInput(_double TimeDelta);
	void resonanceInput(_double TimeDelta);
	void forteInput(_double TimeDelta);

	void upgradeCharacter(_uint iCharacterType);

	void weaponKeyInput(_double TimeDelta);
	void pushUpgradeButton(_double TimeDelta);
	void upshSwitchButton(_double TimeDelta);
	void flashGemSlot(_double TimeDelta);

	// Weapon
	HRESULT weaponStateRender();
	HRESULT weaponSwitchRender();
	void upgradeWeapon();

	HRESULT echoSlotRender();
	HRESULT resonanceRender();
	HRESULT forteRender();

	void upgradeSkill(_double TimeDelta);
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
	class CCameraMovement* m_pCamMovement = nullptr;
	class UICharacter* m_pUICharacter = nullptr;
	class CEchoSystem* m_pEchoSystem = nullptr;

private: //Ortho Desc
	_float4x4 m_ViewMatrix, m_ProjMatrix;

private:
	ORTHO_DESC m_OrthoBackground;
	_float m_fBackgroundAlpha = 1.f;

	// Smooth UI
	enum { SMOOTH_MAX = 20 };
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

#pragma region Weapon
	enum { MAX_WEAPON_UPGRADE = 5};

	ORTHO_DESC m_OrthoWeaponName;
	ORTHO_DESC m_OrthoWeaponDesc; //무기 탭 설명
	ORTHO_DESC m_OrthoUpgradeSign[MAX_WEAPON_UPGRADE]; //강화 표식 0/5회
	ORTHO_DESC m_OrthoDamageBack;
	ORTHO_DESC m_OrthoCriDamageBack;

	// 데미지, 치명타 공격계수(퍼센트)
	enum { DAMAGE_MAX = 4 };
	ORTHO_DESC m_OrthoAttackText[2];
	ORTHO_DESC m_OrthoDamage[DAMAGE_MAX];
	ORTHO_DESC m_OrthoCriRate[DAMAGE_MAX];
	ORTHO_DESC m_OrthoWeaponUpgradeText;
	ORTHO_DESC m_OrthoWeaponUpgradeButton;
	ORTHO_DESC m_OrthoWeaponSwitchText;
	ORTHO_DESC m_OrthoWeaponSwitchButton;

	ORTHO_DESC m_OrthoUpgradeCostBack;
	ORTHO_DESC m_OrthoUpgradeCostText;
	ORTHO_DESC m_OrthoUpgradeOwnCostText;

	enum { GEM_DIGIT_MAX = 4};
	ORTHO_DESC m_OrthoGemIconBack;
	ORTHO_DESC m_OrthoGemIcon;
	ORTHO_DESC m_OrthoGemCost[GEM_DIGIT_MAX];
	ORTHO_DESC m_OrthoOwnGemCost[GEM_DIGIT_MAX];

	// Weapon Upgrade Cost
	_uint m_iOwnGem = 0;
	_uint m_iGemCost = 0;
	_float m_fSwitchWeaponAcc = 0.f;

	// Select Weapon
	ORTHO_DESC m_OrthoWeaponInvenBack;
	ORTHO_DESC m_OrthoSwitchBackpage;
	ORTHO_DESC m_OrthoSelectWeaponText;
	ORTHO_DESC m_OrthoWeaponSlot[20];
	ORTHO_DESC m_OrthoWeaponIcon[20];
	ORTHO_DESC m_OrthoWeaponUpgradePlus[20];
	ORTHO_DESC m_OrthoWeaponUpgradeCount[20];

	ORTHO_DESC m_OrthoSelectWeaponSlot;
	ORTHO_DESC m_OrthoSelectWeapon;
	ORTHO_DESC m_OrthoSelectWeaponName;
	ORTHO_DESC m_OrthoSelectWeaponUpgrade[MAX_WEAPON_UPGRADE];
	ORTHO_DESC m_OrthoSelectWeaponCriBack;
	ORTHO_DESC m_OrthoSelectWeaponAtkBack;
	ORTHO_DESC m_OrthoSelectWeaponAttack[6];
	ORTHO_DESC m_OrthoSelectWeaponCri[4];

	enum { BTN_NONE, BTN_ON, BTN_CLICK };
	ORTHO_DESC m_OrthoWeaponConfirmButton;
	ORTHO_DESC m_OrthoWeaponConfirmText;

	_bool m_bSwitchButton = false;
	_uint m_iSelectSlot = 0;
	_uint m_iConfirmButtonState = 0;
	_bool m_bWeaponChangeConfirm = false;
#pragma endregion

#pragma region Echos
	_float3 GetEchoGradeColor3(_uint iGrade);

	enum { ECHO_SLOT_MAX = 12 };
	ORTHO_DESC m_OrthoBindEcho;
	ORTHO_DESC m_OrthoBindEchoEmpty;
	ORTHO_DESC m_OrthoBindEchoIcon;
	ORTHO_DESC m_OrthoEchoSlot[ECHO_SLOT_MAX];
	ORTHO_DESC m_OrthoEchoIcon[ECHO_SLOT_MAX];
	ORTHO_DESC m_OrthoGradeImage[ECHO_SLOT_MAX][3];

	ORTHO_DESC m_OrthoEchoToolTip[ECHO_SLOT_MAX];
	ORTHO_DESC m_OrthoEchoToolTipName[ECHO_SLOT_MAX];
	ORTHO_DESC m_OrthoEchoToolTipAttack[ECHO_SLOT_MAX];
	ORTHO_DESC m_OrthoEchoToolTipDamage[ECHO_SLOT_MAX][3];

	ORTHO_DESC m_OrthoEchoBack[2]; // Back이미지
	ORTHO_DESC m_OrthoEchoInfo[2]; // 정보
	ORTHO_DESC m_OrthoEchoDamage[3];

	ORTHO_DESC m_OrthoUpgradeButton;// 업그레이드 버튼처리아직안함

	ORTHO_DESC m_OrthoRegisterButton;
	ORTHO_DESC m_OrthoRegisterText;
	_uint m_iRegisterBtnState = 0;
	_uint m_iUpgradeBtnState = 0;

	_uint m_iCurSelectEcho = 0; // 선택(클릭)된 에코슬롯

	//Echo Upgrade
	ORTHO_DESC m_OrthoEchoUpgradeBack;

	//Echo Upgrade Button
	ORTHO_DESC m_OrthoEchoUpgradeButton;
	ORTHO_DESC m_OrthoEchoUpgradeButtonText;
	_bool m_bUpgradeLimit = false;

	//Echo DogTag
	ORTHO_DESC m_OrthoDogTagCostText;
	ORTHO_DESC m_OrthoDogTagCostOwnText;
	ORTHO_DESC m_OrthoDogTagSlot;
	ORTHO_DESC m_OrthoDogTagIcon;
	_uint iDogTagIcon = STATIC_IMAGE::ICON_DOGTAG0;

	ORTHO_DESC m_OrthoDogTagCost[3];
	ORTHO_DESC m_OrthoDogTagCostOwn[4];

	//Tool Tip
	_uint m_iOnMouseEchoSlot = 0;
	_bool m_bToolTip[ECHO_SLOT_MAX] = { false, };
	_float m_fToolTipAcc[ECHO_SLOT_MAX] = { 0.f, };


#pragma endregion

#pragma region RESONANCE
	ORTHO_DESC m_OrthoActivateBack;
	ORTHO_DESC m_OrthoActivateText;

	ORTHO_DESC m_OrthoActivateCount[2];
	ORTHO_DESC m_OrthoActivateLimit[3];

	ORTHO_DESC m_OrthoSequenceSlot1[2];
	ORTHO_DESC m_OrthoSequenceSlot2[2];
	ORTHO_DESC m_OrthoSequenceSlot3[2];
	ORTHO_DESC m_OrthoSequenceSlot4[2];

	ORTHO_DESC m_OrthoSequenceText[4];

	ORTHO_DESC m_OrthoSequenceSlotSocket1[3];
	ORTHO_DESC m_OrthoSequenceSlotSocket2[3];
	ORTHO_DESC m_OrthoSequenceSlotSocket3[3];
	ORTHO_DESC m_OrthoSequenceSlotSocket4[3];

	ORTHO_DESC m_OrthoSequenceUpgradeBack;
	ORTHO_DESC m_OrthoSequenceCurSequenceText;
	ORTHO_DESC m_OrthoSQCostSlot;
	ORTHO_DESC m_OrthoSQCostIcon;
	ORTHO_DESC m_OrthoSQCost[2];
	ORTHO_DESC m_OrthoSQOwn[4];

	ORTHO_DESC m_OrthoSQUpgradeButton;
	ORTHO_DESC m_OrthoSQUpgradeButtonText;

	_bool m_bSQUpgradeClick = false;
	_uint iResonanceSocket[4] = { 0, };

	enum {SQ_1, SQ_2, SQ_3, SQ_4, SQ_NONE};
	_uint m_iSelectSQ = SQ_NONE;
	
#pragma endregion

#pragma region FORTE
	ORTHO_DESC m_OrthoForteBack;
	ORTHO_DESC m_OrthoAttackSlot[4];
	ORTHO_DESC m_OrthoBurstSlot;
	ORTHO_DESC m_OrthoQTESlot;

	ORTHO_DESC m_OrthoAttackIcon[4];
	ORTHO_DESC m_OrthoAttackNeedLevel[4];
	ORTHO_DESC m_OrthoBurstIcon;
	ORTHO_DESC m_OrthoQTEIcon;

	ORTHO_DESC m_OrthoSkillPointText;
	ORTHO_DESC m_OrthoSkillPoint[2];
	ORTHO_DESC m_OrthoSkillDesc;
	ORTHO_DESC m_OrthoSkillState;
	ORTHO_DESC m_OrthoSkillNeedDesc;

	ORTHO_DESC m_OrthoSkillUpgradeBtn;
	ORTHO_DESC m_OrthoSkillUpgradeText;

	enum SKILL_TYPE { ATTACK1, ATTACK2, ATTACK3, ATTACK4 , QTE, BURST, NONE};
	_uint m_iCurSelectSkill = ATTACK1;

	enum SKILL_STATE { LOCK, UNLOCK, ACTIVATE };
	SKILL_STATE m_eSkillState[3][NONE];

	_bool m_bUpgradeButtonClick = false;
#pragma endregion


private:
	_bool m_bActive = false;
	_bool m_bRender = false;


};

END