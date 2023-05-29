#include "..\Public\Model_Instance.h"

#include "Mesh_Instance.h"
#include "Texture.h"
#include "Shader.h"

CModel_Instance::CModel_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Instance_Meshes(rhs.m_Instance_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_ModelMaterials(rhs.m_ModelMaterials)
	, m_LocalMatrix(rhs.m_LocalMatrix)
	, m_iNumInstance{ rhs.m_iNumInstance }
{
	for (auto& pInstance_Mesh : m_Instance_Meshes)
		Safe_AddRef(pInstance_Mesh);

	for (auto& Material : m_ModelMaterials)
	{
		for (_uint i = 0; i < MY_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(Material.pMaterialTexture[i]);
		}
	}
}

HRESULT CModel_Instance::Initialize_Prototype(const _tchar* pModelFilePath, const _tchar* pInstanceFilePath)
{
	HANDLE		hFile = CreateFile(pModelFilePath,		// 파일 경로와 이름을 명시
		GENERIC_READ,								// 파일 접근 모드(쓰기 전용), GENERIC_READ(읽기 전용)
		NULL,										// 공유 방식, 파일이 열려 있는 상태에서 다른 프로세스가 오픈 할 때 허가하는 것에 대한 설정, NULL을 지정하면 공유하지 않겠다는 의미
		NULL,										// 보안 속성, NULL인 경우 기본값으로 보안 상태를 설정
		OPEN_EXISTING,								// 생성 방식, 해당 파일을 열어서 작업을 할 것인지, 새로 만들 것인지 설정(CREATE_ALWAYS : 파일이 없다면 생성, 있다면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 연다)
		FILE_ATTRIBUTE_NORMAL,						// 파일 속성, FILE_ATTRIBUTE_NORMAL 아무런 속싱 없는 일반적인 파일 생성
		NULL);										// 생성될 파일의 속성을 제공할 템플릿 파일, 안쓰니까 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load File");
		return E_FAIL;
	}


	DWORD dwByte = 0;
	SMODELINFO Model;
	ZeroMemory(&Model, sizeof(SMODELINFO));


	ReadFile(hFile, &Model.s_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	Model.s_pMeshes = new SMESHINFO[Model.s_iNumMeshes];
	ZeroMemory(Model.s_pMeshes, sizeof(SMESHINFO) * Model.s_iNumMeshes);

	for (_uint i = 0; i < Model.s_iNumMeshes; ++i)
	{
		ReadFile(hFile, &Model.s_pMeshes[i].s_szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Model.s_pMeshes[i].s_iMaterialIndex, sizeof(_uint) * 3, &dwByte, nullptr);
		/*ReadFile(hFile, &Model.s_pMeshes[i].s_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Model.s_pMeshes[i].s_iNumVertices, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &Model.s_pMeshes[i].s_iNumFaces, sizeof(_uint), &dwByte, nullptr);*/

		Model.s_pMeshes[i].s_pVertices = new VTXSMODEL[Model.s_pMeshes[i].s_iNumVertices];
		ZeroMemory(Model.s_pMeshes[i].s_pVertices, sizeof(VTXSMODEL) * Model.s_pMeshes[i].s_iNumVertices);

		for (_uint j = 0; j < Model.s_pMeshes[i].s_iNumVertices; ++j)
		{
			ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j], sizeof(VTXSMODEL), &dwByte, nullptr);

			/*ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j].vPosition, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j].vNormal, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j].vTexUV, sizeof(_float2), &dwByte, nullptr);
			ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j].vTangent, sizeof(_float3), &dwByte, nullptr);*/
		}

		Model.s_pMeshes[i].s_pFaces = new FACEINDICES32[Model.s_pMeshes[i].s_iNumFaces];
		ZeroMemory(Model.s_pMeshes[i].s_pFaces, sizeof(FACEINDICES32) * Model.s_pMeshes[i].s_iNumFaces);

		for (_uint t = 0; t < Model.s_pMeshes[i].s_iNumFaces; ++t)
		{
			ReadFile(hFile, &Model.s_pMeshes[i].s_pFaces[t], sizeof(FACEINDICES32), &dwByte, nullptr);
			/*ReadFile(hFile, &Model.s_pMeshes[i].s_pFaces[t]._0, sizeof(unsigned long), &dwByte, nullptr);
			ReadFile(hFile, &Model.s_pMeshes[i].s_pFaces[t]._1, sizeof(unsigned long), &dwByte, nullptr);
			ReadFile(hFile, &Model.s_pMeshes[i].s_pFaces[t]._2, sizeof(unsigned long), &dwByte, nullptr);*/
		}
	}

	ReadFile(hFile, &Model.s_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	Model.s_pMtrls = new MTRLINFO[Model.s_iNumMaterials];
	ZeroMemory(Model.s_pMtrls, sizeof(MTRLINFO) * Model.s_iNumMaterials);

	for (_uint i = 0; i < Model.s_iNumMaterials; ++i)
	{
		for (_uint j = 0; j < MY_TEXTURE_TYPE_MAX; ++j)
		{
			ReadFile(hFile, Model.s_pMtrls[i].s_szNameDotExt[j], sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		}
	}

	CloseHandle(hFile);
	//MSG_BOX("Load Succeed");

	if (FAILED(Load_InstanceData(pInstanceFilePath)))	
		return E_FAIL;

	if (0 < m_iNumInstance)
	{
		if (FAILED(Ready_Meshes(&Model, m_pInstanceMatrix)))
			return E_FAIL;

		Safe_Delete_Array(m_pInstanceMatrix);
	}

	if (FAILED(Ready_Materials(&Model, pModelFilePath)))
		return E_FAIL;

	for (_uint i = 0; i < Model.s_iNumMeshes; ++i)
	{
		Safe_Delete_Array(Model.s_pMeshes[i].s_pVertices);
		Safe_Delete_Array(Model.s_pMeshes[i].s_pFaces);
	}
	Safe_Delete_Array(Model.s_pMeshes);
	Safe_Delete_Array(Model.s_pMtrls);

	XMStoreFloat4x4(&m_LocalMatrix, XMMatrixRotationY(XMConvertToRadians(180.f)));

	return S_OK;
}

HRESULT CModel_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel_Instance::SetUp_ShaderMaterialResource(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, MytextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (nullptr == m_ModelMaterials[m_Instance_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType])
		return S_OK;

	return m_ModelMaterials[m_Instance_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->Setup_ShaderResource(pShaderCom, pConstantName);
}

HRESULT CModel_Instance::SetUp_ShaderMaterialResource_Distinction(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, MytextureType eType, _bool* IsDistinction)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (nullptr == m_ModelMaterials[m_Instance_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType])
	{
		if (nullptr != IsDistinction)
			*IsDistinction = true;

		return S_OK;
	}

	return m_ModelMaterials[m_Instance_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->Setup_ShaderResource(pShaderCom, pConstantName);
}

HRESULT CModel_Instance::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Instance_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel_Instance::Load_InstanceData(const _tchar * pInstanceFilePath)
{
	HANDLE		hFile = CreateFile(pInstanceFilePath,		// 파일 경로와 이름을 명시
		GENERIC_READ,								// 파일 접근 모드(쓰기 전용), GENERIC_READ(읽기 전용)
		NULL,										// 공유 방식, 파일이 열려 있는 상태에서 다른 프로세스가 오픈 할 때 허가하는 것에 대한 설정, NULL을 지정하면 공유하지 않겠다는 의미
		NULL,										// 보안 속성, NULL인 경우 기본값으로 보안 상태를 설정
		OPEN_EXISTING,								// 생성 방식, 해당 파일을 열어서 작업을 할 것인지, 새로 만들 것인지 설정(CREATE_ALWAYS : 파일이 없다면 생성, 있다면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 연다)
		FILE_ATTRIBUTE_NORMAL,						// 파일 속성, FILE_ATTRIBUTE_NORMAL 아무런 속싱 없는 일반적인 파일 생성
		NULL);										// 생성될 파일의 속성을 제공할 템플릿 파일, 안쓰니까 NULL

	DWORD		dwByte = 0;

	if (INVALID_HANDLE_VALUE == hFile)
	{
		//MSG_BOX("Failed to Load File In Model_Instance");
		//return E_FAIL;

		m_iNumInstance = { 1 };

		m_pInstanceMatrix = new _float4x4[m_iNumInstance];
		ZeroMemory(m_pInstanceMatrix, sizeof(_float4x4) * m_iNumInstance);

		XMStoreFloat4x4(&m_pInstanceMatrix[0], XMMatrixIdentity());
	}
	else
	{
		ReadFile(hFile, &m_iNumInstance, sizeof(int), &dwByte, nullptr);

		/*
		if (0 >= m_iNumInstance)
		{
			m_iNumInstance = { 1 };

			m_pInstanceMatrix = new _float4x4[m_iNumInstance];
			ZeroMemory(m_pInstanceMatrix, sizeof(_float4x4) * m_iNumInstance);

			XMStoreFloat4x4(&m_pInstanceMatrix[0], XMMatrixIdentity());
			m_pInstanceMatrix[0]._41 = -100.f;
			m_pInstanceMatrix[0]._42 = -100.f;
		}
		else
		{
			m_pInstanceMatrix = new _float4x4[m_iNumInstance];
			ZeroMemory(m_pInstanceMatrix, sizeof(_float4x4) * m_iNumInstance);

			for (_uint i = 0; i < m_iNumInstance; ++i)
				ReadFile(hFile, &m_pInstanceMatrix[i], sizeof(_float4x4), &dwByte, nullptr);
		}
		*/

		if (0 < m_iNumInstance)
		{
			m_pInstanceMatrix = new _float4x4[m_iNumInstance];
			ZeroMemory(m_pInstanceMatrix, sizeof(_float4x4) * m_iNumInstance);

			for (_uint i = 0; i < m_iNumInstance; ++i)
				ReadFile(hFile, &m_pInstanceMatrix[i], sizeof(_float4x4), &dwByte, nullptr);
		}
		else
			m_iNumInstance = 0;
	}

	CloseHandle(hFile);

	return S_OK;
}


_uint CModel_Instance::Get_TotalNumInstance()
{
	if (m_Instance_Meshes.empty())
		return 0;

	return m_Instance_Meshes[0]->Get_TotalNumInstance();
}

VTXMATRIX * CModel_Instance::Get_TotalInstanceMatrix()
{
	if (m_Instance_Meshes.empty())
		return nullptr;

	return m_Instance_Meshes[0]->Get_TotalInstanceMatrix();
}

VTXMATRIX * CModel_Instance::Get_InstanceMatrix(_uint iInstanceNum)
{
	if (m_Instance_Meshes.empty())
		return nullptr;

	return m_Instance_Meshes[0]->Get_InstanceMatrix(iInstanceNum);
}

void CModel_Instance::Get_PSA_To_InstanceMatrix(_uint iGetNum, SOBJECT_DESC * pOut)
{
	if (m_Instance_Meshes.empty())
		return;

	return m_Instance_Meshes[0]->Get_PSA_To_InstanceMatrix(iGetNum, pOut);
}

_float3 CModel_Instance::Get_Pos_To_InstanceMatrix(_uint iGetNum)
{
	if (m_Instance_Meshes.empty())
		return _float3(0.0f, 0.0f, 0.0f);

	return m_Instance_Meshes[0]->GetPosition_To_InstanceMatrix(iGetNum);
}

void CModel_Instance::Culling(_fmatrix Inverse_WorldMatrix, _float fRadius)
{
	for (auto& pInstanceMash : m_Instance_Meshes)
	{
		pInstanceMash->Culling(Inverse_WorldMatrix, fRadius);
	}
}

HRESULT CModel_Instance::Ready_Meshes(SMODELINFO* pModel, _float4x4* pInstanceMatrices)
{
	if (nullptr == pModel)
		return E_FAIL;

	m_iNumMeshes = pModel->s_iNumMeshes;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh_Instance* pMesh = CMesh_Instance::Create(m_pDevice, m_pContext, &pModel->s_pMeshes[i], m_iNumInstance, pInstanceMatrices);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Instance_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Instance::Ready_Materials(SMODELINFO* pModel, const _tchar * pModelFilePath)
{
	m_iNumMaterials = pModel->s_iNumMaterials;

	_tchar szDrive[MAX_PATH] = TEXT("");
	_tchar szDir[MAX_PATH] = TEXT("");

	_wsplitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	_tchar szCommonDir[MAX_PATH] = TEXT("");

	lstrcpy(szCommonDir, szDrive);
	lstrcat(szCommonDir, szDir);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODEL_MATERIAL	ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODEL_MATERIAL));

		//aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 1; j < MY_TEXTURE_TYPE_MAX; j++)
		{
			_tchar szNameDotExt[MAX_PATH] = TEXT("");
			lstrcpy(szNameDotExt, pModel->s_pMtrls[i].s_szNameDotExt[j]);
			if (!lstrcmp(szNameDotExt, TEXT("")))
			{
				continue;
			}

			_tchar szFullPath[MAX_PATH] = TEXT("");
			lstrcpy(szFullPath, szCommonDir);
			lstrcat(szFullPath, szNameDotExt);

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);

			if (nullptr == ModelMaterial.pMaterialTexture[j])
			{
				int a = 10;
				return E_FAIL;
			}

		}
		m_ModelMaterials.push_back(ModelMaterial);
	}

	return S_OK;
}

CModel_Instance * CModel_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pModelFilePath, const _tchar* pInstanceFilePath)
{
	CModel_Instance* pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pInstanceFilePath)))
	{
		MSG_BOX("Failed to Create : CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel_Instance::Clone(void * pArg)
{
	CModel_Instance* pInstance = new CModel_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instance::Free()
{
	__super::Free();

	for (auto& Material : m_ModelMaterials)
	{
		for (_uint i = 0; i < MY_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_Release(Material.pMaterialTexture[i]);
		}
	}
	m_ModelMaterials.clear();

	for (auto& pInstance_Mesh : m_Instance_Meshes)
		Safe_Release(pInstance_Mesh);

	m_Instance_Meshes.clear();

}
