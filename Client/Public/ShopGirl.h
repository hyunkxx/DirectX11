#pragma once

#include "NonPlayer.h"
#include "Collider.h"

BEGIN(Engine)
class CModel_Anim;
class CBone; 
class CCollider;
class IOnCollisionEnter;
class IOnCollisionStay;
class IOnCollisionExit;

END

BEGIN(Client)

class CShopGirl final
	: public CNonPlayer
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
private:
	enum CLIP
	{
		CLIP_IDLE,
		CLIP_TRIP_BACK,
		CLIP_TRIP_FORWARD,
		CLIP_TRIP_LEFT,
		CLIP_TRIP_RIGHT,
		CLIP_TURN_LEFT,
		CLIP_TURN_RIGHT,
		CLIP_LAUGH_END,
		CLIP_LAUGH_LOOP,
		CLIP_LAUGH_START,
		CLIP_TALK_END,
		CLIP_TALK_LOOP,
		CLIP_TALK_START,
		NO_USE_E,
		NO_USE_L,
		NO_USE_S,
		CLIP_SCARE_END,
		CLIP_SCARE_LOOP,
		CLIP_SCARE_START,
	};

protected:
	explicit CShopGirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CShopGirl(const CShopGirl& rhs);
	virtual ~CShopGirl() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

private:
	HRESULT addComponents();
	HRESULT setupShader();

	void animationUpdate(_double TimeDelta);

public:
	virtual void Interaction();
	virtual void PushAnimation(ANIM_STATE eAnimState);

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

public:
	static CShopGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CModel_Anim* m_pModel = nullptr;
	CCollider* m_pCollider = nullptr;
	
private:
	class CPlayerState* m_pPlayerState = nullptr;
	class CUI_Souvenir* m_pTargetUI = nullptr;

	queue<CLIP> m_AnimQueue;
	_uint m_iCurClip = CLIP_IDLE;

	_bool m_bOverlaped = false;
	_float m_fTimeAcc = 0.f;
};

END