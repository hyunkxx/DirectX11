#pragma once
#include "Missile.h"

BEGIN(Engine)
class CRenderer;
class CSphereCollider;
class CNavigation;
END

BEGIN(Client)

class CMissile_Constant
	: public CMissile
{
public:
	enum StopCondition
	{
		STOP_NONE,	// �� ����
		STOP_ONCOLLISIONENTER, // �浹 �� ����
		STOP_END
	};
public:
	typedef struct tagConstantMissileDesc {
		MISSILEDESC tMissileDesc;
		_float3		vMoveDir;
		_float		fVelocity;
		_double		StopTime;
		_uint		iStopCondition;
	}CONSTMISSILEDESC;

private:
	CMissile_Constant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile_Constant(const CMissile_Constant& rhs);
	virtual ~CMissile_Constant() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);

	virtual _bool Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix);
	// �ǵ��� �浹 ���� �浹���� ��� ó��
	virtual void OnCollisionOnTarget(CCollider * src, CCollider * dest);

private:
	// ������
	_float3 m_vLocalMoveDir = {};
	_double m_StopTime = { 0.0 };
	_float m_fVelocity = { 0.f };
	_uint m_iStopCondition = { 0 };

	// ������
	_float3 m_vWorldMoveDir = {};
	_double m_StopTimer = { 0.0 };
	_bool m_bMove = { false };


public:
	static CMissile_Constant* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	//void CMissile_Constant::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	//void CMissile_Constant::OnCollisionStay(CCollider * src, CCollider * dest) override;
	//void CMissile_Constant::OnCollisionExit(CCollider * src, CCollider * dest) override;
};

END