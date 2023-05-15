#pragma once
#include "Client_Defines.h"
#include "TestChar.h"
#include "Renderer.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
END

BEGIN(Client)

class CSandbag 
	: public CCharacter
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
private:
	CSandbag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSandbag(const CSandbag& rhs);
	virtual ~CSandbag() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CSandbag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CSandbag::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CSandbag::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CSandbag::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;
};

END