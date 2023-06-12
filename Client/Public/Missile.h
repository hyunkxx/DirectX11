#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CSphereCollider;
class CNavigation;
class CEffect;
END

BEGIN(Client)

class CMissile 
	: public CGameObject
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
public:
	
	typedef struct tagMissileDesc {
		_tchar				szLoopEffectTag[MAX_PATH];
		_uint				iLoopEffectLayer;
		class CCharacter*	pOwner;			// 미사일의 소유자
		_double				HitInterval;	// 충돌이 유지 중일 때 다단히트하는 간격
		_double				LifeTime;		// 최대 유지 시간
		_uint				iAttackInfoID;	// 히트 시 적용할 공격 정보
		_float				fExtents;		// Collider 구체 반지름
		_bool				bDeleteOnHit;
		_bool				bNoShutDownEffect;

		class CMissilePool**	ppNextMissilePool;	// 해당 미사일이 사라지면서 생성할 다음 미사일의 풀
	}MISSILEDESC;

protected:
	CMissile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile(const CMissile& rhs);
	virtual ~CMissile() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();

	virtual _bool Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vMoveDir);
	virtual void End();

	virtual CCharacter* Get_Owner()
	{
		return m_tMissileDesc.pOwner;
	}

	virtual _uint	Get_AttackID()
	{
		return m_tMissileDesc.iAttackInfoID;
	}

	virtual _vector Get_Position();

	virtual _bool Get_TargetDir()
	{
		return false;
	}

	// 의도한 충돌 대상과 충돌했을 경우 처리
	virtual void OnCollisionOnTarget(CCollider * src, CCollider * dest);
	
protected:
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pCollider = { nullptr };

protected:
	MISSILEDESC m_tMissileDesc;
	_double		m_LifeTimeTimer = { 0.0 };
	_double		m_HitIntervalTimer = { 0.0 };
	_bool		m_bFirstColl = true;

	CEffect*	m_pEffect = nullptr;

protected:
	HRESULT Add_Components();

public:
	static CMissile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	void CMissile::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CMissile::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CMissile::OnCollisionExit(CCollider * src, CCollider * dest) override;
	
};

END