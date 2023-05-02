#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMesh_Effect_P  final : public CEffect
{

public:
	enum Effect_Texture_Type
	{
		TEX_NOISE, TEX_DISSOLVE, EFFECT_TEX_END
	};

protected:
	CMesh_Effect_P(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Effect_P(const CMesh_Effect_P& rhs);
	virtual ~CMesh_Effect_P() = default;

public:
	virtual	HRESULT				Initialize_Prototype(const char* pFileTag , const EFFECT_DESC& MeshDesc);
	virtual	HRESULT				Initialize(void* pArg)override;
	virtual void				Tick(_double TimeDelta)override;
	virtual void				LateTick(_double TimeDelta)override;
	virtual HRESULT				Render()override;
	virtual void				RenderGUI() override;

	virtual void				Play_Effect(_float4x4* pWorldMatrix, _bool bTracking = false)override;
	virtual	EFFECT_DESC			Get_Effect_Desc()override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	CTexture*					m_pDiffuseTextureCom = { nullptr };
	CTexture*					m_pDissolveTextureCom = { nullptr };
	CTexture*					m_pNoiseTextureCom = { nullptr };

private:
	HRESULT						Add_Component(const char* pFileTag);
	HRESULT						SetUp_ShaderResources();
	void						SetUp_Linear();
	
	void						Add_Texture(const char* pFileTag, const char* TextureName, CTexture** pTexture);
	void						Add_Model(const char* pFileTag, const char* TextureName, CModel** pModel);
private:
	_float4x4					m_WorldMatrix;

	_float						m_fFrameAcc = { 0.f };
	_float						m_fLifeAcc = { 0.f };
	_float						m_fEffectAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };

public:
	static CMesh_Effect_P*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pFileTag , const EFFECT_DESC& MeshDesc);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END