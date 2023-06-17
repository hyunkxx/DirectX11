#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;

END

BEGIN(Client)

class CNonPlayer abstract : public CGameObject
{
public:
	enum ANIM_STATE { ANIM_IDLE, ANIM_TALK, ANIM_SCARE };

protected:
	explicit CNonPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNonPlayer(const CNonPlayer& rhs);
	virtual ~CNonPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() = 0;
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() = 0;
	virtual void PreTick(_double TimeDelta) = 0;
	virtual void Tick(_double TimeDelta) = 0;
	virtual void LateTick(_double TimeDelta) = 0;
	virtual HRESULT Render() = 0;
	virtual HRESULT RenderShadow() = 0;
	virtual void RenderGUI() = 0;

public:
	virtual void Interaction() = 0;
	virtual void PushAnimation(ANIM_STATE eState) = 0;

public:
	CGameObject* Clone(void* pArg) = 0;
	virtual void Free();

protected:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CShader* m_pShader = nullptr;

};

END