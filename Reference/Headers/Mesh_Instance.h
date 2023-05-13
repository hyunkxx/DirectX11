#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Instance final : public CVIBuffer
{
private:
	CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Instance(const CMesh_Instance& rhs);
	virtual ~CMesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pMeshInfo, _uint iNumInstance, _float4x4* pInstanceMatrix);
	virtual HRESULT Initialize(void* pArg);
	HRESULT Render();

public:
	_uint Get_MaterialIndex() const
	{
		return m_iMaterialIndex;
	}

private:
	_tchar	m_szName[MAX_PATH] = TEXT("");
	_uint	m_iMaterialIndex = { 0 };
	_uint	m_iNumBones = { 0 };

private:
	vector<_uint>	m_BoneIndices;

private:
	HRESULT Ready_VIBuffer_For_NonAnim_Instance(SMESHINFO* pMeshInfo, _float4x4* pInstanceMatrix);

public:
	_uint Get_TotalNumInstance() { return m_iTotalNumInstance; }
	VTXMATRIX* Get_TotalInstanceMatrix() { return m_pTotalInstanceMatrix; }
	VTXMATRIX* Get_InstanceMatrix(_uint iInstanceNum) { return &m_pTotalInstanceMatrix[iInstanceNum]; }

public:
	void Update(_uint iNumInstance, _float4x4 * pInstanceMatrix);

public:
	void Get_PSA_To_InstanceMatrix(_uint iGetNum, __out SOBJECT_DESC* pOut);

	_float3 GetScale_To_InstanceMatrix(_uint iGetNum);
	_float3 GetAngle_To_InstanceMatrix(_uint iGetNum);
	_float3 GetPosition_To_InstanceMatrix(_uint iGetNum);

private:
	_uint							m_iStrideInstance = { 0 };
	ID3D11Buffer*					m_pVBInstance = { nullptr };

	_uint							m_iNumInstance = { 0 };
	VTXMATRIX*						m_pInstanceMatrix = { nullptr };
	FACEINDICES32*					m_pIndices = { nullptr };

	_uint							m_iTotalNumInstance = { 0 };
	VTXMATRIX*						m_pTotalInstanceMatrix = { nullptr };

public:
	void Culling(_fmatrix Inverse_WorldMatrix, _float fRadius);

private:
	_float							m_fRadius = { 0.0f };

public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pMeshInfo,
		_uint iNumInstance, _float4x4* pInstanceMatrix);
	virtual CComponent*	Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END