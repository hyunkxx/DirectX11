#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;

END

BEGIN(Client)

class CInteractionUI;

class CAcquireSystem final : public CGameObject
{
public:
	explicit CAcquireSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAcquireSystem(const CAcquireSystem& rhs);
	virtual ~CAcquireSystem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	static CAcquireSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CInteractionUI* m_pInteraction = nullptr;

};

END