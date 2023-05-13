#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instance : public CComponent
{
protected:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& rhs);
	virtual ~CModel_Instance() = default;

public:
	_uint	Get_NumMeshes() { return m_iNumMeshes; }
	_matrix	Get_LocalMatrix() { return XMLoadFloat4x4(&m_LocalMatrix); }
	void	Set_LocalMatrix(_fmatrix LocalMatrix) { XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix); }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pModelFilePath, const _tchar* pInstanceFilePath);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Load_InstanceData(const _tchar* pInstanceFilePath);

public:
	HRESULT	SetUp_ShaderMaterialResource(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex, MytextureType eType);
	HRESULT	SetUp_ShaderMaterialResource_Distinction(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex, MytextureType eType, _bool* IsDistinction = nullptr);

protected:
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh_Instance*>	m_Instance_Meshes;

protected:
	_uint					m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>	m_ModelMaterials;

protected:
	_float4x4				m_LocalMatrix;

public:
	_uint Get_TotalNumInstance();
	VTXMATRIX* Get_TotalInstanceMatrix();
	VTXMATRIX* Get_InstanceMatrix(_uint iInstanceNum);

public:
	void Get_PSA_To_InstanceMatrix(_uint iGetNum, __out SOBJECT_DESC* pOut);

private:
	_uint					m_iNumInstance = { 0 };
	_float4x4*				m_pInstanceMatrix = { nullptr };

public:
	void Culling(_fmatrix Inverse_WorldMatrix, _float fRadius);

private:
	HRESULT Ready_Meshes(SMODELINFO* pModel, _float4x4* pInstanceMatrices);
	HRESULT Ready_Materials(SMODELINFO* pModel, const _tchar* pModelFilePath);

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
		const _tchar* pModelFilePath, const _tchar* pInstanceFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END