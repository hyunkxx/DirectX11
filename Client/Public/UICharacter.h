#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;

class CModel_Anim;
class CModel_VTF;
class CVIBuffer_Rect;

END

BEGIN(Client)

class UICharacter final : public CGameObject
{
public:
	enum Animations
	{
		UICHAR_IDLE,
		UICHAR_CHIP_END,
		UICHAR_CHIP_LOOP,
		UICHAR_CHIP_START,
		UICHAR_INTEN_END,
		UICHAR_INTEN_LOOP,
		UICHAR_INTEN_START,
		UICHAR_INTO1_END,
		UICHAR_INTO1_LOOP,
		UICHAR_INTO1_START,
		UICHAR_RESONANT_END,
		UICHAR_RESONANT_LOOP,
		UICHAR_RESONANT_START,
		UICHAR_SKILL_START,
		UICHAR_WEAPON_END,
		UICHAR_WEAPON_LOOP,
		UICHAR_WEAPON_START,
		UICHAR_END
	};

	enum RIB_Animations
	{
		UICHAR_RIB_IDLE,
		UICHAR_RIB_CHIP_END,
		UICHAR_RIB_CHIP_LOOP,
		UICHAR_RIB_CHIP_START,
		UICHAR_RIB_INTO1_END,
		UICHAR_RIB_INTO1_LOOP,
		UICHAR_RIB_INTO1_START,
		UICHAR_RIB_RESONANT_END,
		UICHAR_RIB_RESONANT_LOOP,
		UICHAR_RIB_RESONANT_START,
		UICHAR_RIB_WEAPON_END,
		UICHAR_RIB_WEAPON_LOOP,
		UICHAR_RIB_WEAPON_START,
		UICHAR_RIB_END
	};

private:
	explicit UICharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UICharacter(const UICharacter& rhs);
	virtual ~UICharacter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

public:
	enum UIANIMATION { UI_STATE, UI_WEAPON, UI_ECHO, UI_RESONANCE, UI_WUTHERIDE };
	void SetAnimation(UIANIMATION eCurUI);

	void ExitAnimation(UIANIMATION eCurUI);

private:
	void initAnimation();
	void updateAnimationState(_double TimeDelta);

	HRESULT addComponents();
	HRESULT setupShaderResource();

public:
	static UICharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;
	CModel_VTF*	m_pModel = nullptr;
	CModel_Anim* m_pAnimSetBase = nullptr;
	CModel_Anim* m_pAnimSetRibbon = nullptr;

	_uint m_iAnimID = { 0 };
	_uint m_iRibbonID = { 0 };
	UIANIMATION m_eUIState = UI_STATE;

	// Studio
	_bool m_bStateBase = false;
	CShader* m_pUIShader = nullptr;
	CTransform* m_pStudioTransform = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;

private:
	class CCameraMovement* m_pCamMovement = nullptr;
	class CUICam* m_pUICam = nullptr;
};

END