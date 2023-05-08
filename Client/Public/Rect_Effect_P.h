#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_SSDBox;
class CVIBuffer_Rect_Nor;
END

BEGIN(Client)

class CRect_Effect_P final : public CEffect
{
public:
	enum Effect_Texture_Type
	{
		TEX_NOISE, TEX_DISSOLVE, EFFECT_TEX_END
	};
protected:
	CRect_Effect_P(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRect_Effect_P(const CRect_Effect_P& rhs);
	virtual ~CRect_Effect_P() = default;

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
	CVIBuffer_Rect_Nor*			m_pVIBufferCom = { nullptr };
	CVIBuffer_SSDBox*			m_pVIBoxCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CTexture*					m_pDissolveTextureCom = { nullptr };
	CTexture*					m_pNoiseTextureCom = { nullptr };

private:
	HRESULT						Add_Component(const char* pFileTag , const EFFECT_DESC& ParticleDesc);
	HRESULT						SetUp_ShaderResources();

	void						SetUp_Linear();
	void						Distortion_Tick(_double TimeDelta);
	void						Loop_Check(_double TimeDelta);

	HRESULT						Add_Texture(const char* pFileTag, const char* TextureName, CTexture ** pTexture);
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ResultMatirx;
	_float						m_fFrameAcc = { 0.f };
	_float						m_fLifeAcc = { 0.f };
	_float						m_fEffectAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };

	_bool						m_bDistortion = { false };

private:

#define STRAT_UV m_EffectDesc.vMinScale

#define START_DISTIME m_EffectDesc.vMaxScale.x
#define END_DISTIME m_EffectDesc.vMaxScale.y
#define DISTORTION_POWER m_EffectDesc.vMaxScale.z

#define MAX_DISTORTION_POWER m_EffectDesc.vMinPosition.x
#define DISTORTION_SPEED m_EffectDesc.vMinPosition.y
#define START_DIS_POWER m_EffectDesc.vMinPosition.z

#define BILLBOARD	m_EffectDesc.bSpark

public:
	static CRect_Effect_P*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , const char* pFileTag , const EFFECT_DESC& MeshDesc);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END