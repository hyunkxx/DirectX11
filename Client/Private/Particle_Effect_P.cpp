#include "stdafx.h"
#include "..\Public\Particle_Effect_P.h"

#include "Gameinstance.h"
#include "Model.h"

CParticle_Effect_P::CParticle_Effect_P(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{

}

CParticle_Effect_P::CParticle_Effect_P(const CParticle_Effect_P & rhs)
	: CEffect(rhs)
{

}

HRESULT CParticle_Effect_P::Initialize_Prototype(const char* pFileTag , const EFFECT_DESC & ParticleDesc)
{
	memcpy(&m_EffectDesc, &ParticleDesc, sizeof(EFFECT_DESC));

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Component(pFileTag)))
		return E_FAIL;

	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	_matrix IdenMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ParentsMatrix, IdenMatrix);

	return S_OK;
}

HRESULT CParticle_Effect_P::Initialize(void * pArg)
{
	return S_OK;
}

void CParticle_Effect_P::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fLifeAcc += (_float)TimeDelta;

	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
		return;

	m_fEffectAcc += (_float)TimeDelta;
	m_pVIBufferCom->Update(TimeDelta);

	if (m_EffectDesc.bSprite)
	{
		m_fFrameAcc += m_EffectDesc.fFrameSpeed * (_float)TimeDelta;
		_float FrameTime = (1.f / (m_EffectDesc.vUV.x * m_EffectDesc.vUV.y));

		if (FrameTime < m_fFrameAcc)
		{
			m_fFrameAcc = 0.f;
			m_EffectDesc.vFrame.x += 1.f;

			if (m_EffectDesc.vUV.x - 1.f < m_EffectDesc.vFrame.x)
			{
				m_EffectDesc.vFrame.x = 0.f;
				m_EffectDesc.vFrame.y += 1.f;

				if (m_EffectDesc.vUV.y - 1.f < m_EffectDesc.vFrame.y)
				{
					m_EffectDesc.vFrame.y = 0.f;
				}
			}
		}
	}

	if (m_fEffectAcc > m_EffectDesc.fEffectTime)
	{
		if (m_EffectDesc.bLoop)
		{
			m_fDelayAcc += (_float)TimeDelta;

			if (m_fDelayAcc > m_EffectDesc.fDelayTime)
			{
				m_pVIBufferCom->Reset_Update(m_EffectDesc.vDir);

				m_EffectDesc.vFrame = { 0.f , 0.f };
				m_fFrameAcc = { 0.f };
				m_fEffectAcc = 0.f;
				m_fDelayAcc = 0.f;
				if (nullptr != m_pParentsMatrix)
					memcpy(&m_ParentsMatrix, m_pParentsMatrix, sizeof(_float4x4));
			}
		}
	}

}

void CParticle_Effect_P::LateTick(_double TimeDelta)
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

	_float3 vPos;
	_float fLerp = m_fEffectAcc / m_EffectDesc.fEffectTime;
	XMStoreFloat3(&vPos, XMVectorLerp(XMLoadFloat3(&m_EffectDesc.vMinPosition), XMLoadFloat3(&m_EffectDesc.vMaxPosition), fLerp));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));

	if (m_EffectDesc.bGlow)
	{
		if (m_pRendererCom != nullptr)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PARTICLE, this);
	}
	else
	{
		if (m_pRendererCom != nullptr)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CParticle_Effect_P::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (!m_EffectDesc.bTexture)
	{
		if (m_EffectDesc.bSpark)
		{
			if (ID_RHOMBUS == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(0);
			else if (ID_RECT == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(1);
			else if (ID_TRIANGLE == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(2);
		}
		else
		{
			if (ID_RHOMBUS == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(3);
			else if (ID_RECT == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(4);
			else if (ID_TRIANGLE == m_EffectDesc.eFigure)
				m_pShaderCom->Begin(5);
		}
	}
	else
	{
		if (!m_EffectDesc.bSprite)
		{
			if (!m_EffectDesc.bSpark)
			{
				if (m_EffectDesc.bSetColor)
				{
					if (PASS_ALPHABLEND == m_EffectDesc.iPass)
						m_pShaderCom->Begin(7);
					else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
						m_pShaderCom->Begin(13);
				}
				else
				{
					if (PASS_ALPHABLEND == m_EffectDesc.iPass)
						m_pShaderCom->Begin(10);
					else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
						m_pShaderCom->Begin(16);
				}
			}
			else
			{
				if (m_EffectDesc.bSetColor)
				{
					if (PASS_ALPHABLEND == m_EffectDesc.iPass)
						m_pShaderCom->Begin(6);
					else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
						m_pShaderCom->Begin(12);
				}
				else
				{
					if (PASS_ALPHABLEND == m_EffectDesc.iPass)
						m_pShaderCom->Begin(9);
					else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
						m_pShaderCom->Begin(15);
				}
			}
		}
		else
		{
			if (m_EffectDesc.bSetColor)
			{
				if (PASS_ALPHABLEND == m_EffectDesc.iPass)
					m_pShaderCom->Begin(8);
				else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
					m_pShaderCom->Begin(14);
			}
			else
			{
				if (PASS_ALPHABLEND == m_EffectDesc.iPass)
					m_pShaderCom->Begin(11);
				else if (PASS_COLOR_T0_ALPHA == m_EffectDesc.iPass)
					m_pShaderCom->Begin(17);
			}

		}
	}

	m_pVIBufferCom->Render();

	return S_OK;
}

void CParticle_Effect_P::RenderGUI()
{
}

void CParticle_Effect_P::Play_Effect(_float4x4 * pWorldMatrix, _bool bTracking)
{
	m_bFinish = false;
	m_pVIBufferCom->Reset_Update(m_EffectDesc.vDir);
	m_EffectDesc.vFrame = { 0.f , 0.f };
	m_fFrameAcc = { 0.f };
	m_fEffectAcc = 0.f;
	m_fDelayAcc = 0.f;
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

EFFECT_DESC CParticle_Effect_P::Get_Effect_Desc()
{

	CVIBuffer_Particle::ParticleDesc PrticleDesc = m_pVIBufferCom->Get_ParticleDesc();

	m_EffectDesc.bCircle = PrticleDesc.bCircle;

	m_EffectDesc.bParticleDelay = PrticleDesc.bDelay;
	m_EffectDesc.fParticleDelayTime = PrticleDesc.fDelayTime;
	m_EffectDesc.iParticleDelay = PrticleDesc.iDelayParticle;

	m_EffectDesc.fMinSpeed = PrticleDesc.fMinSpeed;
	m_EffectDesc.fMaxSpeed = PrticleDesc.fMaxSpeed;

	m_EffectDesc.vMinAngle = PrticleDesc.vMinAngle;
	m_EffectDesc.vMaxAngle = PrticleDesc.vMaxAngle;

	m_EffectDesc.fMinDist = PrticleDesc.fMinDist;
	m_EffectDesc.fMaxDist = PrticleDesc.fMaxDist;

	m_EffectDesc.vMinScale = PrticleDesc.vMinScale;
	m_EffectDesc.vMaxScale = PrticleDesc.vMaxScale;

	m_EffectDesc.vMinPosition = PrticleDesc.vMinPosition;
	m_EffectDesc.vMaxPosition = PrticleDesc.vMaxPosition;

	m_EffectDesc.iNumInstance = PrticleDesc.iNumInstance;

	m_EffectDesc.fGravityPower = PrticleDesc.fGravityPower;

	m_EffectDesc.bGravity = PrticleDesc.bGravuty;
	m_EffectDesc.bParticleLoop = PrticleDesc.bLoop;

	return m_EffectDesc;
}

HRESULT CParticle_Effect_P::Add_Component(const char* pFileTag)
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER, L"com_renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC Transform_Desc;
	ZeroMemory(&Transform_Desc, sizeof Transform_Desc);

	Transform_Desc.fMoveSpeed = 5.f;
	Transform_Desc.fRotationSpeed = XMConvertToRadians(60.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM, L"com_transform", (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, SHADER::POINT_INST, L"com_shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_EffectDesc.bTexture)
	{
		Add_Texture(pFileTag, m_EffectDesc.TexName, &m_pTextureCom);
	}

	CVIBuffer_Particle::ParticleDesc stParticleDesc;
	ZeroMemory(&stParticleDesc, sizeof(CVIBuffer_Particle::ParticleDesc));

	stParticleDesc.bCircle = m_EffectDesc.bCircle;

	stParticleDesc.bDelay = m_EffectDesc.bParticleDelay;
	stParticleDesc.fDelayTime = m_EffectDesc.fParticleDelayTime;
	stParticleDesc.iDelayParticle = m_EffectDesc.iParticleDelay;

	stParticleDesc.fMinSpeed = m_EffectDesc.fMinSpeed;
	stParticleDesc.fMaxSpeed = m_EffectDesc.fMaxSpeed;

	stParticleDesc.vMinAngle = m_EffectDesc.vMinAngle;
	stParticleDesc.vMaxAngle = m_EffectDesc.vMaxAngle;

	stParticleDesc.fMinDist = m_EffectDesc.fMinDist;
	stParticleDesc.fMaxDist = m_EffectDesc.fMaxDist;

	stParticleDesc.vMaxScale = m_EffectDesc.vMinScale;
	stParticleDesc.vMinScale = m_EffectDesc.vMaxScale;

	stParticleDesc.vMinPosition = m_EffectDesc.vMinPosition;
	stParticleDesc.vMaxPosition = m_EffectDesc.vMaxPosition;

	stParticleDesc.iNumInstance = 100;

	stParticleDesc.fGravityPower = m_EffectDesc.fGravityPower;

	stParticleDesc.bGravuty = m_EffectDesc.bGravity;
	stParticleDesc.bLoop = m_EffectDesc.bParticleLoop;

	m_pVIBufferCom = CVIBuffer_Particle::Create(m_pDevice, m_pContext, stParticleDesc);

	return S_OK;
}

HRESULT CParticle_Effect_P::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4 LastMatirx;
	if (m_EffectDesc.bTracking)
	{
		if (nullptr == m_pParentsMatrix)
		{
			XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * XMLoadFloat4x4(&m_ParentsMatrix));
		}
		else
			XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * XMLoadFloat4x4(m_pParentsMatrix));
	}
	else
		XMStoreFloat4x4(&LastMatirx, XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()) * XMLoadFloat4x4(&m_ParentsMatrix));

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &LastMatirx)))
		return E_FAIL;

	//if (FAILED(m_pMainTransform->Setup_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pVIBufferCom->SetUp_Shader_Color(m_pShaderCom, "g_vParticleColor")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_fAlpha", &m_EffectDesc.fCurAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (m_EffectDesc.bTexture)
	{
		if (nullptr == m_pTextureCom)
			return E_FAIL;

		if (FAILED(m_pTextureCom->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetRawValue("g_vUV", &m_EffectDesc.vUV, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetRawValue("g_vFrame", &m_EffectDesc.vFrame, sizeof(_float2))))
			return E_FAIL;
	}

	return S_OK;
}

void CParticle_Effect_P::Add_Texture(const char * pFileTag, const char * TextureName, CTexture ** pTexture)
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
}

CParticle_Effect_P * CParticle_Effect_P::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , const char* pFileTag , const EFFECT_DESC & ParticleDesc)
{
	CParticle_Effect_P*		pInstance = new CParticle_Effect_P(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFileTag , ParticleDesc)))
	{
		MSG_BOX("Failed to Created : CParticle_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Effect_P::Clone(void * pArg)
{
	CGameObject*		pInstance = new CParticle_Effect_P(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Effect_P");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Effect_P::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMainTransform);

}
