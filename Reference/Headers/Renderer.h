#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER_GROUP { RENDER_PRIORITY, RENDER_SHADOWDEPTH, RENDER_NONALPHA, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	HRESULT Add_RenderGroup(RENDER_GROUP eRenderGroup, class CGameObject* pGameObject);
	void Draw();

private:
	void Render_Priority();
	void Render_ShadowDepth();
	void Render_NonAlphaBlend();
	void Render_Lights();
	void Render_Outline();
	void Render_Blend();
	void Render_NonLight();
	void Render_AlphaBlend();
	void Render_UI();

	void AlphaSort(CRenderer::RENDER_GROUP eGroup);

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
	class CShader* m_pShader = nullptr;
	_float4x4 m_FullScreenWorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	class CLightManager* m_pLightManager = nullptr;
	class CTargetManager* m_pTargetManager = nullptr;

};

END