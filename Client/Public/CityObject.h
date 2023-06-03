#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CCityObject : public CGameObject
{
public:
	enum TYPE_ID { ID_BUI, ID_ELE, ID_RAI, ID_GRO, ID_STA, ID_DES, ID_STO, ID_END };
	enum OPTION_ID { OP_DECO, OP_FLOOR, OP_INTERACT, OP_END };

private:
	CCityObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCityObject(const CCityObject& rhs);
	virtual ~CCityObject() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	HRESULT Render_Default();

public:
	void SetUp_State(_float3 vP, _float3 vS, _float3 vA);

public:
	SCITY_OBJECT_DESC Get_CityObject_ToolDesc() const { return m_CityObject_Desc; }
	void Set_CityObject_ToolDesc(SCITY_OBJECT_DESC CityObjectToolDesc) { m_CityObject_Desc = CityObjectToolDesc; }

private:
	SCITY_OBJECT_DESC			m_CityObject_Desc = {};

	_uint						m_iShaderPassID = { 0 };

private:
	CRenderer*					m_pRendererCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

	CCollider*					m_pCollider = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

	HRESULT	SetUp_ShadowShaderResources();

public:
	static CCityObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
