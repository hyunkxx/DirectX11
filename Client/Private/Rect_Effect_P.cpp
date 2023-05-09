#include "stdafx.h"
#include "..\Public\Rect_Effect_P.h"

#include "Gameinstance.h"

CRect_Effect_P::CRect_Effect_P(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CRect_Effect_P::CRect_Effect_P(const CRect_Effect_P & rhs)
	: CEffect(rhs)
{

}

HRESULT CRect_Effect_P::Initialize_Prototype(const char* pFileTag , const EFFECT_DESC & MeshDesc)
{
	memcpy(&m_EffectDesc, &MeshDesc, sizeof(EFFECT_DESC));
	m_EffectDesc.eEffectType = TYPE_RECT;

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Component(pFileTag , m_EffectDesc)))
		return E_FAIL;

	_matrix IdenMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ParentsMatrix, IdenMatrix);

	return S_OK;
}

HRESULT CRect_Effect_P::Initialize(void * pArg)
{
	return S_OK;
}

void CRect_Effect_P::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fLifeAcc += (_float)TimeDelta;
	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
		return;

	m_fEffectAcc += (_float)TimeDelta;

	Distortion_Tick(TimeDelta);
	Loop_Check(TimeDelta);

}

void CRect_Effect_P::LateTick(_double TimeDelta)
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
	Setup_Matrix();
	XMStoreFloat2(&m_EffectDesc.vUV, XMLoadFloat2(&m_EffectDesc.vUV) + XMLoadFloat2(&m_EffectDesc.fUVSpeed) * (_float)TimeDelta);

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDER_GROUP)m_EffectDesc.iRenderGroup, this);
}

HRESULT CRect_Effect_P::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_EffectDesc.iPass);

	if (CRenderer::RENDER_SSD == m_EffectDesc.iRenderGroup || CRenderer::RENDER_GLOWSSD == m_EffectDesc.iRenderGroup)
		m_pVIBoxCom->Render();
	else
		m_pVIBufferCom->Render();

	return S_OK;
}

void CRect_Effect_P::RenderGUI()
{

}

void CRect_Effect_P::Play_Effect(_float4x4 * pWorldMatrix, _bool bTracking)
{
	if (nullptr == pWorldMatrix)
		return;

	m_bDistortion = false;
	m_bFinish = false;
	m_fDelayAcc = 0.f;
	m_fEffectAcc = 0.f;
	m_fLifeAcc = 0.f;
	m_pParentsMatrix = nullptr;
	DISTORTION_POWER = START_DIS_POWER;
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

EFFECT_DESC CRect_Effect_P::Get_Effect_Desc()
{
	return m_EffectDesc;
}

HRESULT CRect_Effect_P::Add_Component(const char* pFileTag , const EFFECT_DESC & ParticleDesc)
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER, L"com_renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC Transform_Desc;
	ZeroMemory(&Transform_Desc, sizeof Transform_Desc);

	Transform_Desc.fMoveSpeed = 5.f;
	Transform_Desc.fRotationSpeed = XMConvertToRadians(60.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM, L"com_transform", (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, SHADER::SKILL_RECT, L"com_shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::VIBUIFFER_RECT_NOR, L"com_buffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::VIBUIFFER_SSD_BOX, L"com_ssdbox", (CComponent**)&m_pVIBoxCom)))
		return E_FAIL;

	if (m_EffectDesc.bTexture)
	{
		if (FAILED(Add_Texture(pFileTag, m_EffectDesc.TexName, &m_pTextureCom)))
			return E_FAIL;
	}

	if (m_EffectDesc.bNoiseTexure)
	{
		if (FAILED(Add_Texture(pFileTag, m_EffectDesc.NoiseTexName, &m_pNoiseTextureCom)))
			return E_FAIL;
	}

	if (m_EffectDesc.bDissolveTexure)
	{
		if (FAILED(Add_Texture(pFileTag, m_EffectDesc.DissolveTexName, &m_pDissolveTextureCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRect_Effect_P::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

#pragma region  Default_Matix_RawValue

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_ResultMatirx)))
		return E_FAIL;

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

	_float2 vUV;
	vUV.x = m_EffectDesc.vUV.x + m_EffectDesc.vMinScale.x;
	vUV.y = m_EffectDesc.vUV.y + m_EffectDesc.vMinScale.y;

	if (FAILED(m_pShaderCom->SetRawValue("g_vUV", &vUV, sizeof(_float2))))
		return E_FAIL;

#pragma endregion


#pragma region Texture

	if (m_EffectDesc.bTexture && nullptr != m_pTextureCom)
	{
		if (FAILED(m_pTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}

	if (m_EffectDesc.bDissolveTexure &&  nullptr != m_pDissolveTextureCom)
	{
		if (FAILED(m_pDissolveTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;

		_float fTimeAcc = (m_fEffectAcc / m_EffectDesc.fEffectTime) * 3.f;
		if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;
	}

	if (m_EffectDesc.bNoiseTexure && nullptr != m_pNoiseTextureCom)
	{
		if (FAILED(m_pNoiseTextureCom->Setup_ShaderResource(m_pShaderCom, "g_NormalTexture")))
			return E_FAIL;
	}

#pragma endregion

	//Decal
	if (7 < m_EffectDesc.iPass)
	{
		if (m_EffectDesc.iRenderGroup == (_int)CRenderer::RENDER_SSD || m_EffectDesc.iRenderGroup == (_int)CRenderer::RENDER_GLOWSSD)
		{
			if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Diffuse"), "g_BackTexture")))
				return E_FAIL;
			if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Normal"), "g_BackNormalTexture")))
				return E_FAIL;
			if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
				return E_FAIL;
			if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Static_Depth"), "g_Static_DepthTexture")))
				return E_FAIL;

			_float4x4 InvViewMat = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);
			_float4x4 InvProjMat = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::TS_PROJ);
			_float4x4 InvWorldMatrix, InvWV;

			XMStoreFloat4x4(&InvWorldMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ResultMatirx)));
			XMStoreFloat4x4(&InvWV, XMLoadFloat4x4(&InvViewMat) * XMLoadFloat4x4(&InvWorldMatrix));

			if (FAILED(m_pShaderCom->SetMatrix("g_InvWV", &InvWV)))
				return E_FAIL;
			_float4 vInvPorj = _float4(1.f, 1.f, 1.f, 1.f);
			XMStoreFloat4(&vInvPorj, XMVector4Transform(XMLoadFloat4(&vInvPorj), XMLoadFloat4x4(&InvProjMat)));
			vInvPorj.y = -vInvPorj.y;

			if (FAILED(m_pShaderCom->SetRawValue("g_InvProj", &vInvPorj, sizeof(_float4))))
				return E_FAIL;
		}
	}	

	if (m_bDistortion)
	{
		if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_Glow_Result"), "g_BackTexture")))
			return E_FAIL;
		if (FAILED(CGameInstance::GetInstance()->Set_ShaderRenderTargetResourceView(m_pShaderCom, TEXT("Target_DistortionMap"), "g_DepthTexture")))
			return E_FAIL;

		_float fTimeAcc = (m_fEffectAcc / m_EffectDesc.fEffectTime) * 3.f;
		if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetRawValue("g_fDisPower", &m_EffectDesc.vMaxScale.z, sizeof(_float))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CRect_Effect_P::Add_Texture(const char * pFileTag, const char * TextureName, CTexture ** pTexture)
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
	
	return S_OK;
}

void CRect_Effect_P::SetUp_Linear()
{
	_float fLerp = (m_fEffectAcc) / (m_EffectDesc.fEffectTime);

	_vector vStart, vEnd;

	//Color
	vStart = XMVectorSet(m_EffectDesc.vStartColor.x, m_EffectDesc.vStartColor.y, m_EffectDesc.vStartColor.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndColor.x, m_EffectDesc.vEndColor.y, m_EffectDesc.vEndColor.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vTextrueColor, XMVectorLerp(vStart, vEnd, fLerp * 3.f));

	// Angle
	vStart = XMVectorSet(m_EffectDesc.vStartAngle.x, m_EffectDesc.vStartAngle.y, m_EffectDesc.vStartAngle.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndAngle.x, m_EffectDesc.vEndAngle.y, m_EffectDesc.vEndAngle.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vCurAngle, XMVectorLerp(vStart, vEnd, fLerp));

	m_fBillboard_Angle = m_EffectDesc.vCurAngle.x - m_EffectDesc.vStartAngle.x;


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

void CRect_Effect_P::Distortion_Tick(_double TimeDelta)
{
	if (m_fEffectAcc >= START_DISTIME && m_fEffectAcc <= END_DISTIME)
	{
		DISTORTION_POWER += DISTORTION_SPEED * (_float)TimeDelta;
		if (MAX_DISTORTION_POWER < DISTORTION_POWER)
			DISTORTION_POWER = MAX_DISTORTION_POWER;
	}

	if (m_fEffectAcc > END_DISTIME)
	{
		DISTORTION_POWER -= DISTORTION_SPEED * (_float)TimeDelta;
		if (0.f >= DISTORTION_POWER)
		{
			DISTORTION_POWER = 0.f;
		}
	}
	if (0.f != DISTORTION_POWER)
		m_bDistortion = true;
	else
		m_bDistortion = false;
}

void CRect_Effect_P::Loop_Check(_double TimeDelta)
{
	if (m_fEffectAcc > m_EffectDesc.fEffectTime)
	{
		if (m_EffectDesc.bLoop)
		{
			m_fDelayAcc += (_float)TimeDelta;

			if (m_fDelayAcc > m_EffectDesc.fDelayTime)
			{
				m_fDelayAcc = 0.f;
				m_fEffectAcc = 0.f;
				DISTORTION_POWER = START_DIS_POWER;
				if (nullptr != m_pParentsMatrix)
					memcpy(&m_ParentsMatrix, m_pParentsMatrix, sizeof(_float4x4));
			}
		}
		m_EffectDesc.vUV = { 0.f , 0.f };
		m_fFrameAcc = { 0.f };
	}
}

void CRect_Effect_P::Setup_Matrix()
{
	if (true == BILLBOARD)
	{
		XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&m_ParentsMatrix));
		_float3 Scale;
		_float4 vPos;
		_float3 vNewRight, vNewUp, vNewLook;
		vNewUp = _float3(0.f, 1.f, 0.f);

		vPos = _float4(m_ResultMatirx._41, m_ResultMatirx._42, m_ResultMatirx._43, 1.f);

		_float4 vCameraPos = CPipeLine::GetInstance()->Get_CamPosition();
		XMStoreFloat3(&vNewLook, XMVector3Normalize(XMLoadFloat4(&vCameraPos) - XMLoadFloat4(&vPos)));
		XMStoreFloat3(&vNewRight, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vNewUp), XMLoadFloat3(&vNewLook))));
		XMStoreFloat3(&vNewUp, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vNewLook), XMLoadFloat3(&vNewRight))));

		XMStoreFloat3(&vNewRight, XMLoadFloat3(&vNewRight)*m_EffectDesc.vCurScale.x);
		XMStoreFloat3(&vNewUp, XMLoadFloat3(&vNewUp)*m_EffectDesc.vCurScale.y);
		XMStoreFloat3(&vNewLook, XMLoadFloat3(&vNewLook)*m_EffectDesc.vCurScale.z);

		m_ResultMatirx._11 = vNewRight.x;
		m_ResultMatirx._12 = vNewRight.y;
		m_ResultMatirx._13 = vNewRight.z;

		m_ResultMatirx._21 = vNewUp.x;
		m_ResultMatirx._22 = vNewUp.y;
		m_ResultMatirx._23 = vNewUp.z;

		m_ResultMatirx._31 = vNewLook.x;
		m_ResultMatirx._32 = vNewLook.y;
		m_ResultMatirx._33 = vNewLook.z;

		_matrix RotationMat = XMMatrixRotationAxis(XMLoadFloat3(&vNewLook), XMConvertToRadians(m_fBillboard_Angle));
		XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_ResultMatirx) *RotationMat);
	}
	else
	{
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

		XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&m_ParentsMatrix));

	}
}

CRect_Effect_P * CRect_Effect_P::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pFileTag ,  const EFFECT_DESC & MeshDesc)
{
	CRect_Effect_P*		pInstance = new CRect_Effect_P(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFileTag , MeshDesc)))
	{
		MSG_BOX("Failed to Created : CRect_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRect_Effect_P::Clone(void * pArg)
{
	CGameObject*		pInstance = new CRect_Effect_P(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRect_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRect_Effect_P::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBoxCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pNoiseTextureCom);

	Safe_Release(m_pMainTransform);
}
