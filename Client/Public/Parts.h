#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CParts : public CGameObject
{
public:
	enum ParentID
	{
		PARENT_BACK,
		PARENT_RHAND,
		PARENT_LHAND,
	};
public:
	typedef struct tagPartsDesc {
		CBone** ppParentBone;
		CTransform* pParentTransform;
	}PARTSDESC;

private:
	CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParts(const CParts& rhs);
	virtual ~CParts() = default;

public:
	virtual HRESULT	Initialize_Prototype(_uint iModelID);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

	void Set_Parent(_uint iParentID)
	{
		m_iCurParent = iParentID;
	}

	void Set_Render(_bool bRender)
	{
		m_bRender = bRender;
	}


private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

private:
	_uint				m_iModelID;
	PARTSDESC			m_tPartsDesc;
	_uint				m_iCurParent;
	_float4x4			m_WorldMatrix;
	_bool				m_bRender = { true };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iModelID);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END