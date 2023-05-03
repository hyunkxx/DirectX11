#include "..\Public\Model_Anim.h"

#include "Bone.h"
#include "Mesh.h"
#include "Animation.h"
#include "AnimController.h"

#include "Texture.h"
#include "Shader.h"


CModel_Anim::CModel_Anim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CModel(pDevice, pContext)
{
}

CModel_Anim::CModel_Anim(const CModel_Anim & rhs)
	: CModel(rhs)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Animations(rhs.m_Animations)
{
	for (auto pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());


	for (auto& pBone : m_Bones)
	{
		const CBone* pParentBone = pBone->Get_ParentBone();
		if (nullptr != pParentBone)
			pBone->Set_ParentBone(Get_BonePtr(pParentBone->Get_Name()));
	}

	for (auto& pAnim : m_Animations)
		Safe_AddRef(pAnim);
}

CBone * CModel_Anim::Get_BonePtr(const _tchar * pBoneName)
{
	auto& iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
	{
		return !lstrcmp(pBone->Get_Name(), pBoneName);
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}


_uint CModel_Anim::Get_BoneIndex(const _tchar * pBoneName)
{
	for (_uint i = 0; i < m_Bones.size(); ++i)
	{
		if (!lstrcmp(m_Bones[i]->Get_Name(), pBoneName))
			return i;
	}

	MSG_BOX("Get_BoneIndex Failed");
	return UINT_MAX;
}

const _bool CModel_Anim::Get_AnimInterpolating() const
{
	return m_pAnimController->Get_AnimInterpolating();
}

void CModel_Anim::Set_OffsetZero()
{
	for (auto& pBone : m_Bones)
		pBone->Set_OffsetMatrix(XMMatrixIdentity());
}

void CModel_Anim::Set_TrackPos(_float fTrackPos)
{
	m_pAnimController->Set_TrackPos(fTrackPos);
}

vector<class CAnimation*>& CModel_Anim::Get_Animations()
{
	return m_Animations;
}

HRESULT CModel_Anim::Initialize_Prototype(const _tchar * pModelFilePath)
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
	DMODELINFO Model;
	ZeroMemory(&Model, sizeof(DMODELINFO));


	ReadFile(hFile, &Model.s_iNumBones, sizeof(_uint), &dwByte, nullptr);

	Model.s_pRootBone = new BONEINFO[Model.s_iNumBones];
	ZeroMemory(Model.s_pRootBone, sizeof(BONEINFO) * Model.s_iNumBones);

	ReadFile(hFile, Model.s_pRootBone, sizeof(BONEINFO) * Model.s_iNumBones, &dwByte, nullptr);

	ReadFile(hFile, &Model.s_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	Model.s_pMeshes = new DMESHINFO[Model.s_iNumMeshes];
	ZeroMemory(Model.s_pMeshes, sizeof(DMESHINFO) * Model.s_iNumMeshes);

	for (_uint i = 0; i < Model.s_iNumMeshes; ++i)
	{
		ReadFile(hFile, Model.s_pMeshes[i].s_szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &Model.s_pMeshes[i].s_iMaterialIndex, sizeof(_uint) * 4, &dwByte, nullptr);

		Model.s_pMeshes[i].s_pVertices = new VTXDMODEL[Model.s_pMeshes[i].s_iNumVertices];
		ZeroMemory(Model.s_pMeshes[i].s_pVertices, sizeof(VTXDMODEL) * Model.s_pMeshes[i].s_iNumVertices);

		for (_uint j = 0; j < Model.s_pMeshes[i].s_iNumVertices; ++j)
		{
			ReadFile(hFile, &Model.s_pMeshes[i].s_pVertices[j], sizeof(VTXDMODEL), &dwByte, nullptr);
		}

		Model.s_pMeshes[i].s_pFaces = new FACEINDICES32[Model.s_pMeshes[i].s_iNumFaces];
		ZeroMemory(Model.s_pMeshes[i].s_pFaces, sizeof(FACEINDICES32) * Model.s_pMeshes[i].s_iNumFaces);

		for (_uint t = 0; t < Model.s_pMeshes[i].s_iNumFaces; ++t)
		{
			ReadFile(hFile, &Model.s_pMeshes[i].s_pFaces[t], sizeof(FACEINDICES32), &dwByte, nullptr);
		}

		Model.s_pMeshes[i].s_pszBoneNameArray = new _tchar*[Model.s_pMeshes[i].s_iNumBones];

		for (_uint k = 0; k < Model.s_pMeshes[i].s_iNumBones; ++k)
		{
			Model.s_pMeshes[i].s_pszBoneNameArray[k] = new _tchar[MAX_PATH];
			ReadFile(hFile, Model.s_pMeshes[i].s_pszBoneNameArray[k], sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
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


	ReadFile(hFile, &Model.s_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	Model.s_pAnimations = new ANIMINFO[Model.s_iNumAnimations];
	ZeroMemory(Model.s_pAnimations, sizeof(ANIMINFO) * Model.s_iNumAnimations);

	for (_uint i = 0; i < Model.s_iNumAnimations; ++i)
	{
		ReadFile(hFile, Model.s_pAnimations[i].s_szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		ReadFile(hFile, &Model.s_pAnimations[i].s_Duration, sizeof(_double) * 2 + sizeof(_uint), &dwByte, nullptr);

		Model.s_pAnimations[i].s_pChannels = new CHANNELINFO[Model.s_pAnimations[i].s_iNumChannels];
		ZeroMemory(Model.s_pAnimations[i].s_pChannels, sizeof(CHANNELINFO) * Model.s_pAnimations[i].s_iNumChannels);

		for (_uint j = 0; j < Model.s_pAnimations[i].s_iNumChannels; ++j)
		{
			ReadFile(hFile, Model.s_pAnimations[i].s_pChannels[j].s_szName, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

			ReadFile(hFile, &Model.s_pAnimations[i].s_pChannels[j].s_iNumKeyFrames, sizeof(_uint) * 4, &dwByte, nullptr);

			Model.s_pAnimations[i].s_pChannels[j].s_pKeyFrames = new KEYFRAME[Model.s_pAnimations[i].s_pChannels[j].s_iNumKeyFrames];
			ZeroMemory(Model.s_pAnimations[i].s_pChannels[j].s_pKeyFrames, sizeof(KEYFRAME) * Model.s_pAnimations[i].s_pChannels[j].s_iNumKeyFrames);

			for (_uint k = 0; k < Model.s_pAnimations[i].s_pChannels[j].s_iNumKeyFrames; ++k)
			{
				ReadFile(hFile, &Model.s_pAnimations[i].s_pChannels[j].s_pKeyFrames[k], sizeof(KEYFRAME), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	//MSG_BOX("Load Succeed");


	if (FAILED(Ready_Bones(&Model)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(&Model)))
		return E_FAIL;

	if (FAILED(Ready_Materials(&Model, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations(&Model)))
		return E_FAIL;



	// 메모리 해제 코드~
	for (_uint i = 0; i < Model.s_iNumAnimations; ++i)
	{
		for (_uint j = 0; j < Model.s_pAnimations[i].s_iNumChannels; ++j)
		{
			Safe_Delete_Array(Model.s_pAnimations[i].s_pChannels[j].s_pKeyFrames);
		}
		Safe_Delete_Array(Model.s_pAnimations[i].s_pChannels);
	}

	for (_uint i = 0; i < Model.s_iNumMeshes; ++i)
	{
		Safe_Delete_Array(Model.s_pMeshes[i].s_pVertices);
		Safe_Delete_Array(Model.s_pMeshes[i].s_pFaces);

		for (_uint j = 0; j < Model.s_pMeshes[i].s_iNumBones; ++j)
		{
			Safe_Delete_Array(Model.s_pMeshes[i].s_pszBoneNameArray[j]);
		}
		Safe_Delete_Array(Model.s_pMeshes[i].s_pszBoneNameArray);
	}

	Safe_Delete_Array(Model.s_pAnimations);
	Safe_Delete_Array(Model.s_pMtrls);
	Safe_Delete_Array(Model.s_pMeshes);
	Safe_Delete_Array(Model.s_pRootBone);

	XMStoreFloat4x4(&m_LocalMatrix, XMMatrixRotationY(XMConvertToRadians(180.f)));


	return S_OK;
}

HRESULT CModel_Anim::Initialize(void * pArg)
{
	m_pAnimController = CAnimController::Create(m_iNumBones);

	return S_OK;
}

HRESULT CModel_Anim::SetUp_BoneMatrices(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_float4x4 MeshBoneMatrices[256];

	m_Meshes[iMeshIndex]->Get_BoneMatrices(MeshBoneMatrices, this);

	return  pShaderCom->Set_Matrices(pConstantName, MeshBoneMatrices, 256);
}

HRESULT CModel_Anim::SetUp_Animation(_uint iAnimationIndex, _bool bInterpolate, _bool bFootAltitude)
{
	if (iAnimationIndex >= m_iNumAnimations)
		return E_FAIL;

	m_pAnimController->SetUp_Animation(iAnimationIndex, this, bInterpolate, bFootAltitude);

	return S_OK;
}

_float3 CModel_Anim::Play_Animation(_double TimeDelta, _double* pFrameAccOut, _bool* pFinishedOut, _bool bContinue)
{
	_float3 vMove = m_pAnimController->Play_Animation(TimeDelta, this, pFrameAccOut, pFinishedOut, bContinue);

	return vMove;
}

void CModel_Anim::Invalidate_CombinedMatrices()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedMatrix();
	}
}

void CModel_Anim::Update_TargetBones()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Update_TargetBone();
	}
}

void CModel_Anim::Ribbon_TargetBones()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Ribbon_TargetBone();
	}
}

HRESULT CModel_Anim::Ready_Bones(DMODELINFO * pModel)
{
	if (nullptr == pModel)
		return E_FAIL;

	m_iNumBones = pModel->s_iNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(&pModel->s_pRootBone[i]);
		m_Bones.push_back(pBone);

		pBone->Set_ParentBone(Get_BonePtr(pModel->s_pRootBone[i].s_szParentName));
	}

	return S_OK;
}

HRESULT CModel_Anim::Ready_Meshes(DMODELINFO * pModel)
{
	if (nullptr == pModel)
		return E_FAIL;


	m_iNumMeshes = pModel->s_iNumMeshes;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, &pModel->s_pMeshes[i], this);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel_Anim::Ready_Materials(DMODELINFO * pModel, const _tchar * pModelFilePath)
{
	if (nullptr == pModel)
		return E_FAIL;

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
				return E_FAIL;
		}
		m_ModelMaterials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel_Anim::Ready_Animations(DMODELINFO * pModel)
{
	if (nullptr == pModel)
		return E_FAIL;

	m_iNumAnimations = pModel->s_iNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnim = CAnimation::Create(&pModel->s_pAnimations[i], this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}

	return S_OK;
}

CModel_Anim* CModel_Anim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pModelFilePath)
{
	CModel_Anim* pInstance = new CModel_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CModel_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel_Anim::Clone(void * pArg)
{
	CModel_Anim* pInstance = new CModel_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Anim::Free()
{
	__super::Free();

	if (nullptr != m_pAnimController)
		Safe_Release(m_pAnimController);

	for (auto& pAnim : m_Animations)
		Safe_Release(pAnim);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();
}
