#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CAcquireUI : public CGameObject
{
public:
	explicit CAcquireUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CAcquireUI(const CAcquireUI& rhs);
	virtual ~CAcquireUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	void SetAcquireActive(_bool bValue);

private:
	HRESULT addComponents();

public:
	static CAcquireUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;

private:
	_float4x4 m_ViewMatrix, m_ProjMatrix;
	ORTHO_DESC m_KeyDesc;

private: 
	queue<struct ITEM_DESC*> m_ItemDescQueue;

};

END