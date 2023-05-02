#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_NONLIGHT, RENDER_PARTICLE , RENDER_ALPHABLEND, RENDER_UI, RENDER_DYNAMIC_SHADOW, RENDER_END };

private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

	HRESULT Add_RenderGroup(RENDER_GROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw();

private:
	void AlphaSort(CRenderer::RENDER_GROUP eGroup);

	void Render_Priority();
	void Render_DynamicShadowMap();
	void Render_NonAlphaBlend();
	void Render_Lights();
	void Render_Outline();
	void Render_Particle();
	void Render_Shadow();
	void Render_Blend();
	void Render_Glow();
	void Render_Glow_Blend();
	void Render_NonLight();
	void Render_AlphaBlend();
	void Render_UI();

	void Extraction(const _tchar * pBindTargetTag, const _tchar * pSourTag);
	void Target_Blur(const _tchar* TargetTag, _int BlurCount = 1);
	void Ready_SSAO(const _tchar* pBindTargetTag);

public:
	typedef struct tagSSAOSetting
	{
		float fRadius;
		float fFalloff;
		float fStrength;
		float fTotStrength;
		float fInvSamples;
	}SSAO_SETTING;

	void SetSSAO(const SSAO_SETTING& tagSetting) { m_eSSAO = tagSetting; };

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
public:
	list<class CGameObject*> m_RenderObject[RENDER_END];
	typedef list<class CGameObject*> RENDER_OBJECT;

#ifdef _DEBUG
public:
	HRESULT AddDebugGroup(class CComponent* pComponent);
	HRESULT RenderDebugGroup();

private:
	list<class CComponent*> m_DebugRenderObjects;

#endif

private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
	class CShader* m_pShader_Extraction = nullptr;
	class CShader* m_pShader = nullptr;
	class CShader* m_pShader_Blur = nullptr;
	class CShader* m_pShader_SSAO = nullptr;

	_float4x4 m_FullScreenWorldMatrix, m_ViewMatrix, m_ProjMatrix;

	class CTexture* m_pNoiseTexture = nullptr;

private:
	class CLightManager* m_pLightManager = nullptr;
	class CTargetManager* m_pTargetManager = nullptr;

private:
	SSAO_SETTING m_eSSAO;

};

END