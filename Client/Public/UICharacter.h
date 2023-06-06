#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;

class CModel_Anim;
class CModel_VTF;

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

	_uint		m_iAnimID = { 0 };

	// 현재 선택된 탭 Index를 저장 (or 매 프레임 가져와서 조건 체크?)
	_uint		m_iUIState = { 0 };

};

END