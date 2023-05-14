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
	const _float4x4& Get_TransformationMatrix() { return m_TransformationMatrix; }
	const _float4x4& Get_CombinedTransfromationMatrix() { return m_CombinedTransformationMatrix; }

	void Set_OffsetMatrix(_fmatrix OffsetMatrix)
	{
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

	_float4x4 Get_OffsetMatrix()	const {
		return m_OffsetMatrix;
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

	_float3* Get_CombinedPosition_Float3()
	{
		return (_float3*)&m_CombinedTransformationMatrix.m[3];
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
		else
			return m_pParent->Is_ChildOf(pBoneName);
	}

	void Set_Apply(_bool bApply)
	{
		m_bApply = bApply;
	}

public:
	HRESULT Initialize(BONEINFO* pBoneInfo);
	HRESULT	Set_ParentBone(CBone*	pParent);
	void	Invalidate_CombinedMatrix();

	void Update_TargetBone();
	void Ribbon_TargetBone();
	void Reset_Pose();

private:
	_tchar m_szName[MAX_PATH] = TEXT("");
	_float4x4 m_OffsetMatrix;
	_float4x4 m_DefaultTransformationMatrix;
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	CBone* m_pParent = { nullptr };

	// AnimSet 전용, Render용 모델의 같은 이름의 본을 타겟으로 잡아놓은 상태
	CBone* m_pTargetBone = { nullptr };

	// AnimSet 전용, 이번 프레임에 애니메이션 채널에서 갱신한 정보를 Target에 전달하는가
	_bool m_bApply = { false };

public:
	static CBone* Create(BONEINFO* pBoneInfo);
	CBone* Clone();
	virtual void Free() override;
};

END