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
END

BEGIN(Client)

// AnimTool에서 플레이어 외의 모든 캐릭터들의 모델을 출력하고 편집하기 위한 클래스

class CLobbyCharacter : public CGameObject
{
public:
	enum STATE
	{
		STATE_IDLE,
		STATE_SELECTED,
		STATE_END
	};

private:
	CLobbyCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobbyCharacter(const CLobbyCharacter& rhs);
	virtual ~CLobbyCharacter() = default;

public:
	virtual HRESULT	Initialize_Prototype(_uint iModelID, _uint iAnimID);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };

private:
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
	static CLobbyCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iModelID, _uint iAnimID);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
};

END