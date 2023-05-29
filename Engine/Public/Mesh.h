#pragma once

#include "VIBuffer.h"
#include "Model_Anim.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pMeshInfo, CModel_Anim* pModel);
	virtual HRESULT Initialize(void* pArg);

public:
	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

	const _tchar* Get_Name() { return m_szName; }
	void Get_BoneMatrices(_float4x4* pMeshBoneMatrices, CModel_Anim* Model);
	void Get_BoneMatrices_VTF(_float4x4* pMeshBoneMatrices, CModel_Anim* Model);

	_float3* Get_PosMinValue() { return &m_fPosMinValue; }
	_float3* Get_PosMaxValue() { return &m_fPosMaxValue; }
private:
	_tchar	m_szName[MAX_PATH] = TEXT("");
	_uint	m_iMaterialIndex = { 0 };
	_uint	m_iNumBones = { 0 };

	_float3	m_fPosMinValue = { };
	_float3	m_fPosMaxValue = { };

private:
	//vector<_uint>	m_BoneIndices;

	_uint*	m_pBoneIndices = { nullptr };


private:
	HRESULT Ready_VIBuffer_For_NonAnim(SMESHINFO* pMeshInfo);
	HRESULT	Ready_VIBuffer_For_Anim(DMESHINFO* pMeshInfo, CModel_Anim* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pMeshInfo, CModel_Anim* pModel = nullptr);
	virtual CComponent*	Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END