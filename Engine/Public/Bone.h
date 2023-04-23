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

	const _float4x4& Get_TransformationMatrix() { return m_TransformationMatrix; }

	const _float4x4& Get_CombinedTransfromationMatrix() { return m_CombinedTransformationMatrix; }

	void	Set_TransformationMatrix(_fmatrix matTransform)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, matTransform);
	}

	_float4x4 Get_OffsetMatrix()	const {
		return m_OffsetMatrix;
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix)
	{
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
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

public:
	HRESULT Initialize(BONEINFO* pBoneInfo);
	HRESULT	Set_ParentBone(CBone*	pParent);
	void	Invalidate_CombinedMatrix();

private:
	_tchar m_szName[MAX_PATH] = TEXT("");
	_float4x4 m_OffsetMatrix;
	_float4x4 m_DefaultTransformationMatrix;
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	CBone* m_pParent = { nullptr };
	_bool	m_bAnim = { true };

public:
	static CBone* Create(BONEINFO* pBoneInfo);
	CBone* Clone();
	virtual void Free() override;
};

END