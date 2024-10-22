#include "stdafx.h"
#include "..\Public\MapObject.h"

#include "GameInstance.h"
#include "GameMode.h"

CMapObject::CMapObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_EditionDesc, sizeof(SMAP_OBJECT_EDITION_DESC));
}

CMapObject::CMapObject(const CMapObject & rhs)
	: CGameObject(rhs)
	, m_EditionDesc(rhs.m_EditionDesc)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_EditionDesc, pArg, sizeof(SMAP_OBJECT_EDITION_DESC));

	SetUp_LevelFilePath(LEVEL_ANYWHERE, m_EditionDesc.pEditionFilePath, *m_szEdition_FilePath);

	if (FAILED(Load_Edition()))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_uint		iNumInstance = { 0 };

	iNumInstance = m_pModelCom->Get_TotalNumInstance();

	if (0 >= iNumInstance)
		Destroy();
	
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);

	switch (m_EditionDesc.iTypeID)
	{
	case Client::CMapObject::ID_TREE:
		break;
	case Client::CMapObject::ID_ROCK:
		m_EditionDesc.fCullingRatio = 0.f;
		break;
	case Client::CMapObject::ID_FLOOR:
		break;
	case Client::CMapObject::ID_STAIRS:
		break;
	case Client::CMapObject::ID_GRASS:
		break;
	case Client::CMapObject::ID_GRASS_MASK:
		break;
	case Client::CMapObject::ID_VIN:
		break;
	case Client::CMapObject::ID_VEG:
		break;
	case Client::CMapObject::ID_SHR:
		break;
	case Client::CMapObject::ID_STRUCTURE:
		m_EditionDesc.fCullingRatio = 0.f;
		break;
	case Client::CMapObject::ID_PIL:
		m_EditionDesc.fCullingRatio = 0.f;
		break;
	case Client::CMapObject::ID_STATUE:
		break;
	case Client::CMapObject::ID_NOIROCK:
		break;
	case Client::CMapObject::ID_TOF_GRASS:
		break;
	case Client::CMapObject::ID_END:
		break;
	default:
		break;
	}

	//m_EditionDesc.fCullingRatio = 0.f;

	//SetUp_Random_Shake();
	//SetUp_Shake();

	SetState(CGameObject::STATE::NONTICK);
	
	return S_OK;
}

void CMapObject::Start()
{
	__super::Start();

	if (m_EditionDesc.iTypeID != CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK && m_EditionDesc.iTypeID != CMapObject::MAPOBJECT_TYPEID::ID_TOF_GRASS)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
}

void CMapObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapObject::LateTick(_double TimeDelta)
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	m_fTimeeAcc += (_float)TimeDelta;

	__super::LateTick(TimeDelta);

	_bool bCulling = false;
	if (nullptr != m_pModelCom && 0.0f < m_EditionDesc.fCullingRatio)
		m_pModelCom->Culling(m_pMainTransform->Get_WorldMatrixInverse(), m_EditionDesc.fCullingRatio);

	if (nullptr != m_pRendererCom)
	{
		m_pModelCom->Get_Pos_To_InstanceMatrix(m_EditionDesc.iTypeID);
		_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&pGame->Get_CamPosition()) - XMLoadFloat3(&m_pModelCom->Get_Pos_To_InstanceMatrix(m_EditionDesc.iTypeID))));

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
	}
}

HRESULT CMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	switch (m_EditionDesc.iTypeID)
	{
	case CMapObject::MAPOBJECT_TYPEID::ID_TREE:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_ROCK:
		if (FAILED(Render_Rock()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_FLOOR:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STAIRS:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK:
		/*if (FAILED(Render_Default()))
			return E_FAIL;*/
		if (FAILED(Render_SubEditionColor_Mask()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VIN:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VEG:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_SHR:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_STRUCTURE:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_PIL:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STATUE:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_NOIROCK:
		if (FAILED(Render_Default_SelfShadow()))
			return E_FAIL;
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_TOF_GRASS:
		if (FAILED(Render_Grass()))
			return E_FAIL;
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CMapObject::RenderShadow()
{
	if (FAILED(SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Render_Rock()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pDiffuseTexture[m_EditionDesc.iDiffuseTex_ID]->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture");

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		if (STATIC_IMAGE::ROCK_MASK_NONE == m_EditionDesc.iMaskTex_ID)
			m_iShaderPassID = 0;
		else
		{
			if (FAILED(pGameInstance->SetupSRV(m_EditionDesc.iMaskTex_ID, m_pShaderCom, "g_MaskTexture")))
				return E_FAIL;

			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ROCK_SUB_D, m_pShaderCom, "g_SubDiffuseTexture")))
				return E_FAIL;

			m_iShaderPassID = 4;
		}

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Render_SubEditionColor_Mask()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pGrassMaskTexture[m_EditionDesc.iMaskTex_ID]->Setup_ShaderResource(m_pShaderCom, "g_MaskTexture");

		m_iShaderPassID = 3;

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Render_Default()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		m_iShaderPassID = 0;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		if (true == m_EditionDesc.UseEditionColor && i == m_EditionDesc.iEditionColor_MeshNum)
			m_IsUse_EdtionColor = true;
		else
			m_IsUse_EdtionColor = false;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseEditionColor", &m_IsUse_EdtionColor, sizeof(_bool))))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Render_Default_SelfShadow()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		m_iShaderPassID = 1;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		_bool bGlow = { false };

		if (m_EditionDesc.iSIMD_ID == SIMODEL::SIMD_TREE_31)
		{
			if (i == 0)
				bGlow = true;
			else
				bGlow = false;

			if (FAILED(m_pShaderCom->SetRawValue("g_IsUseGlow", &bGlow, sizeof(_bool))))
				return E_FAIL;

		}
		if (m_EditionDesc.iSIMD_ID == SIMODEL::SIMD_SHR_4)
		{
			if (i == 2)
				bGlow = true;
			else
				bGlow = false;

			if (FAILED(m_pShaderCom->SetRawValue("g_IsUseGlow", &bGlow, sizeof(_bool))))
				return E_FAIL;
		}

		if (true == m_EditionDesc.UseEditionColor && i == m_EditionDesc.iEditionColor_MeshNum)
			m_IsUse_EdtionColor = true;
		else
			m_IsUse_EdtionColor = false;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseEditionColor", &m_IsUse_EdtionColor, sizeof(_bool))))
			return E_FAIL;

		if (true == m_EditionDesc.IsDetail_Shake)
		{
			if (i == m_EditionDesc.iShake_MeshNum)
				m_EditionDesc.UseShake = true;
			else
				m_EditionDesc.UseShake = false;

			if (FAILED(m_pShaderCom->SetRawValue("m_IsUseShake", &m_EditionDesc.UseShake, sizeof(_bool))))
				return E_FAIL;
		}

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapObject::Render_Grass()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;

		m_iShaderPassID = 5;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		if (true == m_EditionDesc.UseEditionColor && i == m_EditionDesc.iEditionColor_MeshNum)
			m_IsUse_EdtionColor = true;
		else
			m_IsUse_EdtionColor = false;

		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseEditionColor", &m_IsUse_EdtionColor, sizeof(_bool))))
			return E_FAIL;

		if (true == m_EditionDesc.IsDetail_Shake)
		{
			if (i == m_EditionDesc.iShake_MeshNum)
				m_EditionDesc.UseShake = true;
			else
				m_EditionDesc.UseShake = false;

			if (FAILED(m_pShaderCom->SetRawValue("m_IsUseShake", &m_EditionDesc.UseShake, sizeof(_bool))))
				return E_FAIL;
		}

		m_pShaderCom->Begin(m_iShaderPassID);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CMapObject::SetUp_LevelFilePath(_uint iLevelID, const _tchar* pFilePath, _Out_ _tchar& szResultFilePath)
{
	ZeroMemory(&szResultFilePath, sizeof(_tchar) * MAX_PATH);

	_tchar			szLevelFilePath[MAX_PATH] = TEXT("../../Data/");
	_tchar			szDataFilePath[MAX_PATH] = TEXT("");

	lstrcat(szDataFilePath, pFilePath);

	switch (iLevelID)
	{
	case LEVEL_ID::LEVEL_GAMEPLAY:
		lstrcat(szLevelFilePath, TEXT("GamePlay/"));
		break;
	case LEVEL_ID::LEVEL_CITY:
		lstrcat(szLevelFilePath, TEXT("City/"));
		break;
	case LEVEL_ID::LEVEL_FOREST:
		lstrcat(szLevelFilePath, TEXT("Forest/"));
		break;
	case LEVEL_ID::LEVEL_CROWN:
		lstrcat(szLevelFilePath, TEXT("Crown/"));
		break;
	default:
		break;
	}

	lstrcat(&szResultFilePath, szLevelFilePath);
	lstrcat(&szResultFilePath, szDataFilePath);

}

HRESULT CMapObject::Load_Edition()
{
	HANDLE		hFile = CreateFile(m_szEdition_FilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in CMapObject : EditionData");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	Load_EditionID(hFile, dwByte);

	switch (m_EditionDesc.iTypeID)
	{
	case CMapObject::MAPOBJECT_TYPEID::ID_TREE:
		Load_EditionColor(hFile, dwByte);
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_ROCK:
		Load_DiffuseTexID(hFile, dwByte);
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_FLOOR:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STAIRS:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK:
		Load_SubEditionColor_Mask(hFile, dwByte);
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VIN:
		Load_EditionColor(hFile, dwByte);
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VEG:
		Load_EditionColor(hFile, dwByte);
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_SHR:
		Load_EditionColor(hFile, dwByte);
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_STRUCTURE:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_PIL:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STATUE:
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_NOIROCK:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_TOF_GRASS:
		break;

	default:
		break;
	}

	CloseHandle(hFile);

	return S_OK;
}

void CMapObject::Load_EditionID(HANDLE& _hFile, DWORD& _dwByte)
{
	ReadFile(_hFile, &m_EditionDesc.iSIMD_ID, sizeof(_uint), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.iTypeID, sizeof(_uint), &_dwByte, nullptr);

	ReadFile(_hFile, &m_EditionDesc.UseGlow, sizeof(_bool), &_dwByte, nullptr);

	ReadFile(_hFile, &m_EditionDesc.UseShake, sizeof(_bool), &_dwByte, nullptr);

	ReadFile(_hFile, &m_EditionDesc.fShakePower, sizeof(_float), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.fShakeRange_Ratio, sizeof(_float), &_dwByte, nullptr);

	ReadFile(_hFile, &m_EditionDesc.IsDetail_Shake, sizeof(_bool), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.iShake_MeshNum, sizeof(_uint), &_dwByte, nullptr);
}

void CMapObject::Load_EditionColor(HANDLE& _hFile, DWORD& _dwByte)
{
	ReadFile(_hFile, &m_EditionDesc.UseEditionColor, sizeof(_bool), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.iEditionColor_MeshNum, sizeof(_uint), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.vEditionColor, sizeof(_float3), &_dwByte, nullptr);
}

void CMapObject::Load_DiffuseTexID(HANDLE& _hFile, DWORD& _dwByte)
{
	ReadFile(_hFile, &m_EditionDesc.iDiffuseTex_ID, sizeof(_uint), &_dwByte, nullptr);
}

void CMapObject::Load_SubEditionColor_Mask(HANDLE& _hFile, DWORD& _dwByte)
{
	ReadFile(_hFile, &m_EditionDesc.vEditionColor, sizeof(_float3), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.iMaskTex_ID, sizeof(_uint), &_dwByte, nullptr);
	ReadFile(_hFile, &m_EditionDesc.vSubEditionColor, sizeof(_float3), &_dwByte, nullptr);
}

void CMapObject::SetUp_Random_Shake()
{
	// 초기 랜덤 값으로 세팅.
	m_EditionDesc.fShakePower = m_EditionDesc.fShakePower * fabsf(sinf(_float(rand()))) * m_EditionDesc.fShakePower;
	m_EditionDesc.fShakeRange_Ratio = m_EditionDesc.fShakeRange_Ratio * fabsf(sinf(_float(rand()))) * m_EditionDesc.fShakeRange_Ratio;
}

void CMapObject::SetUp_Shake()
{
	// 나중에 급하게 클라에서 수정할때.
	m_EditionDesc.UseShake;
	m_EditionDesc.fShakePower;
	m_EditionDesc.fShakeRange_Ratio;
	m_EditionDesc.IsDetail_Shake;
	m_EditionDesc.iShake_MeshNum;

	switch (m_EditionDesc.iTypeID)
	{
	case CMapObject::MAPOBJECT_TYPEID::ID_TREE:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_ROCK:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_FLOOR:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STAIRS:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VIN:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_VEG:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_SHR:
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_STRUCTURE:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_PIL:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_STATUE:
		break;

	case CMapObject::MAPOBJECT_TYPEID::ID_NOIROCK:
		break;
	case CMapObject::MAPOBJECT_TYPEID::ID_TOF_GRASS:
		break;

	default:
		break;
	}
}

HRESULT CMapObject::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;
	m_pMainTransform->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, m_EditionDesc.iSIMD_ID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL_INSTANCE,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (CMapObject::MAPOBJECT_TYPEID::ID_ROCK == m_EditionDesc.iTypeID)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_1,
			TEXT("com_texture_d_1"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_1])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_2,
			TEXT("com_texture_d_2"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_2])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_3,
			TEXT("com_texture_d_3"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_3])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_4,
			TEXT("com_texture_d_4"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_4])))
			return E_FAIL;
	}

	if (CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK == m_EditionDesc.iTypeID)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_1,
			TEXT("com_texture_gm_1"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_1])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_2,
			TEXT("com_texture_gm_2"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_2])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_3,
			TEXT("com_texture_gm_3"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_3])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_4,
			TEXT("com_texture_gm_4"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_4])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_5,
			TEXT("com_texture_gm_5"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_5])))
			return E_FAIL;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::GRASS_MASK_6,
			TEXT("com_texture_gm_6"), (CComponent**)&m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_6])))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CMapObject::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vEditionColor", &m_EditionDesc.vEditionColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vSubEditionColor", &m_EditionDesc.vSubEditionColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_IsUseGlow", &m_EditionDesc.UseGlow, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &m_fTimeeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("m_IsUseShake", &m_EditionDesc.UseShake, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_fShakePower", &m_EditionDesc.fShakePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetRawValue("g_fShakeRange_Ratio", &m_EditionDesc.fShakeRange_Ratio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::SetUp_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->GetBakeLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	return S_OK;
}

CMapObject * CMapObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapObject* pInstance = new CMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapObject::Clone(void * pArg)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	if (CMapObject::MAPOBJECT_TYPEID::ID_ROCK == m_EditionDesc.iTypeID)
	{
		for (_uint i = 0; i < ROCK_DIFFUSE_KINDS::RD_END; i++)
			Safe_Release(m_pDiffuseTexture[i]);
	}

	if (CMapObject::MAPOBJECT_TYPEID::ID_GRASS_MASK == m_EditionDesc.iTypeID)
	{
		for (_uint i = 0; i < GRASS_MASK_KINDS::GM_END; i++)
			Safe_Release(m_pGrassMaskTexture[i]);
	}

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
