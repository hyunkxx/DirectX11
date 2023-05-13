#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CInteractionUI : public CGameObject
{
public:
	explicit CInteractionUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CInteractionUI(const CInteractionUI& rhs);
	virtual ~CInteractionUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	void SetRender(_bool bValue);

private:
	HRESULT addComponents();

public:
	static CInteractionUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;
	CTexture* m_pKeyTexture = nullptr;
	CTexture* m_pListBackTexture = nullptr;
	CTexture* m_pSpriteTexture = nullptr;

	

private: 
	_float4x4 m_ViewMatrix, m_ProjMatrix;

	ORTHO_DESC m_KeyDesc;
	ORTHO_DESC m_InteractionBack;

private: // Sprite
	_float2 m_SpriteSize;
	_float m_fSpriteAcc = 0.0f;
	_float m_fSpriteSpeed = 0.02f;
	_float m_fCurrentSpriteIndex = 0.f;

private:
	_bool m_bActive = false;

	_bool m_UIRender = false;
	_float m_fAlpha = 0.f;

};

END