#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
END

BEGIN(Client)

class CModelTest_Dynamic : public CGameObject
{
private:
	CModelTest_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelTest_Dynamic(const CModelTest_Dynamic& rhs);
	virtual ~CModelTest_Dynamic() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };


private:
	_uint				m_iAnimID = { 0 };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	void SetUp_Animation();

public:
	static CModelTest_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
};

END