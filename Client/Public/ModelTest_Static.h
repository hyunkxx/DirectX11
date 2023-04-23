#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "IAlphaSort.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class CModelTest_Static 
	: public CGameObject
{
private:
	CModelTest_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelTest_Static(const CModelTest_Static& rhs);
	virtual ~CModelTest_Static() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	_uint				m_iAnimID = { 0 };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CModelTest_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END