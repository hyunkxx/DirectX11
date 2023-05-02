#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Instance_Tree1;
END

BEGIN(Client)

class CTree_1 : public CGameObject
{
private:
	CTree_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTree_1(const CTree_1& rhs);
	virtual ~CTree_1() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	HRESULT DrawDefault();

private:
	CRenderer*				m_pRendererCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CModel_Instance_Tree1*	m_pModelCom = { nullptr };

private:
	_uint				m_iAnimID = { 0 };
	_uint				m_iShaderPassID = { 0 };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT	SetUp_ShadowShaderResources();

public:
	static CTree_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END