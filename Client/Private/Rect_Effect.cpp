#include "stdafx.h"
#include "..\Public\Rect_Effect.h"

#include "Gameinstance.h"

CRect_Effect::CRect_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CRect_Effect::CRect_Effect(const CRect_Effect & rhs)
	: CEffect(rhs)
{

}

HRESULT CRect_Effect::Initialize_Prototype(const EFFECT_DESC & MeshDesc)
{
	memcpy(&m_EffectDesc, &MeshDesc, sizeof(EFFECT_DESC));
	m_EffectDesc.eEffectType = TYPE_RECT;

	for (int j = 0; EFFECT_TEX_END > j; j++)
	{
		for (int i = 0; 600 > i; i++)
		{
			_itoa_s(i, m_Effect_TexName[j][i], 10);
		}
	}

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Component(m_EffectDesc)))
		return E_FAIL;

	_matrix IdenMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ParentsMatrix, IdenMatrix);

	return S_OK;
}

HRESULT CRect_Effect::Initialize(void * pArg)
{
	return S_OK;
}

void CRect_Effect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fLifeAcc += (_float)TimeDelta;

	if (m_fLifeAcc >= m_EffectDesc.fLifeTime)
	{
		m_bFinish = true;
		return;
	}
	else
		m_bFinish = false;

	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
	{
		if (m_pParentsMatrix != nullptr)
			m_ParentsMatrix = *m_pParentsMatrix;
		return;
	}
		

	m_fEffectAcc += (_float)TimeDelta;

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

void CRect_Effect::LateTick(_double TimeDelta)
{
	if (m_bFinish)
		return;


	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
		return;

	if (m_fEffectAcc >= m_EffectDesc.fEffectTime)
		return;

	__super::LateTick(TimeDelta);

	SetUp_Linear();
	XMStoreFloat2(&m_EffectDesc.vUV, XMLoadFloat2(&m_EffectDesc.vUV) + XMLoadFloat2(&m_EffectDesc.fUVSpeed) * (_float)TimeDelta);

	if (true == BILLBOARD)
	{
		m_WorldMatrix = m_pMainTransform->Get_WorldMatrix();

		_float3 vParentsPos;
		if (m_EffectDesc.bTracking  && nullptr != m_pParentsMatrix)
			vParentsPos = _float3(m_pParentsMatrix->_41, m_pParentsMatrix->_42, m_pParentsMatrix->_43);
		else
			vParentsPos = _float3(m_ParentsMatrix._41, m_ParentsMatrix._42, m_ParentsMatrix._43);

		XMStoreFloat4x4(&m_ResultMatirx, XMMatrixIdentity());

		_float3 vPos;
		_float3 vNewRight, vNewUp, vNewLook;
		_float3 vDefaultUp = _float3(0.f, 1.f, 0.f);

		vPos = _float3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);

		_float4 vCameraPos = CPipeLine::GetInstance()->Get_CamPosition();
		XMStoreFloat3(&vNewLook, XMVector3Normalize(XMLoadFloat3(&vParentsPos) - XMLoadFloat4(&vCameraPos)));
		XMStoreFloat3(&vNewRight, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vDefaultUp), XMLoadFloat3(&vNewLook))));
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

		XMVector3TransformCoord(XMLoadFloat3(&vPos), XMLoadFloat4x4(&m_ResultMatirx));

		m_ResultMatirx._41 = vPos.x;
		m_ResultMatirx._42 = vPos.y;
		m_ResultMatirx._43 = vPos.z;

		_matrix RotationMat = XMMatrixRotationAxis(XMLoadFloat3(&vNewLook), XMConvertToRadians(m_fBillboard_Angle));
		XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_ResultMatirx) *RotationMat);

		m_ResultMatirx._41 += vParentsPos.x;
		m_ResultMatirx._42 += vParentsPos.y;
		m_ResultMatirx._43 += vParentsPos.z;
		vParentsPos = _float3(m_ResultMatirx._41, m_ResultMatirx._42, m_ResultMatirx._43);

		XMStoreFloat3(&vNewLook, XMVector3Normalize(XMLoadFloat3(&vParentsPos) - XMLoadFloat4(&vCameraPos)));
		XMStoreFloat3(&vNewRight, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vDefaultUp), XMLoadFloat3(&vNewLook))));
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

		if (m_EffectDesc.bTracking && nullptr != m_pParentsMatrix)
			XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(m_pParentsMatrix));
		else
			XMStoreFloat4x4(&m_ResultMatirx, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&m_ParentsMatrix));

	}

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup((CRenderer::RENDER_GROUP)m_EffectDesc.iRenderGroup, this);

}

HRESULT CRect_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_EffectDesc.iPass);

	if(CRenderer::RENDER_SSD ==  m_EffectDesc.iRenderGroup || CRenderer::RENDER_GLOWSSD == m_EffectDesc.iRenderGroup)
		m_pVIBoxCom->Render();
	else
		m_pVIBufferCom->Render();

	return S_OK;
}

void CRect_Effect::RenderGUI()
{
	if (!m_bEDit)
		return;

	ImGui::Begin("Effect_Rect");

	ImGui::Separator();
	ImGui::Text("**Base_Prop**");
	ImGui::InputFloat("LifeTime", (float*)&m_EffectDesc.fLifeTime);
	ImGui::InputFloat("EffectTime", (float*)&m_EffectDesc.fEffectTime);
	ImGui::InputFloat("StartDelay", (float*)&m_EffectDesc.fStartDelay);
	ImGui::Checkbox("Loop", &m_EffectDesc.bLoop);

	ImGui::SameLine(100);
	ImGui::Checkbox("Tracking", &m_EffectDesc.bTracking);

	if (m_EffectDesc.bLoop)
	{
		ImGui::InputFloat("DelayTime", (float*)&m_EffectDesc.fDelayTime);
	}

	ImGui::Separator();
	ImGui::Text("**Reset**");

	if (ImGui::Button("Time"))
	{
		m_fEffectAcc = 0.f;
		m_fLifeAcc = 0.f;
		DISTORTION_POWER = START_DIS_POWER;
	}
	ImGui::Separator();


#pragma region Texture
	ImGui::Text("**Texture_Prop**");

#pragma region Group

	if (1 > m_EffectDesc.iRenderGroup)
		m_EffectDesc.iRenderGroup = 8;
	else if (8 < m_EffectDesc.iRenderGroup)
		m_EffectDesc.iRenderGroup = 1;
	if (1 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: DYNAMIC");
	else if (2 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: NONLIGHT");
	else if (3 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: PARTICLE");
	else if (4 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: ALPHABLEND");
	else if (5 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: DISTORTION");
	else if (6 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: SSD");
	else if (7 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: STATIC");
	else if (8 == m_EffectDesc.iRenderGroup)
		ImGui::Text("Render_Group: Glow_SSD");

	ImGui::InputInt("RederGroup", &m_EffectDesc.iRenderGroup);

	if (0 > m_EffectDesc.iPass)
		m_EffectDesc.iPass = 13;
	else if (13 < m_EffectDesc.iPass)
		m_EffectDesc.iPass = 0;
	if (0 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Default");
	else if (1 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Default_NORMAL");
	else if (2 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Particle_Color");
	else if (3 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Particle_Color_Clamp");
	else if (4 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Particle_Alpha");
	else if (5 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Particle_Alpha_Clamp");
	else if (6 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Distortion");
	else if (7 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Distortion_No_Depth");
	else if (8 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Decal");
	else if (9 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Decal_Normal");
	else if (10 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Decal_Color_Alpha");
	else if (11 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Decal_Color");
	else if (12 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Default_No_Depth");
	else if (13 == m_EffectDesc.iPass)
		ImGui::Text("Pass: Particle_Color_No_Depth");

	ImGui::InputInt("Pass", &m_EffectDesc.iPass);

#pragma endregion

	ImGui::Checkbox("Diffuse", &m_EffectDesc.bTexture);
	ImGui::SameLine(85);
	ImGui::Checkbox("Noise", &m_EffectDesc.bNoiseTexure);
	ImGui::SameLine(155);
	ImGui::Checkbox("Dissolve", &m_EffectDesc.bDissolveTexure);
	if (m_EffectDesc.bTexture)
	{
#pragma region Diffuse_List

		if (ImGui::BeginCombo("Diffuse_List", m_EffectDesc.TexName))
		{
			for (int n = 0; n < IM_ARRAYSIZE(m_FileName[BUFFER_TEX]); n++)
			{
				bool is_selected = (m_EffectDesc.TexName == m_FileName[BUFFER_TEX][n]);
				if (ImGui::Selectable(m_FileName[BUFFER_TEX][n], is_selected))
					strcpy_s(m_EffectDesc.TexName, m_FileName[BUFFER_TEX][n]);
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (m_EffectDesc.TexName != NULL)
		{
			if (strcmp(m_EffectDesc.TexName, m_PrevSelect_Diffuse_TexName))
			{
				strcpy_s(m_PrevSelect_Diffuse_TexName, m_EffectDesc.TexName);
				Set_Image(m_EffectDesc.TexName);
			}
		}
		// 여기서 스프라이트 기능 만들어주기
#pragma endregion
	}
	if (m_EffectDesc.bNoiseTexure)
	{
#pragma region Noise_List

		if (ImGui::BeginCombo("Noise_List", m_EffectDesc.NoiseTexName))
		{
			for (int n = 0; n < IM_ARRAYSIZE(m_Effect_TexName[TEX_NOISE]); n++)
			{
				bool is_selected = (m_EffectDesc.NoiseTexName == m_Effect_TexName[TEX_NOISE][n]);
				if (ImGui::Selectable(m_Effect_TexName[TEX_NOISE][n], is_selected))
					strcpy_s(m_EffectDesc.NoiseTexName, m_Effect_TexName[TEX_NOISE][n]);
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (m_EffectDesc.NoiseTexName != NULL)
		{
			if (strcmp(m_PrevSelect_Noise_TexName, m_EffectDesc.NoiseTexName))
			{
				strcpy_s(m_PrevSelect_Noise_TexName, m_EffectDesc.NoiseTexName);
				Set_NoiseImage(m_EffectDesc.NoiseTexName);
			}
		}
#pragma endregion
	}
	if (m_EffectDesc.bDissolveTexure)
	{
#pragma region Dissolve_List

		if (ImGui::BeginCombo("Dissolve_List", m_EffectDesc.DissolveTexName))
		{
			for (int n = 0; n < IM_ARRAYSIZE(m_Effect_TexName[TEX_DISSOLVE]); n++)
			{
				bool is_selected = (m_EffectDesc.DissolveTexName == m_Effect_TexName[TEX_DISSOLVE][n]);
				if (ImGui::Selectable(m_Effect_TexName[TEX_DISSOLVE][n], is_selected))
					strcpy_s(m_EffectDesc.DissolveTexName, m_Effect_TexName[TEX_DISSOLVE][n]);
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (m_EffectDesc.DissolveTexName != NULL)
		{
			if (strcmp(m_PrevSelect_Dissolve_TexName, m_EffectDesc.DissolveTexName))
			{
				strcpy_s(m_PrevSelect_Dissolve_TexName, m_EffectDesc.DissolveTexName);
				Set_DissolveImage(m_EffectDesc.DissolveTexName);
			}
		}
#pragma endregion
	}
#pragma endregion

#pragma region Motion
	_int iID = 0;
	ImGui::Separator();
	ImGui::Text("**Motion_Prop**");

	ImGui::Separator();
	ImGui::Text("Color");
	ImGui::SameLine(100);
	ImGui::PushID(iID++);
	ImGui::Checkbox("Lock", &m_bColorLock);
	ImGui::PopID();
	ImGui::ColorEdit3("StartColor", (float*)&m_EffectDesc.vStartColor);
	ImGui::ColorEdit3("EndColor", (float*)&m_EffectDesc.vEndColor);
	if (m_bColorLock)
	{
		m_EffectDesc.vEndColor = m_EffectDesc.vStartColor;
	}

	ImGui::Separator();
	ImGui::Text("Position");
	ImGui::SameLine(100);
	ImGui::PushID(iID++);
	ImGui::Checkbox("Lock", &m_bPositionLock);
	ImGui::SameLine(200);
	ImGui::Checkbox("Billboard", &BILLBOARD);
	ImGui::PopID();
	ImGui::InputFloat3("StartPosition", (float*)&m_EffectDesc.vStartPosition);
	ImGui::InputFloat3("EndPosition", (float*)&m_EffectDesc.vEndPosition);
	if (m_bPositionLock)
	{
		m_EffectDesc.vEndPosition = m_EffectDesc.vStartPosition;
	}

	ImGui::Separator();
	ImGui::Text("Angle");
	ImGui::Checkbox("Slider_On", &m_bAngleSlider);
	ImGui::SameLine(100);
	ImGui::PushID(iID++);
	ImGui::Checkbox("Lock", &m_bAngleLock);
	ImGui::PopID();

	if (BILLBOARD)
	{
		if (m_bAngleSlider)
		{
			ImGui::SliderFloat("StartAngle", (float*)&m_EffectDesc.vStartAngle, -180.f, 180.f);
			ImGui::SliderFloat("EndAngle", (float*)&m_EffectDesc.vEndAngle, -180.f, 180.f);
		}
		else
		{
			ImGui::InputFloat("StartAngle", (float*)&m_EffectDesc.vStartAngle);
			ImGui::InputFloat("EndAngle", (float*)&m_EffectDesc.vEndAngle);
		}
	}
	else
	{
		if (m_bAngleSlider)
		{
			ImGui::SliderFloat3("StartAngle", (float*)&m_EffectDesc.vStartAngle, -180.f, 180.f);
			ImGui::SliderFloat3("EndAngle", (float*)&m_EffectDesc.vEndAngle, -180.f, 180.f);
		}
		else
		{
			ImGui::InputFloat3("StartAngle", (float*)&m_EffectDesc.vStartAngle);
			ImGui::InputFloat3("EndAngle", (float*)&m_EffectDesc.vEndAngle);
		}
	}
	if (m_bAngleLock)
	{
		m_EffectDesc.vEndAngle = m_EffectDesc.vStartAngle;
	}

	ImGui::Separator();
	ImGui::Text("Scale");
	ImGui::SameLine(100);
	ImGui::PushID(iID++);
	ImGui::Checkbox("Lock", &m_bScaleLock);
	ImGui::PopID();
	ImGui::InputFloat3("StartScale", (float*)&m_EffectDesc.vStartScale);
	ImGui::InputFloat3("EndScale", (float*)&m_EffectDesc.vEndScale);
	if (m_bScaleLock)
	{
		m_EffectDesc.vEndScale = m_EffectDesc.vStartScale;
	}

	ImGui::Separator();
	ImGui::Text("Alpha");

	ImGui::InputFloat("StartAlpha", (float*)&m_EffectDesc.fStartAlpha);
	ImGui::InputFloat("EndAlpha", (float*)&m_EffectDesc.fEndAlpha);

	ImGui::Separator();
	ImGui::Text("Start_UV");
	ImGui::InputFloat2("StartUV", (float*)&m_EffectDesc.vMinScale);
	ImGui::Text("UVSpeed");
	ImGui::InputFloat2("UVSpeed", (float*)&m_EffectDesc.fUVSpeed);

	ImGui::Separator();
	ImGui::Text("Distortion");
	ImGui::InputFloat2("DistortionTime", (float*)&m_EffectDesc.vMaxScale);
	ImGui::InputFloat("Start_Power", (float*)&m_EffectDesc.vMinPosition.z);
	ImGui::InputFloat("Power", (float*)&m_EffectDesc.vMinPosition.x);
	ImGui::InputFloat("Speed", (float*)&m_EffectDesc.vMinPosition.y);

#pragma endregion

	ImGui::End();
}

void CRect_Effect::Play_Effect(_float4x4 * pWorldMatrix, _bool bTracking)
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

EFFECT_DESC CRect_Effect::Get_Effect_Desc()
{
	return m_EffectDesc;
}

HRESULT CRect_Effect::Add_Component(const EFFECT_DESC & ParticleDesc)
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
	//VIBUIFFER_RECT_NOR VIBUIFFER_SSD_BOX

#ifdef _DEBUG

#pragma region Texture

	if (FAILED(Add_Texture(L"../../Resource/Texture/Flare/")))
		return E_FAIL;

	if (FAILED(Add_Texture(L"../../Resource/Texture/Spark/")))
		return E_FAIL;

	if (FAILED(Add_Texture(L"../../Resource/Texture/Ring/")))
		return E_FAIL;

	if (FAILED(Add_Texture(L"../../Resource/Texture/Decal/")))
		return E_FAIL;

	if (FAILED(Add_Texture(L"../../Resource/Texture/Sprite/")))
		return E_FAIL;

	if (FAILED(Add_Texture(L"../../Resource/Texture/Wenli/")))
		return E_FAIL;

#pragma endregion

	if (FAILED(Add_DissolveTex(L"../../Resource/Texture/Dissolve/")))
		return E_FAIL;

	if (FAILED(Add_NoiseTex(L"../../Resource/Texture/Noise/")))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

HRESULT CRect_Effect::SetUp_ShaderResources()
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

HRESULT CRect_Effect::Add_Texture(const char * pKeyTag, const char * pCharTag)
{
	char pTag1[MAX_PATH] = "";
	strcpy_s(pTag1, pCharTag);
	_tchar pTag2[MAX_PATH] = L"";

	MultiByteToWideChar(CP_ACP, 0, pTag1, (_int)strlen(pTag1),
		pTag2, MAX_PATH);

	CTexture* pTextureCom = nullptr;
	pTextureCom = CTexture::Create(m_pDevice, m_pContext, pTag2);

	if (nullptr == pTextureCom)
		return E_FAIL;

	char* pChar = new char[MAX_PATH];
	strcpy_s(pChar, sizeof(char)*MAX_PATH, pKeyTag);

	strcpy_s(m_FileName[BUFFER_TEX][m_iFileCount[BUFFER_TEX]++], pChar);
	m_Textures.emplace(pChar, pTextureCom);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRect_Effect::Add_Texture(const _tchar * pPathName)
{
	wstring path = pPathName;
	_tchar ParentFolderName[MAX_PATH] = TEXT("");

	for (const auto & file : recursive_directory_iterator(path))
	{
		if (file.path().extension().string() != ".dds")
			continue;

		lstrcpy(ParentFolderName, file.path().parent_path().filename().c_str());

		_tchar RouteName[MAX_PATH] = TEXT("");
		_tchar FileName[MAX_PATH] = TEXT("");
		_tchar EndName[MAX_PATH] = TEXT(".dds");
		_wsplitpath_s(file.path().c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);

		list<EFFECT_DESC*> pEffectDesces;
		lstrcpy(RouteName, pPathName);
		lstrcat(RouteName, FileName);
		lstrcat(RouteName, EndName);

		CTexture* pTextureCom = nullptr;
		pTextureCom = CTexture::Create(m_pDevice, m_pContext, RouteName);

		char* pChar = new char[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, FileName, -1, pChar, MAX_PATH, NULL, NULL);

		if (nullptr == pTextureCom)
			return E_FAIL;

		strcpy_s(m_FileName[BUFFER_TEX][m_iFileCount[BUFFER_TEX]++], pChar);
		m_Textures.emplace(pChar, pTextureCom);
	}

	return S_OK;
}


HRESULT CRect_Effect::Add_NoiseTex(const _tchar * pPathName)
{
	wstring path = pPathName;
	_tchar ParentFolderName[MAX_PATH] = TEXT("");

	for (const auto & file : recursive_directory_iterator(path))
	{
		if (file.path().extension().string() != ".dds")
			continue;

		lstrcpy(ParentFolderName, file.path().parent_path().filename().c_str());

		_tchar RouteName[MAX_PATH] = TEXT("");
		_tchar FileName[MAX_PATH] = TEXT("");
		_tchar EndName[MAX_PATH] = TEXT(".dds");
		_wsplitpath_s(file.path().c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);

		list<EFFECT_DESC*> pEffectDesces;
		lstrcpy(RouteName, pPathName);
		lstrcat(RouteName, FileName);
		lstrcat(RouteName, EndName);

		CTexture* pTextureCom = nullptr;
		pTextureCom = CTexture::Create(m_pDevice, m_pContext, RouteName);

		char* pChar = new char[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, FileName, -1, pChar, MAX_PATH, NULL, NULL);

		if (nullptr == pTextureCom)
			return E_FAIL;

		strcpy_s(m_Effect_TexName[TEX_NOISE][m_iEffect_TexCount[TEX_NOISE]++], pChar);
		m_NoiseTex.emplace(pChar, pTextureCom);
	}

	return S_OK;
}

HRESULT CRect_Effect::Add_DissolveTex(const _tchar * pPathName)
{
	wstring path = pPathName;
	_tchar ParentFolderName[MAX_PATH] = TEXT("");

	for (const auto & file : recursive_directory_iterator(path))
	{
		if (file.path().extension().string() != ".dds")
			continue;

		lstrcpy(ParentFolderName, file.path().parent_path().filename().c_str());

		_tchar RouteName[MAX_PATH] = TEXT("");
		_tchar FileName[MAX_PATH] = TEXT("");
		_tchar EndName[MAX_PATH] = TEXT(".dds");
		_wsplitpath_s(file.path().c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);

		list<EFFECT_DESC*> pEffectDesces;
		lstrcpy(RouteName, pPathName);
		lstrcat(RouteName, FileName);
		lstrcat(RouteName, EndName);

		CTexture* pTextureCom = nullptr;
		pTextureCom = CTexture::Create(m_pDevice, m_pContext, RouteName);

		char* pChar = new char[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, FileName, -1, pChar, MAX_PATH, NULL, NULL);

		if (nullptr == pTextureCom)
			return E_FAIL;

		strcpy_s(m_Effect_TexName[TEX_DISSOLVE][m_iEffect_TexCount[TEX_DISSOLVE]++], pChar);
		m_DissolveTex.emplace(pChar, pTextureCom);
	}
	return S_OK;
}


#endif // _DEBUG

HRESULT CRect_Effect::Add_NoiseTex(const char * pKeyTag, const char * pCharTag)
{
	char pTag1[MAX_PATH] = "";
	strcpy_s(pTag1, pCharTag);
	_tchar pTag2[MAX_PATH] = L"";

	MultiByteToWideChar(CP_ACP, 0, pTag1, (_int)strlen(pTag1),
		pTag2, MAX_PATH);

	CTexture* pTextureCom = nullptr;
	pTextureCom = CTexture::Create(m_pDevice, m_pContext, pTag2);

	if (nullptr == pTextureCom)
		return E_FAIL;

	char* pChar = new char[MAX_PATH];
	strcpy_s(pChar, sizeof(char)*MAX_PATH, pKeyTag);

	strcpy_s(m_Effect_TexName[TEX_NOISE][m_iEffect_TexCount[TEX_NOISE]++], pChar);
	m_NoiseTex.emplace(pChar, pTextureCom);

	return S_OK;
}

HRESULT CRect_Effect::Add_DissolveTex(const char * pKeyTag, const char * pCharTag)
{
	char pTag1[MAX_PATH] = "";
	strcpy_s(pTag1, pCharTag);
	_tchar pTag2[MAX_PATH] = L"";

	MultiByteToWideChar(CP_ACP, 0, pTag1, (_int)strlen(pTag1),
		pTag2, MAX_PATH);

	CTexture* pTextureCom = nullptr;
	pTextureCom = CTexture::Create(m_pDevice, m_pContext, pTag2);

	if (nullptr == pTextureCom)
		return E_FAIL;

	char* pChar = new char[MAX_PATH];
	strcpy_s(pChar, sizeof(char)*MAX_PATH, pKeyTag);

	strcpy_s(m_Effect_TexName[TEX_DISSOLVE][m_iEffect_TexCount[TEX_DISSOLVE]++], pChar);
	m_DissolveTex.emplace(pChar, pTextureCom);

	return S_OK;
}

void CRect_Effect::Set_Image(const char * pImageTag)
{
	for (auto& iter : m_Textures)
	{
		char			szFileName[MAX_PATH] = "";
		_splitpath_s(iter.first, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		if (!strcmp(pImageTag, szFileName))
		{
			m_pTextureCom = iter.second;
		}
	}
}

void CRect_Effect::Set_DissolveImage(const char * pImageTag)
{
	for (auto& iter : m_DissolveTex)
	{
		char			szFileName[MAX_PATH] = "";
		_splitpath_s(iter.first, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		if (!strcmp(pImageTag, szFileName))
		{
			m_pDissolveTextureCom = iter.second;
		}
	}
}

void CRect_Effect::Set_NoiseImage(const char * pImageTag)
{
	for (auto& iter : m_NoiseTex)
	{
		char			szFileName[MAX_PATH] = "";
		_splitpath_s(iter.first, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

		if (!strcmp(pImageTag, szFileName))
		{
			m_pNoiseTextureCom = iter.second;
		}
	}
}

void CRect_Effect::SetUp_Linear()
{
	_float fLerp = (m_fEffectAcc) / (m_EffectDesc.fEffectTime);
	if (1.f < fLerp)
		fLerp = 1.f;
	if (0.f > fLerp)
		fLerp = 0.f;

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

	// 러프 역수로 바꾸고 알파값 다살린후 컬러만 역으로 그후 디스카드하기
}

CRect_Effect * CRect_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const EFFECT_DESC & MeshDesc)
{
	CRect_Effect*		pInstance = new CRect_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(MeshDesc)))
	{
		MSG_BOX("Failed to Created : CRect_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRect_Effect::Clone(void * pArg)
{
	CGameObject*		pInstance = new CRect_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRect_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRect_Effect::Free()
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

	for (auto& iter : m_Textures)
	{
		char* pChar = iter.first;

		Safe_Release(iter.second);
		Safe_Delete_Array(pChar);
	}
	m_Textures.clear();

	for (auto& iter : m_NoiseTex)
	{
		char* pChar = iter.first;

		Safe_Release(iter.second);
		Safe_Delete_Array(pChar);
	}
	m_NoiseTex.clear();

	for (auto& iter : m_DissolveTex)
	{
		char* pChar = iter.first;

		Safe_Release(iter.second);
		Safe_Delete_Array(pChar);
	}
	m_DissolveTex.clear();


}
