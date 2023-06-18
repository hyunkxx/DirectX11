#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Missile.h"

BEGIN(Engine)
class CTransform;
class CBone;
class CNavigation;
END

BEGIN(Client)

class CMissilePool : public CGameObject
{
public:
	enum MissileType
	{
		MISS_NOMOVE,	// 움직이지 않는 공격 범위
		MISS_CONSTANT,	// 등속 운동을 하는 투사체
		MISS_ROTAROUND, // 특정 대상 주위를 선회하는 투사체
		MISS_NAVIBOOM,	// CONSTANT랑 동일한데 바닥에 닿으면 터지는 투사체
		MISS_END
	};

public:
	typedef struct tagMissilePoolDesc
	{
		_tchar*	pMissilePoolTag;
		CMissile::MISSILEDESC tMissileDesc;
		_uint iMissileType;
		_uint iNumMissiles;

		//Constant용 변수
		_bool		bTargetDir;
		_float3		vFixMoveDir;
		_float		fVelocity;
		_double		StopTime;
		_uint		iStopCondition;

		//RotAround용 변수
		CTransform* pTargetTransform;
		CBone*		pTargetBone;
		_float3		vAxis;
		_float		fInitAngle;
		_float		fDistance;
		_float		fRotSpeed;	// 회전 각속도

		//Binglie 거시기용
		CNavigation* pNavigation;

	}MISSILEPOOLDESC;

private:
	CMissilePool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissilePool(const CMissilePool& rhs);
	virtual ~CMissilePool() = default;

public:
	virtual HRESULT	Initialize(_fvector vLocalPos, MISSILEPOOLDESC* pMissilePoolDesc);
	virtual void Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vTargetPos = XMVectorZero());

	_bool Get_TargetDir()
	{
		m_Missiles.front()->Get_TargetDir();
	}

	void Set_CamMovement(class CCameraMovement* pPtr, _uint iShakeLevel)
	{
		m_pCamMovement = pPtr;
		m_iShakeLevel = iShakeLevel;
	}

private:
	_float3 m_vLocalPos;
	vector<CMissile*> m_Missiles;

	class CCameraMovement* m_pCamMovement = { nullptr };
	_uint m_iShakeLevel;

public:
	static CMissilePool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _fvector vLocalPos, MISSILEPOOLDESC* pMissilePoolDesc);
	CGameObject* Clone(void* pArg) override { return nullptr; }
	virtual void Free() override;
};

END