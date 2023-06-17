#pragma once
#include "Missile_Constant.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CMissile_NaviBoom : public CMissile_Constant
{
public:
	typedef struct tagNaviBoomMissileDesc {
		CONSTMISSILEDESC tConstMissileDesc;
		CNavigation*	pNavigation;
	}NAVIBOOMMISSILEDESC;
	
private:
	CMissile_NaviBoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMissile_NaviBoom(const CMissile_NaviBoom& rhs);
	virtual ~CMissile_NaviBoom() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);

	virtual _bool Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vMoveDir);
	virtual void End();
	
private:
	CNavigation*	m_pNavigation = { nullptr };
	_bool			m_bNaviBoom = { false };

public:
	static CMissile_NaviBoom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END