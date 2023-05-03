#include "..\Public\Bone.h"

#include "Model_Anim.h"

CBone::CBone()
{
}

CBone::CBone(const CBone & rhs)
	: m_DefaultTransformationMatrix(rhs.m_DefaultTransformationMatrix)
	, m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_CombinedTransformationMatrix(rhs.m_CombinedTransformationMatrix)
	, m_pParent(rhs.m_pParent)
	, m_pTargetBone(rhs.m_pTargetBone)
	, m_bAnim(rhs.m_bAnim)
{
	lstrcpy(m_szName, rhs.Get_Name());
	Safe_AddRef(m_pParent);
}

HRESULT CBone::Initialize(BONEINFO * pBoneInfo)
{
	lstrcpy(m_szName, pBoneInfo->s_szName);
	memcpy(&m_DefaultTransformationMatrix, &pBoneInfo->s_TransformationMatrix, sizeof(_float4x4));
	memcpy(&m_TransformationMatrix, &pBoneInfo->s_TransformationMatrix, sizeof(_float4x4));
	memcpy(&m_OffsetMatrix, &pBoneInfo->s_OffsetMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	m_bAnim = pBoneInfo->s_bAnim;

	return S_OK;
}

HRESULT CBone::Set_ParentBone(CBone * pParent)
{
	if (nullptr != m_pParent)
		Safe_Release(m_pParent);

	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Invalidate_CombinedMatrix()
{
	if (nullptr == m_pParent)
	{
		m_CombinedTransformationMatrix = m_TransformationMatrix;
		return;
	}

	if (m_bAnim)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->Get_CombinedTransfromationMatrix()));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_DefaultTransformationMatrix) * XMLoadFloat4x4(&m_pParent->Get_CombinedTransfromationMatrix()));
}

void CBone::Update_TargetBone()
{
	if (nullptr != m_pTargetBone)
	{
		m_pTargetBone->Set_TransformationMatrix(XMLoadFloat4x4(&m_TransformationMatrix));
	}
}

void CBone::Ribbon_TargetBone()
{
	if (nullptr != m_pTargetBone)
	{
		m_pTargetBone->Set_TransformationMatrix(XMLoadFloat4x4(&m_pTargetBone->Get_DefaultTransformationMatrix()) * XMLoadFloat4x4(&m_TransformationMatrix));
	}
}

CBone * CBone::Create(BONEINFO * pBoneInfo)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pBoneInfo)))
	{
		MSG_BOX("Failed to Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone * CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}
