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
		class CCharacter*	pOwner;			// �̻����� ������
		_double				HitInterval;	// �浹�� ���� ���� �� �ٴ���Ʈ�ϴ� ����
		_double				LifeTime;		// �ִ� ���� �ð�
		_uint				iAttackInfoID;	// ��Ʈ �� ������ ���� ����
		_float				fExtents;		// Collider ��ü ������
		_bool				bDeleteOnHit;
		_bool				bNoShutDownEffect;

		class CMissilePool**	ppNextMissilePool;	// �ش� �̻����� ������鼭 ������ ���� �̻����� Ǯ
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

	// �ǵ��� �浹 ���� �浹���� ��� ó��
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