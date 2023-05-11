#pragma once
#include "Client_Defines.h"
#include "TestChar.h"
#include "Renderer.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CModel_VTF;
class CBone;

class CTexture;
END

BEGIN(Client)

class CLobbyCharacter : public CGameObject
{
public:
	enum STATE
	{
		STATE_SELECTED,
		STATE_IDLE,
		STATE_END
	};

	enum { LEFT_MODEL, RIGHT_MODEL };

private:
	CLobbyCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobbyCharacter(const CLobbyCharacter& rhs);
	virtual ~CLobbyCharacter() = default;

public:
	virtual HRESULT	Initialize_Prototype(_uint iModelID, _uint iAnimID, _uint iModelType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

public:
	void PlaySelectedAnimation();
	void SetMouseInRect(_bool bValue) { m_bOnMoused = bValue; };
	_vector GetEyePosition();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
	_uint m_iModelType = 0;

	_uint m_iModelID = { 0 };
	_uint m_iAnimID = { 0 };
	// State
	STATE	m_eState = { STATE_IDLE };

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void Tick_State(_double TimeDelta);

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CLobbyCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iModelID, _uint iAnimID, _uint iModelType);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	_bool m_bOnMoused = false;

	CBone* m_pEyeBone = nullptr;
	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;
};

END