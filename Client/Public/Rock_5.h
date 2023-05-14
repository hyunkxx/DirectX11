#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel_Instance;
class CTexture;
END

BEGIN(Client)

class CRock_5 : public CGameObject
{
public:
	enum ROCK_DIFFUSE_KINDS { RD_1, RD_2, RD_3, RD_4, RD_END };

private:
	CRock_5(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRock_5(const CRock_5& rhs);
	virtual ~CRock_5() = default;

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
	HRESULT Load_DiffuseTexID(const _tchar* pDiffuseTexIDFilePath);

private:
	SOBJECT_EDITION_DESC	m_EditionDesc = {};
	_uint					m_iDiffuseTex_ID = { 0 };

	_float					m_fCullingRatio = { 0.0f };

	_uint					m_iShaderPassID = { 0 };

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Instance*	m_pModelCom = { nullptr };
	CTexture*			m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_END] = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT	SetUp_ShadowShaderResources();

public:
	static CRock_5* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END