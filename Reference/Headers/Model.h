#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel : public CComponent
{
protected:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_NumMeshes() { return m_iNumMeshes; }
	_matrix	Get_LocalMatrix() { return XMLoadFloat4x4(&m_LocalMatrix); }
	void	Set_LocalMatrix(_fmatrix LocalMatrix) { XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix); }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pModelFilePath);
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT	SetUp_ShaderMaterialResource(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex, MytextureType eType);
	

protected:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

protected:
	_uint					m_iNumMaterials = { 0 };
	vector<MODEL_MATERIAL>	m_ModelMaterials;

protected:
	_float4x4				m_LocalMatrix;


private:
	HRESULT Ready_Meshes(SMODELINFO* pModel);
	HRESULT Ready_Materials(SMODELINFO* pModel, const _tchar* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END