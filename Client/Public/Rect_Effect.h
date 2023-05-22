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

class CRect_Effect final : public CEffect
{
public:
	enum Effect_Texture_Type
	{
		TEX_NOISE, TEX_DISSOLVE, EFFECT_TEX_END
	};

protected:
	CRect_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRect_Effect(const CRect_Effect& rhs);
	virtual ~CRect_Effect() = default;

public:
	virtual	HRESULT				Initialize_Prototype(const EFFECT_DESC& MeshDesc);
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
	CVIBuffer_SSDBox*			m_pVIBoxCom	= { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CTexture*					m_pDissolveTextureCom = { nullptr };
	CTexture*					m_pNoiseTextureCom = { nullptr };

private:
	HRESULT						Add_Component(const EFFECT_DESC& ParticleDesc);
	HRESULT						SetUp_ShaderResources();

	HRESULT						Add_Texture(const char * pKeyTag, const char * pCharTag);
	HRESULT						Add_NoiseTex(const char * pKeyTag, const char * pCharTag);
	HRESULT						Add_DissolveTex(const char * pKeyTag, const char * pCharTag);
	
#ifdef _DEBUG
	HRESULT						Add_Texture(const _tchar* pPathName);
	HRESULT						Add_NoiseTex(const _tchar* pPathName);
	HRESULT						Add_DissolveTex(const _tchar* pPathName);
#endif // _DEBUG

	void						Set_Image(const char * pImageTag);
	void						Set_DissolveImage(const char * pImageTag);
	void						Set_NoiseImage(const char * pImageTag);

	void						SetUp_Linear();

private:
	char						m_PrevSelect_Diffuse_TexName[MAX_PATH] = "";
	char						m_PrevSelect_Noise_TexName[MAX_PATH] = "";
	char						m_PrevSelect_Dissolve_TexName[MAX_PATH] = "";

	_float4x4					m_WorldMatrix;
	_float4x4					m_ResultMatirx;
	_float						m_fFrameAcc = { 0.f };
	_float						m_fLifeAcc = { 0.f };
	_float						m_fEffectAcc = { 0.f };
	_float						m_fDelayAcc = { 0.f };
	_float						m_fBillboard_Angle = { 0.f };

	_bool						m_bAngleSlider = { false };
	_bool						m_bDistortion = { false };

	_bool						m_bPositionLock = { false };
	_bool						m_bAngleLock = { false };
	_bool						m_bColorLock = { false };
	_bool						m_bScaleLock = { false };

	unordered_map<char*, CTexture*> m_Textures;
	unordered_map<char*, CTexture*> m_NoiseTex;
	unordered_map<char*, CTexture*> m_DissolveTex;

	char		m_Effect_TexName[EFFECT_TEX_END][600][MAX_PATH];
	_int		m_iEffect_TexCount[EFFECT_TEX_END] = { 0 , 0 };

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
	static CRect_Effect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const EFFECT_DESC& MeshDesc);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;
};

END