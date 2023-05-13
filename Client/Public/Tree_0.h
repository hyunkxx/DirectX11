#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel_Instance;
END

BEGIN(Client)

class CTree_0 : public CGameObject
{
private:
	CTree_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTree_0(const CTree_0& rhs);
	virtual ~CTree_0() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	HRESULT DrawDefault();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Instance*	m_pModelCom = { nullptr };

private:
	_uint				m_iShaderPassID = { 0 };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT	SetUp_ShadowShaderResources();

public:
	static CTree_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END