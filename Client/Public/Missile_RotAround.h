#pragma once
#include "Missile.h"

BEGIN(Engine)
class CRenderer;
class CSphereCollider;
class CNavigation;
class CBone;
END

BEGIN(Client)

class CMissile_RotAround
	: public CMissile
{
public:
	typedef struct tagRotAroundMissileDesc {
		MISSILEDESC tMissileDesc;
		CTransform* pTargetTransform;
		CBone*		pTargetBone;
		_float3		vAxis;
		_float		fInitAngle;
		_float		fDistance;
		_float		fRotSpeed;	// 회전 각속도
	}ROTMISSILEDESC;

private:
	CMissile_RotAround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile_RotAround(const CMissile_RotAround& rhs);
	virtual ~CMissile_RotAround() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);

	virtual _bool Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vMoveDir);

private:
	CTransform* m_pTargetTransform = { nullptr };
	CBone*		m_pTargetBone = { nullptr };
	_float3		m_vAxis = { 0.f, 0.f, 0.f };
	_float		m_fInitAngle = { 0.f };
	_float		m_fRotSpeed = { 0.f };
	_float		m_fDistance = { 0.f };

	// 
	_float3		m_vShotLookDir = {};
	_float		m_fCurAngle = { 0.f };


public:
	static CMissile_RotAround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END