#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Particle;
END

BEGIN(Client)

class CParticle_Effect_P final : public CEffect
{
protected:
	CParticle_Effect_P(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Effect_P(const CParticle_Effect_P& rhs);
	virtual ~CParticle_Effect_P() = default;

public:
	virtual	HRESULT				Initialize_Prototype(const char* pFileTag , const EFFECT_DESC& ParticleDesc);
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
	CVIBuffer_Particle*			m_pVIBufferCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };

private:
	HRESULT						Add_Component(const char* pFileTag);
	HRESULT						SetUp_ShaderResources();

	void						Loop_Check(_double TimeDelta);
	void						Setup_Linear();

	void						Add_Texture(const char* pFileTag, const char* TextureName, CTexture** pTexture);
private:
	_float						m_fFrameAcc = { 0.f };
	_float						m_fEffectAcc = { 0.f };
	_float						m_fLifeAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };

#define	 MAX_STARTDISTANCE		m_EffectDesc.vStartPosition.x
#define	 MIN_DISTANCE			m_EffectDesc.vStartPosition.y

public:
	static CParticle_Effect_P*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , const char* pFileTag , const EFFECT_DESC& ParticleDesc);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END