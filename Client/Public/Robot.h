#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)

class CRobot final  : public CGameObject
	, public IOnCollisionEnter
{
private:
	CRobot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRobot(const CRobot& rhs);
	virtual ~CRobot() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	_bool		All_Hit_Check();
	void		Give_Compensation(_int iLast_Hit_Count);
	
private:
	CEffect*	m_pEffect[4] = { nullptr };
	CEffect*	m_pHitEffect[4] = { nullptr };
	CGameObject* m_pMyBox = { nullptr };
	CGameObject* m_pCamera = { nullptr };

	_float4x4	m_RobotMatrix[4];
	_float4x4	m_BoxMatrix;

	_tchar		m_BoxPath[MAX_PATH];
	_bool		m_bHit[4];

	CCollider*  m_pCollider[4] = { nullptr };
protected:
	HRESULT Add_Components();

public:
	static CRobot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider[0]; }
	void CRobot::OnCollisionEnter(CCollider * src, CCollider * dest)override;
};

END