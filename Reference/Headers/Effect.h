#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEffect abstract : public CGameObject
{
public:
	enum BUFFER_TYPE
	{
		BUFFER_TEX , BUFFER_MESH , BUFFER_END
	};

	enum EFFECT_TYPE
	{
		ID_MESH , ID_PARTICLE , EFFECT_TYPE_END
	};

	enum Figure
	{
		ID_RHOMBUS, ID_RECT, ID_TRIANGLE, ID_END
	};

	enum PARTICLE_PASS
	{
		PASS_ALPHABLEND, PASS_COLOR_T0_ALPHA, PASS_END
	};

	typedef struct EFFECT_DESC
	{
#pragma region Effect
		EFFECT_TYPE eEffectType;

		_float	fLifeTime;
		_float	fEffectTime;

		_bool	bSprite;
		_bool	bSetColor;

		_bool	bDelay;
		_float	fDelayTime;

		_float  fStartAlpha;
		_float  fCurAlpha;
		_float  fEndAlpha;

		_float3 vStartColor;
		_float3 vTextrueColor;
		_float3 vEndColor;

		_float2	vUV;
		_float2	fUVSpeed;
		_float2	vFrame;
		_float	fFrameSpeed;

		_bool  	bRhombus;
		_bool	bRect;
		_bool 	bTriangle;

		_bool	bGlow;
		_int	iRenderGroup;
		_int	iPass;

		_bool	bLoop;
		_bool	bTracking;
#pragma endregion

#pragma region Particle
		_bool	bSpark;
		_bool	bCircle;

		_bool	bParticleDelay;
		_float	fParticleDelayTime;
		_int	iParticleDelay;

		_float3 vDir;
		_float2 vDir_Angle;

		_float fMinSpeed;
		_float fMaxSpeed;

		_float3 vMinAngle;
		_float3 vMaxAngle;

		_float fMinDist;
		_float fMaxDist;

		_float3 vMinScale;
		_float3 vMaxScale;

		_float3 vMinPosition;
		_float3 vMaxPosition;

		_int iNumInstance;

		_float fParticlePower;
		_float fGravityPower;

		_bool bGravity;
		_bool bParticleLoop;

		Figure	eFigure;
#pragma endregion

#pragma region Mesh

		_float3 vStartPosition;
		_float3 vCurPosition;
		_float3 vEndPosition;

		_float3 vStartScale;
		_float3 vCurScale;
		_float3 vEndScale;

		_float3 vStartAngle;
		_float3 vCurAngle;
		_float3 vEndAngle;

		_float	fStartDelay;

		char MeshName[MAX_PATH];

#pragma endregion
		_bool bTexture;
		_bool bNoiseTexure;
		_bool bDissolveTexure;

		char TexName[MAX_PATH];
		char NoiseTexName[MAX_PATH];
		char DissolveTexName[MAX_PATH];
	};

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual void Play_Effect(_float4x4* pWorldMatrix , _bool bTracking = false) = 0;
	virtual EFFECT_DESC Get_Effect_Desc() = 0;

public:
	_float Get_CamDistance() const {
		return m_fCameraDistance;
	}

	_bool Get_Finish() { return m_bFinish; }
	void Set_Edit();

protected:
	void Compute_CamDistance(_fvector vWorldPos);

public:
	const _bool	Get_UpdateOK() const { return m_bEffectUpdate; }
	void	Set_UpdateOK(_bool bUpdateOK) { m_bEffectUpdate = bUpdateOK; }
protected:
	EFFECT_DESC m_EffectDesc;

	_bool	m_bEffectUpdate = { false };
	_float	m_fCameraDistance = { 0.f };
	_bool	m_bEDit = { false };
	_bool	m_bFinish = { true };

	char		m_FileName[BUFFER_END][100][MAX_PATH];
	_int		m_iFileCount[BUFFER_END] = { 0 , 0 };

	_float4x4					m_ParentsMatrix;
	_float4x4*					m_pParentsMatrix = { nullptr };

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void Free()override;
};

END