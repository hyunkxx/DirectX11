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

	void stateActive();
	void stateDisable();
	void stateUpdate(_double TimeDelta);
	_uint getCurSlotText(RESONATOR_BTN eButton);
	_uint getCurSlotIcon(RESONATOR_BTN eButton);
	_uint getStateImage(_uint iIndex);

	HRESULT defaultSlotRender();
	HRESULT stateRender();
	HRESULT stateData();

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

private: //Ortho Desc
	_float4x4 m_ViewMatrix, m_ProjMatrix;

private:
	ORTHO_DESC m_OrthoBackground;
	_float m_fBackgroundAlpha = 1.f;

	// Title Text
	ORTHO_DESC m_OrthoTitleText;

	// Button
	RESONATOR_BTN m_eCurButton = BTN_STATE;
	ORTHO_DESC m_OrthoButtons[BTN_END];
	ORTHO_DESC m_OrthoButtonIcons[BTN_END];
	_float m_fButtonAlpha[BTN_END];
	_bool m_bButtonActive[BTN_END];
	
	// Stats Detail
	//title name level

	// State
	enum { STATE_HP, STATE_ATTACK, STATE_DEFENSE, STATE_CRITRATE, STATE_END };
	ORTHO_DESC m_OrthoState[STATE_END];				//능력치 배경
	ORTHO_DESC m_OrthoStateData[STATE_END][6];		//능력치 수치

private:
	_bool m_bActive = false;
	_bool m_bRender = false;

};

END