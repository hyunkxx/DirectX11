#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel_Instance;
END

BEGIN(Client)

class CTree_12 : public CGameObject
{
private:
	CTree_12(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTree_12(const CTree_12& rhs);
	virtual ~CTree_12() = default;

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

public:
	HRESULT Load_EditionColor(const _tchar* pEditionColorFilePath);

private:
	SOBJECT_EDITION_DESC	m_EditionDesc = {};
	_bool					m_UseEditionColor = { false };
	_uint					m_iEditionColor_MeshNum = { 0 };
	_float3					m_vEditionColor = { 0.0f, 0.0f, 0.0f };

	_float					m_fCullingRatio = { 0.0f };

	_uint					m_iShaderPassID = { 0 };

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Instance*	m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT	SetUp_ShadowShaderResources();

public:
	static CTree_12* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END