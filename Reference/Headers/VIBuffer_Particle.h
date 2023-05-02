#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle final : public CVIBuffer
{
public:
	struct ParticleDesc
	{
		_bool	bCircle;

		_bool	bDelay;
		_float	fDelayTime;
		_int	iDelayParticle;

		_float3 vDir;

		_float fMinSpeed;
		_float fMaxSpeed;

		_float3 vMinAngle;
		_float3 vMaxAngle;

		_float fMinDist;
		_float fMaxDist;

		_float3 vMinScale;
		_float3 vMaxScale;
		_float3 vEndScale;

		_float3 vStartColor;
		_float3 vEndColor;

		_float3 vMinPosition;
		_float3 vMaxPosition;

		_int iNumInstance;

		_float fParticlePower;
		_float fGravityPower;

		_bool bGravuty;
		_bool bLoop;
	};

	enum ParticleNum
	{
		ID_CIRCLE ,
		ID_DELAY , ID_DELAY_TIME , ID_DELAY_PARTICLE ,
		ID_DIR ,
		ID_MINSPEED , ID_MAXSPEED , ID_MINANGLE , ID_MAXANGLE , ID_MINDIST , ID_MAXDIST ,
		ID_MINSCALE , ID_MAXSCALE , ID_ENDSCALE , ID_STARTCOLOR , ID_ENDCOLOR ,
		ID_MINPOSITION , ID_MAXPOSITION , ID_NUMINSTANCE , ID_PARTICLE_POWER , ID_GRAVITY_POWER ,
		ID_GRAVITY , ID_LOOP ,  ID_END
	};

protected:
	CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Particle(const CVIBuffer_Particle& rhs);
	virtual ~CVIBuffer_Particle() = default;

public:
	virtual	HRESULT Initialize_Prototype(const ParticleDesc& ParticleDesc);
	virtual	HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;

public:
	void Update(_double TimeDelta);
	void Reset_Update(_float3 vDir);

	void Set_Desc(ParticleNum eID, void* pArg);

	HRESULT SetUp_Shader_Color(class CShader* pShader , const char* ShaderTag);
	ParticleDesc Get_ParticleDesc() { return m_ParticleDesc; }

private:
	ID3D11Buffer*				m_pVBInstance = { nullptr };

	_int						m_iStrideInstance = { 0 };
	_int						m_iOriMaxNumInstance = { 0 };
	_int						m_iMaxNumInstance = { 0 };

	//ImGui Max설정용 인스턴스
	_uint						m_iSettimeMaxNumInst = { 0 };

	ParticleDesc				m_ParticleDesc;

	_float3						m_vCurColor = { 0.f , 0.f , 0.f };
	_float						m_fTimeAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };

	_float3*					m_pStartScale = { false };
	_float3*					m_pCurScale = { false };
	_float3*					m_pEndScale = { false };
	_float3*					m_pOriPos = { nullptr };

	_float3*					m_pAngle = { nullptr };

	_float*						m_pDist = { nullptr };
	_float*						m_pMaxDist = { nullptr };
	_float*						m_pSpeed = { nullptr };

	_float4x4*					m_pRotationMatrix = { nullptr };

public:
	static CVIBuffer_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const ParticleDesc& ParticleDesc);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END