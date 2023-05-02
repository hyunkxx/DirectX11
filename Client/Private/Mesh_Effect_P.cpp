#include "stdafx.h"
#include "..\Public\Mesh_Effect_P.h"

#include "Gameinstance.h"
#include "Model.h"

CMesh_Effect_P::CMesh_Effect_P(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CMesh_Effect_P::CMesh_Effect_P(const CMesh_Effect_P & rhs)
	: CEffect(rhs)
{
}

HRESULT CMesh_Effect_P::Initialize_Prototype(const char* pFileTag , const EFFECT_DESC & MeshDesc)
{
	memcpy(&m_EffectDesc, &MeshDesc, sizeof(EFFECT_DESC));

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Component(pFileTag)))
		return E_FAIL;

	m_pMainTransform->Set_Scale(_float3(0.1f, 0.1f, 0.1f));
	_matrix IdenMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ParentsMatrix, IdenMatrix);

	return S_OK;
}

HRESULT CMesh_Effect_P::Initialize(void * pArg)
{
	return S_OK;
}

void CMesh_Effect_P::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fLifeAcc += TimeDelta;

	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
		return;

	m_fEffectAcc += TimeDelta;

	if (m_fEffectAcc > m_EffectDesc.fEffectTime)
	{
		if (m_EffectDesc.bLoop)
		{
			m_fDelayAcc += TimeDelta;

			if (m_fDelayAcc > m_EffectDesc.fDelayTime)
			{
				m_fDelayAcc = 0.f;
				m_fEffectAcc = 0.f;
				if (nullptr != m_pParentsMatrix)
					memcpy(&m_ParentsMatrix, m_pParentsMatrix, sizeof(_float4x4));
			}
		}
		m_EffectDesc.vUV = { 0.f , 0.f };
		m_fFrameAcc = { 0.f };
	}
}

void CMesh_Effect_P::LateTick(_double TimeDelta)
{
	if (m_fLifeAcc >= m_EffectDesc.fLifeTime)
	{
		m_bFinish = true;
		return;
	}

	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
		return;

	if (m_fEffectAcc >= m_EffectDesc.fEffectTime)
		return;

	__super::LateTick(TimeDelta);

	SetUp_Linear();
	XMStoreFloat2(&m_EffectDesc.vUV, XMLoadFloat2(&m_EffectDesc.vUV) + XMLoadFloat2(&m_EffectDesc.fUVSpeed) * TimeDelta);
	m_pMainTransform->SetRotationXYZ(m_EffectDesc.vCurAngle);
	m_WorldMatrix = m_pMainTransform->Get_WorldMatrix();

	_float4 NewRight = _float4(m_WorldMatrix._11 * m_EffectDesc.vCurScale.x,
		m_WorldMatrix._12 * m_EffectDesc.vCurScale.x,
		m_WorldMatrix._13 * m_EffectDesc.vCurScale.x,
		m_WorldMatrix._14);

	_float4 NewUp = _float4(m_WorldMatrix._21 * m_EffectDesc.vCurScale.y,
		m_WorldMatrix._22  * m_EffectDesc.vCurScale.y,
		m_WorldMatrix._23  * m_EffectDesc.vCurScale.y,
		m_WorldMatrix._24);

	_float4 NewLook = _float4(m_WorldMatrix._31  * m_EffectDesc.vCurScale.z,
		m_WorldMatrix._32  * m_EffectDesc.vCurScale.z,
		m_WorldMatrix._33  * m_EffectDesc.vCurScale.z,
		m_WorldMatrix._34);

	memcpy(((_float4*)&m_WorldMatrix + 0), &NewRight, sizeof(_float3));
	memcpy(((_float4*)&m_WorldMatrix + 1), &NewUp, sizeof(_float3));
	memcpy(((_float4*)&m_WorldMatrix + 2), &NewLook, sizeof(_float3));

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDER_GROUP)m_EffectDesc.iRenderGroup, this);
}

HRESULT CMesh_Effect_P::Render()
{
	if (nullptr == m_pModelCom)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pShaderCom->Begin(m_EffectDesc.iPass);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CMesh_Effect_P::RenderGUI()
{
}

void CMesh_Effect_P::Play_Effect(_float4x4 * pWorldMatrix, _bool bTracking)
{
	if (nullptr == pWorldMatrix)
		return;

	m_bFinish = false;
	m_fDelayAcc = 0.f;
	m_fEffectAcc = 0.f;
	m_fLifeAcc = 0.f;
	m_pParentsMatrix = nullptr;

	if (m_EffectDesc.bTracking)
	{
		m_pParentsMatrix = pWorldMatrix;
	}
	else
	{
		m_pParentsMatrix = pWorldMatrix;
		memcpy(&m_ParentsMatrix, pWorldMatrix, sizeof(_float4x4));
	}
}

CEffect::EFFECT_DESC CMesh_Effect_P::Get_Effect_Desc()
{
	return m_EffectDesc;
}

HRESULT CMesh_Effect_P::Add_Component(const char* pFileTag)
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER, L"com_renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC Transform_Desc;
	ZeroMemory(&Transform_Desc, sizeof Transform_Desc);

	Transform_Desc.fMoveSpeed = 5.f;
	Transform_Desc.fRotationSpeed = XMConvertToRadians(60.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM, L"com_transform", (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, SHADER::SKILL_MODEL, L"com_shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Add_Texture(pFileTag, m_EffectDesc.TexName, &m_pDiffuseTextureCom);
	Add_Model(pFileTag, m_EffectDesc.MeshName, &m_pModelCom);

	if (m_EffectDesc.bNoiseTexure)
		Add_Texture(pFileTag, m_EffectDesc.NoiseTexName, &m_pNoiseTextureCom);

	if (m_EffectDesc.bDissolveTexure)
	{
		Add_Texture(pFileTag, m_EffectDesc.DissolveTexName, &m_pDissolveTextureCom);

		/*if (5 < strnlen_s(m_EffectDesc.DissolveTexName, sizeof(MAX_PATH)))
			Add_Texture(pFileTag, m_EffectDesc.DissolveTexName, &m_pDissolveTextureCom);
		else
			m_EffectDesc.bDissolveTexure = false;*/
	}
	

	return S_OK;
}

HRESULT CMesh_Effect_P::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4 LastMatirx;
	if (m_EffectDesc.bTracking)
	{
		if (nullptr == m_pParentsMatrix)
		{
			XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&m_ParentsMatrix));
		}
		else
			XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(m_pParentsMatrix));
	}
	else
		XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&m_ParentsMatrix));

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &LastMatirx)))
		return E_FAIL;

	if (FAILED(m_pDiffuseTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (m_EffectDesc.bDissolveTexure)
	{
		if (FAILED(m_pDissolveTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;
	}
	if (m_EffectDesc.bNoiseTexure)
	{
		/*if (FAILED(m_pDiffuseTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;*/
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_fAlpha", &m_EffectDesc.fCurAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vColor", &m_EffectDesc.vTextrueColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vUV", &m_EffectDesc.vUV, sizeof(_float2))))
		return E_FAIL;


	if (m_EffectDesc.bDissolveTexure &&  nullptr != m_pDissolveTextureCom)
	{
		if (FAILED(m_pDissolveTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;

		_float fTimeAcc = (m_fEffectAcc / m_EffectDesc.fEffectTime) * 3.f;
		if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMesh_Effect_P::SetUp_Linear()
{
	_float fLerp = (m_fEffectAcc) / (m_EffectDesc.fEffectTime);

	_vector vStart, vEnd;

	//Color
	vStart = XMVectorSet(m_EffectDesc.vStartColor.x, m_EffectDesc.vStartColor.y, m_EffectDesc.vStartColor.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndColor.x, m_EffectDesc.vEndColor.y, m_EffectDesc.vEndColor.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vTextrueColor, XMVectorLerp(vStart, vEnd, fLerp));

	// Angle
	vStart = XMVectorSet(m_EffectDesc.vStartAngle.x, m_EffectDesc.vStartAngle.y, m_EffectDesc.vStartAngle.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndAngle.x, m_EffectDesc.vEndAngle.y, m_EffectDesc.vEndAngle.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vCurAngle, XMVectorLerp(vStart, vEnd, fLerp));

	vStart = XMVectorSet(m_EffectDesc.vStartScale.x, m_EffectDesc.vStartScale.y, m_EffectDesc.vStartScale.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndScale.x, m_EffectDesc.vEndScale.y, m_EffectDesc.vEndScale.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vCurScale, XMVectorLerp(vStart, vEnd, fLerp));

	vStart = XMVectorSet(m_EffectDesc.fStartAlpha, m_EffectDesc.fStartAlpha, m_EffectDesc.fStartAlpha, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.fEndAlpha, m_EffectDesc.fEndAlpha, m_EffectDesc.fEndAlpha, 0.f);
	m_EffectDesc.fCurAlpha = XMVectorGetX(XMVectorLerp(vStart, vEnd, fLerp));

	vStart = XMVectorSet(m_EffectDesc.vStartPosition.x, m_EffectDesc.vStartPosition.y, m_EffectDesc.vStartPosition.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndPosition.x, m_EffectDesc.vEndPosition.y, m_EffectDesc.vEndPosition.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vCurPosition, XMVectorLerp(vStart, vEnd, fLerp));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_EffectDesc.vCurPosition));
}

void CMesh_Effect_P::Add_Texture(const char * pFileTag, const char * TextureName, CTexture ** pTexture)
{
	char EndTag[MAX_PATH] = ".dds";
	char TextureTag[MAX_PATH] = "";
	_tchar TextureTag2[MAX_PATH] = L"";

	strcpy_s(TextureTag, pFileTag);
	strcat_s(TextureTag, TextureName);
	strcat_s(TextureTag, EndTag);
	MultiByteToWideChar(CP_ACP, 0, TextureTag, (_int)strlen(TextureTag),
		TextureTag2, MAX_PATH);

	*pTexture = CTexture::Create(m_pDevice, m_pContext, TextureTag2);
	_int i = 0;

}

void CMesh_Effect_P::Add_Model(const char * pFileTag, const char * TextureName, CModel** pModel)
{
	char EndTag[MAX_PATH] = ".smdl";
	char TextureTag[MAX_PATH] = "";
	_tchar TextureTag2[MAX_PATH] = L"";

	strcpy_s(TextureTag, pFileTag);
	strcat_s(TextureTag, TextureName);
	strcat_s(TextureTag, EndTag);
	MultiByteToWideChar(CP_ACP, 0, TextureTag, (_int)strlen(TextureTag),
		TextureTag2, MAX_PATH);

	*pModel = CModel::Create(m_pDevice, m_pContext, TextureTag2);
	_int i = 0;
}



CMesh_Effect_P * CMesh_Effect_P::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pFileTag, const EFFECT_DESC & MeshDesc)
{
	CMesh_Effect_P*		pInstance = new CMesh_Effect_P(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFileTag , MeshDesc)))
	{
		MSG_BOX("Failed to Created : CMesh_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMesh_Effect_P::Clone(void * pArg)
{
	CGameObject*		pInstance = new CMesh_Effect_P(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Effect_P::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pMainTransform);
}
