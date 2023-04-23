#include "..\Public\Model.h"

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes) 
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_ModelMaterials(rhs.m_ModelMaterials)
	, m_LocalMatrix(rhs.m_LocalMatrix)
{
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_ModelMaterials)
	{
		for (_uint i = 0; i < MY_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(Material.pMaterialTexture[i]);
		}
	}
}

HRESULT CModel::Initialize_Prototype(const _tchar* pModelFilePath)
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


	if (FAILED(Ready_Meshes(&Model)))
		return E_FAIL;

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

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::SetUp_ShaderMaterialResource(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex, MytextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (nullptr == m_ModelMaterials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType])
		return S_OK;
	
	return m_ModelMaterials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].pMaterialTexture[eType]->Setup_ShaderResource(pShaderCom, pConstantName);
}

void CModel::Set_HangBone(CBone * pBone)
{
	m_pHangBone = pBone;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Ready_Meshes(SMODELINFO* pModel)
{
	if (nullptr == pModel)
		return E_FAIL;


	m_iNumMeshes = pModel->s_iNumMeshes;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, &pModel->s_pMeshes[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	
	return S_OK;
}

HRESULT CModel::Ready_Materials(SMODELINFO* pModel, const _tchar * pModelFilePath)
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

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
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

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();
}
