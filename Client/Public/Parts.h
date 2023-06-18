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

	enum DissolveType
	{
		DISS_XLINEAR,
		DISS_TEX,
		DISS_END
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
	virtual HRESULT RenderShadow();

	void Set_Parent(_uint iParentID)
	{
		m_iCurParent = iParentID;
	}

	void Set_Render(_bool bRender)
	{
		m_bDissolve = false;
		m_bRender = bRender;
	}

	_bool Get_Render()
	{
		return m_bRender;
	}

	_bool Get_Dissolve()
	{
		return m_bDissolve;
	}

	void Set_Outline(_bool bValue)
	{
		m_bOutline = bValue;
	}

	_float4x4* Get_WorldMatrix()
	{
		return &m_WorldMatrix;
	}

	void Set_DissolveColor(_float3& vDissolve)
	{
		m_vDissolveColor = vDissolve;
	}

	void Start_Dissolve(_bool bDissType, _float fDissSpeed, _bool bUseTex)
	{
		if (true == bUseTex)
			m_eDissolveType = DISS_TEX;
		else
			m_eDissolveType = DISS_XLINEAR;

		m_fDissolveSpeed = fDissSpeed;
		m_bDissolveType = bDissType;


		m_bRender = true;
		m_bDissolve = true;
		m_fDissolveTimeAcc = 0.f;
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
	_bool				m_bOutline = { false };
	
	// 임시 디졸브 벨류
	_float				m_fDissolveAmount = 0.f;
	_float				m_fDissolveTimeAcc = 0.f;
	_bool				m_bDissolve = false;
	DissolveType		m_eDissolveType = DISS_XLINEAR;
	_bool				m_bDissolveType = false;
	_float				m_fDissolveSpeed = 1.f;
	_float3				m_vDissolveColor = {};

	// 로컬 정점 Position XYZ 최소/최대값
	_float3				m_vMinPoint = {};
	_float3				m_vMaxPoint = {};

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CParts* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iModelID);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END