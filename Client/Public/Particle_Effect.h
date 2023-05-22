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
class CParticle_Effect final : public CEffect
{
protected:
	CParticle_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Effect(const CParticle_Effect& rhs);
	virtual ~CParticle_Effect() = default;

public:
	virtual	HRESULT				Initialize_Prototype(const EFFECT_DESC& ParticleDesc);
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
	HRESULT						Add_Component(const EFFECT_DESC& ParticleDesc);
	HRESULT						SetUp_ShaderResources();

	HRESULT						Add_Texture(const char* pKeyTag, const char* pCharTag);

	void						Set_Image(const char * pImageTag);

private:
	char						m_PrevSelect_Diffuse_TexName[MAX_PATH] = "";

	_float						m_fFrameAcc = { 0.f };
	_float						m_fEffectAcc = { 0.f };
	_float						m_fLifeAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };

	unordered_map<const char*, CTexture*> m_Textures;

	//ImGui 에서만 필요한 변수
	_bool						m_bPrevLoop = { false };

#define	 MAX_STARTDISTANCE		m_EffectDesc.vStartPosition.x
#define	 MIN_DISTANCE			m_EffectDesc.vStartPosition.y
#define  CUMULATIVE_ROTATION	m_EffectDesc.vEndPosition

public:
	static CParticle_Effect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const EFFECT_DESC& ParticleDesc);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END