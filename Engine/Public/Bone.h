#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	const _tchar* Get_Name() const { return m_szName; }

	const _float4x4& Get_DefaultTransformationMatrix() { return m_DefaultTransformationMatrix; }
	const _float4x4& Get_PoseTransformationMatrix() { return m_PoseTransformationMatrix; }
	const _float4x4& Get_TransformationMatrix() { return m_TransformationMatrix; }
	const _float4x4& Get_CombinedTransfromationMatrix() { return m_CombinedTransformationMatrix; }

	void Reset_Pose()
	{
		m_PoseTransformationMatrix = m_DefaultTransformationMatrix;
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix)
	{
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

	_float4x4 Get_OffsetMatrix()	const {
		return m_OffsetMatrix;
	}

	void Set_PoseTransformationMatrix(_fmatrix RibbonMatrix)
	{
		XMStoreFloat4x4(&m_PoseTransformationMatrix, RibbonMatrix);
	}

	void	Set_TransformationMatrix(_fmatrix matTransform)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, matTransform);
	}

	CBone* Get_ParentBone() { return m_pParent; }

	void Set_Position(const _float3& vPosition)
	{
		memcpy((_float3*)&m_TransformationMatrix.m[3], &vPosition, sizeof(_float3));
	}

	_vector	Get_CombinedPosition()
	{
		return XMLoadFloat3((_float3*)&m_CombinedTransformationMatrix.m[3]);
	}

	void Set_LookDir(const _float3& vLookDir)
	{
		memcpy((_float3*)&m_TransformationMatrix.m[2], &vLookDir, sizeof(_float3));
	}

	void Set_TargetBone(CBone* pTargetBone)
	{
		m_pTargetBone = pTargetBone;
	}

	_bool Is_ChildOf(const _tchar* pBoneName)
	{
		if (nullptr == m_pParent)
			return false;

		if (!lstrcmp(m_pParent->m_szName, pBoneName))
			return true;
		else if (!lstrcmp(m_pParent->m_szName, TEXT("Root")))
			return false;
		else
			return m_pParent->Is_ChildOf(pBoneName);
	}

public:
	HRESULT Initialize(BONEINFO* pBoneInfo);
	HRESULT	Set_ParentBone(CBone*	pParent);
	void	Invalidate_CombinedMatrix();

	void Update_TargetBone_Pose();
	void Update_TargetBone();
	void Ribbon_TargetBone_Pose();
	void Ribbon_TargetBone();

private:
	_tchar m_szName[MAX_PATH] = TEXT("");
	_float4x4 m_OffsetMatrix;
	_float4x4 m_DefaultTransformationMatrix;
	_float4x4 m_PoseTransformationMatrix;
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	CBone* m_pParent = { nullptr };
	CBone* m_pTargetBone = { nullptr };

	_bool m_bAnim = { true };

public:
	static CBone* Create(BONEINFO* pBoneInfo);
	CBone* Clone();
	virtual void Free() override;
};

END