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
		MISS_NOMOVE,	// �������� �ʴ� ���� ����
		MISS_CONSTANT,	// ��� ��� �ϴ� ����ü
		MISS_ROTAROUND, // Ư�� ��� ������ ��ȸ�ϴ� ����ü
		MISS_NAVIBOOM,	// CONSTANT�� �����ѵ� �ٴڿ� ������ ������ ����ü
		MISS_END
	};

public:
	typedef struct tagMissilePoolDesc
	{
		_tchar*	pMissilePoolTag;
		CMissile::MISSILEDESC tMissileDesc;
		_uint iMissileType;
		_uint iNumMissiles;

		//Constant�� ����
		_bool		bTargetDir;
		_float3		vFixMoveDir;
		_float		fVelocity;
		_double		StopTime;
		_uint		iStopCondition;

		//RotAround�� ����
		CTransform* pTargetTransform;
		CBone*		pTargetBone;
		_float3		vAxis;
		_float		fInitAngle;
		_float		fDistance;
		_float		fRotSpeed;	// ȸ�� ���ӵ�

		//Binglie �Žñ��
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