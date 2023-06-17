#include "stdafx.h"
#include "..\Public\MissilePool.h"
#include "GameInstance.h"
#include "GameMode.h"
#include "Missile_Constant.h"
#include "Missile_RotAround.h"
#include "Missile_NaviBoom.h"

CMissilePool::CMissilePool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMissilePool::CMissilePool(const CMissilePool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMissilePool::Initialize(_fvector vLocalPos, MISSILEPOOLDESC* pMissilePoolDesc)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	XMStoreFloat3(&m_vLocalPos, vLocalPos);

	CGameInstance* pGI = CGameInstance::GetInstance();

	CMissile* pMissile = nullptr;
	for (_uint i = 0; i < pMissilePoolDesc->iNumMissiles; ++i)
	{
		_tchar szBuffer[MAX_PATH] = TEXT("");
		wsprintf(szBuffer, pMissilePoolDesc->pMissilePoolTag, i);

		if (MISS_NOMOVE == pMissilePoolDesc->iMissileType)
		{
			if (FAILED(pGI->Add_GameObjectEx((CGameObject**)(&pMissile), LEVEL_ANYWHERE, OBJECT::MISSILE, TEXT("Layer_Missile"), szBuffer, &pMissilePoolDesc->tMissileDesc)))
				return E_FAIL;
			m_Missiles.push_back(pMissile);
		}
		else if (MISS_CONSTANT == pMissilePoolDesc->iMissileType)
		{
			CMissile_Constant::CONSTMISSILEDESC tConstDesc;
			ZeroMemory(&tConstDesc, sizeof(CMissile_Constant::CONSTMISSILEDESC));

			tConstDesc.tMissileDesc = pMissilePoolDesc->tMissileDesc;
			tConstDesc.bTargetDir = pMissilePoolDesc->bTargetDir;
			tConstDesc.vFixMoveDir = pMissilePoolDesc->vFixMoveDir;
			tConstDesc.fVelocity = pMissilePoolDesc->fVelocity;
			tConstDesc.StopTime = pMissilePoolDesc->StopTime;
			tConstDesc.iStopCondition = pMissilePoolDesc->iStopCondition;

			if (FAILED(pGI->Add_GameObjectEx((CGameObject**)(&pMissile), LEVEL_ANYWHERE, OBJECT::MISSILE_CONSTANT, TEXT("Layer_Missile"), szBuffer, &tConstDesc)))
				return E_FAIL;

			m_Missiles.push_back(pMissile);
		}
		else if (MISS_ROTAROUND == pMissilePoolDesc->iMissileType)
		{
			CMissile_RotAround::ROTMISSILEDESC tRotDesc;
			ZeroMemory(&tRotDesc, sizeof(CMissile_RotAround ::ROTMISSILEDESC));

			tRotDesc.tMissileDesc = pMissilePoolDesc->tMissileDesc;
			tRotDesc.pTargetTransform = pMissilePoolDesc->pTargetTransform;
			tRotDesc.pTargetBone = pMissilePoolDesc->pTargetBone;
			tRotDesc.vAxis = pMissilePoolDesc->vAxis;
			tRotDesc.fInitAngle = pMissilePoolDesc->fInitAngle;
			tRotDesc.fDistance = pMissilePoolDesc->fDistance;
			tRotDesc.fRotSpeed = pMissilePoolDesc->fRotSpeed;

			if (FAILED(pGI->Add_GameObjectEx((CGameObject**)(&pMissile), LEVEL_ANYWHERE, OBJECT::MISSILE_ROTAROUND, TEXT("Layer_Missile"), szBuffer, &tRotDesc)))
				return E_FAIL;

			m_Missiles.push_back(pMissile);
		}
		else if (MISS_NAVIBOOM == pMissilePoolDesc->iMissileType)
		{
			CMissile_NaviBoom::NAVIBOOMMISSILEDESC tNaviBoomDesc;
			ZeroMemory(&tNaviBoomDesc, sizeof(CMissile_NaviBoom::NAVIBOOMMISSILEDESC));

			tNaviBoomDesc.tConstMissileDesc.tMissileDesc = pMissilePoolDesc->tMissileDesc;
			tNaviBoomDesc.tConstMissileDesc.bTargetDir = pMissilePoolDesc->bTargetDir;
			tNaviBoomDesc.tConstMissileDesc.vFixMoveDir = pMissilePoolDesc->vFixMoveDir;
			tNaviBoomDesc.tConstMissileDesc.fVelocity = pMissilePoolDesc->fVelocity;
			tNaviBoomDesc.tConstMissileDesc.StopTime = pMissilePoolDesc->StopTime;
			tNaviBoomDesc.tConstMissileDesc.iStopCondition = pMissilePoolDesc->iStopCondition;

			tNaviBoomDesc.pNavigation = pMissilePoolDesc->pNavigation;

			if (FAILED(pGI->Add_GameObjectEx((CGameObject**)(&pMissile), LEVEL_ANYWHERE, OBJECT::MISSILE_NAVIBOOM, TEXT("Layer_Missile"), szBuffer, &tNaviBoomDesc)))
				return E_FAIL;

			m_Missiles.push_back(pMissile);
		}
	}

	return S_OK;
}

void CMissilePool::Shot(_fvector vInitPos, _fvector vLookDir, _fmatrix vMissileRotMatrix, _fvector vTargetPos)
{
	_bool bShot = false;

	_vector vLook = XMVector3Normalize(vLookDir);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	_vector vFinalInitPos = vInitPos + vRight * m_vLocalPos.x + vUp * m_vLocalPos.y + vLook * m_vLocalPos.z;

	_vector vMoveDir = XMVectorZero();
	if (!XMVector3Equal(vTargetPos, XMVectorZero()))
		vMoveDir = XMVector3Normalize(vTargetPos - vFinalInitPos);

	for (auto& pMissile : m_Missiles)
	{
		if (bShot = pMissile->Shot(vFinalInitPos, vLookDir, vMissileRotMatrix, vMoveDir))
			break;
	}

	if (false == bShot)
 		MSG_BOX("Shot Failed, Missiles are all busy");
}

CMissilePool * CMissilePool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _fvector vLocalPos, MISSILEPOOLDESC* pMissilePoolDesc)
{
	CMissilePool* pInstance = new CMissilePool(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vLocalPos, pMissilePoolDesc)))
	{
		MSG_BOX("Failed to Create : CMissilePool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissilePool::Free()
{
	__super::Free();

	m_Missiles.clear();
}
