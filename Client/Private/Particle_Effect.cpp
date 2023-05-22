#include "stdafx.h"
#include "..\Public\Particle_Effect.h"

#include "Gameinstance.h"

CParticle_Effect::CParticle_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{

}

CParticle_Effect::CParticle_Effect(const CParticle_Effect & rhs)
	:CEffect(rhs)
{

}

HRESULT CParticle_Effect::Initialize_Prototype(const EFFECT_DESC & ParticleDesc)
{
	memcpy(&m_EffectDesc , &ParticleDesc , sizeof(EFFECT_DESC));
	m_EffectDesc.eEffectType = ID_PARTICLE;

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Add_Component(ParticleDesc)))
		return E_FAIL;

	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	_matrix IdenMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&m_ParentsMatrix, IdenMatrix);
	
	return S_OK;
}

HRESULT CParticle_Effect::Initialize(void * pArg)
{
	return S_OK;
}

void CParticle_Effect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fLifeAcc += (_float)TimeDelta;

	if (m_EffectDesc.fStartDelay >= m_fLifeAcc)
	{
		if(m_pParentsMatrix != nullptr)
			m_ParentsMatrix = *m_pParentsMatrix;
		return;
	}

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

void CParticle_Effect::LateTick(_double TimeDelta)
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

	_vector vStart, vEnd;
	vStart = XMVectorSet(m_EffectDesc.vStartScale.x, m_EffectDesc.vStartScale.y, m_EffectDesc.vStartScale.z, 0.f);
	vEnd = XMVectorSet(m_EffectDesc.vEndScale.x, m_EffectDesc.vEndScale.y, m_EffectDesc.vEndScale.z, 0.f);
	XMStoreFloat3(&m_EffectDesc.vCurScale, XMVectorLerp(vStart, vEnd, fLerp));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_EffectDesc.vCurPosition));
	m_pMainTransform->SetRotationXYZ(m_EffectDesc.vCurAngle);

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

HRESULT CParticle_Effect::Render()
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

void CParticle_Effect::RenderGUI()
{
	if (!m_bEDit)
		return;
	ImGui::Begin("Effect_Particle");

	ImGui::InputInt("NumInstance", (int*)&m_EffectDesc.iNumInstance);
	if (ImGui::Button("Set_NumInstance"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_NUMINSTANCE, &m_EffectDesc.iNumInstance);
	}
	ImGui::Separator();
	ImGui::Text("**Base_Prop**");
	ImGui::InputFloat("LifeTime", (float*)&m_EffectDesc.fLifeTime);
	ImGui::InputFloat("EffectTime", (float*)&m_EffectDesc.fEffectTime);
	ImGui::InputFloat("StartDelay", (float*)&m_EffectDesc.fStartDelay);
	ImGui::Checkbox("Loop", &m_EffectDesc.bLoop);
	if (m_EffectDesc.bLoop)
	{
		ImGui::InputFloat("DelayTime", (float*)&m_EffectDesc.fDelayTime);
	}
	ImGui::Separator();
	ImGui::Text("**Reset**");
	if (ImGui::Button("Option"))
	{
#pragma region Reset

		m_EffectDesc.bCircle = { false };

		m_EffectDesc.bDelay = { false };
		m_EffectDesc.fDelayTime = { 1.f };

		m_EffectDesc.bParticleDelay = { false };
		m_EffectDesc.fParticleDelayTime = { 1.f };
		m_EffectDesc.iParticleDelay= { 1 };

		m_EffectDesc.fMinDist = { 0.f };
		m_EffectDesc.fMaxDist = { 1.f };
		m_EffectDesc.fMinSpeed = { 0.f };
		m_EffectDesc.fMaxSpeed = { 1.f };

		m_EffectDesc.vDir = { 0.f , 0.f , 1.f };
		m_EffectDesc.vDir_Angle = { 0.f , 0.f };

		m_EffectDesc.vMinPosition = { 0.f , 0.f , 0.f };
		m_EffectDesc.vMaxPosition = { 0.f , 0.f , 0.f };
		m_EffectDesc.vMinAngle = { 0.f , 0.f , 0.f };
		m_EffectDesc.vMaxAngle = { 0.f , 0.f , 0.f };
		m_EffectDesc.vMaxScale = { 0.04f ,0.04f ,0.04f };
		m_EffectDesc.vMinScale = { 0.04f ,0.04f ,0.04f };
		m_EffectDesc.vStartColor = { 1.f , 1.f , 1.f };
		m_EffectDesc.vEndColor = { 1.f , 1.f , 1.f };

		m_EffectDesc.vUV = { 1.f , 1.f };
		m_EffectDesc.fFrameSpeed = { 1.f };
		m_EffectDesc.vFrame = { 0.f , 0.f };

		m_EffectDesc.bTexture = { false };
		m_EffectDesc.bSprite = { false };
		m_EffectDesc.bSetColor = { false };

		m_EffectDesc.bSpark = { true };
		m_EffectDesc.bRhombus = { false };
		m_EffectDesc.bRect = { false };
		m_EffectDesc.bTriangle = { false };

		m_EffectDesc.bGravity = { false };
		m_bPrevLoop = { false };
		m_EffectDesc.bLoop = { false };

		m_EffectDesc.fGravityPower = { 0.f };

		m_EffectDesc.fEffectTime = { 1.f };
		m_EffectDesc.bGlow = false;
		m_EffectDesc.iPass = PASS_ALPHABLEND;

#pragma endregion 
	}
	ImGui::SameLine(75);
	if (ImGui::Button("Time"))
	{
		static_cast<CVIBuffer_Particle*>(m_pVIBufferCom)->Reset_Update(m_EffectDesc.vDir);
		m_fFrameAcc = 0.f;
		m_fEffectAcc = 0.f;
		m_fLifeAcc = 0.f;
	}

	ImGui::Separator();
#pragma region Texture
	ImGui::Text("**Texture**");
	ImGui::Checkbox("Texture", &m_EffectDesc.bTexture);
	ImGui::SameLine(100);
	ImGui::Checkbox("Glow", &m_EffectDesc.bGlow);
	ImGui::SameLine(160);
	ImGui::Checkbox("Tracking", &m_EffectDesc.bTracking);
	
	if (m_EffectDesc.bTexture)
	{
		ImGui::Separator();
		ImGui::Text("**Texture_Option**");

		ImGui::Checkbox("Sprite", &m_EffectDesc.bSprite);
		ImGui::SameLine(100);
		ImGui::Checkbox("SetColor", &m_EffectDesc.bSetColor);
		ImGui::InputInt("RenderGroup", (_int*)&m_EffectDesc.iPass);
		ImGui::InputFloat2("UV", (float*)&m_EffectDesc.vUV);
		ImGui::InputFloat("FrameSpeed", (float*)&m_EffectDesc.fFrameSpeed);

		if (PASS_END <= m_EffectDesc.iPass || 0 > m_EffectDesc.iPass)
		{
			m_EffectDesc.iPass = PASS_ALPHABLEND;
		}

		if (ImGui::BeginCombo("TextureList", m_EffectDesc.TexName))
		{
			for (int n = 0; n < IM_ARRAYSIZE(m_FileName[BUFFER_TEX]); n++)
			{
				bool is_selected = (m_EffectDesc.TexName == m_FileName[BUFFER_TEX][n]);
				if (ImGui::Selectable(m_FileName[BUFFER_TEX][n], is_selected))
					strcpy_s(m_EffectDesc.TexName , m_FileName[BUFFER_TEX][n]);
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (m_EffectDesc.TexName != NULL)
		{
			if (strcmp(m_PrevSelect_Diffuse_TexName, m_EffectDesc.TexName))
			{
				strcpy_s(m_PrevSelect_Diffuse_TexName, m_EffectDesc.TexName);
				Set_Image(m_EffectDesc.TexName);
			}
		}
	}
	else
	{
		m_EffectDesc.bSprite = false;
	}

#pragma endregion
	ImGui::Separator();
#pragma region Particle_Delay
	ImGui::Text("**Particle_Delay**");
	ImGui::Checkbox("Particle_Delay", &m_EffectDesc.bParticleDelay);
	m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_DELAY, &m_EffectDesc.bParticleDelay);

	if (m_EffectDesc.bParticleDelay)
	{
		ImGui::InputFloat("Particle_DelayTime", (float*)&m_EffectDesc.fParticleDelayTime);
		ImGui::InputInt("DelayParticle", (int*)&m_EffectDesc.iParticleDelay);
		if (ImGui::Button("Set_Particle_Delay"))
		{
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_DELAY_TIME, &m_EffectDesc.fParticleDelayTime);
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_DELAY_PARTICLE, &m_EffectDesc.iParticleDelay);
		}
	}
#pragma endregion
	ImGui::Separator();
#pragma region Dir
	ImGui::Text("**Dir**");
	_float3 vDir = _float3(0.f, 0.f, 1.f);
	ImGui::SliderAngle("Dir_AngleX", (float*)&m_EffectDesc.vDir_Angle.x);
	ImGui::SliderAngle("Dir_AngleY", (float*)&m_EffectDesc.vDir_Angle.y);
	_matrix DirMatrix = XMMatrixIdentity();
	DirMatrix *= XMMatrixRotationX(m_EffectDesc.vDir_Angle.x);
	DirMatrix *= XMMatrixRotationY(m_EffectDesc.vDir_Angle.y);
	XMStoreFloat3(&m_EffectDesc.vDir, XMVector3TransformNormal(XMLoadFloat3(&vDir), DirMatrix));
	
	ImGui::InputFloat3("Cumulative_Rotation", (float*)&CUMULATIVE_ROTATION);
	if(ImGui::Button("Cmul_Roat_Set"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_ROTATION, &CUMULATIVE_ROTATION);
	}

#pragma endregion
	ImGui::Separator();
#pragma region Dist
	ImGui::Text("**Dist**");
	ImGui::InputFloat("MinDist", (float*)&m_EffectDesc.fMinDist);
	ImGui::InputFloat("MaxDist", (float*)&m_EffectDesc.fMaxDist);

	if (ImGui::Button("Set_Dist"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MINDIST, &m_EffectDesc.fMinDist);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAXDIST, &m_EffectDesc.fMaxDist);
	}

#pragma endregion
	ImGui::Separator();
#pragma region Position
	ImGui::Text("**Position**");
	ImGui::Checkbox("Circle", &m_EffectDesc.bCircle);

	ImGui::InputFloat3("Parents_Position", (float*)&m_EffectDesc.vCurPosition);
	ImGui::InputFloat3("Parents_Angle", (float*)&m_EffectDesc.vCurAngle);
	
	if (m_EffectDesc.bCircle)
	{
		ImGui::InputFloat("Max_Start_Distance", (float*)&MAX_STARTDISTANCE);
		ImGui::InputFloat("Min_Start_Distance", (float*)&MIN_DISTANCE);
	}
	else
	{
		ImGui::InputFloat3("MinPosition", (float*)&m_EffectDesc.vMinPosition);
		ImGui::InputFloat3("MaxPosition", (float*)&m_EffectDesc.vMaxPosition);
	}

	if (ImGui::Button("Set_Position"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_CIRCLE, &m_EffectDesc.bCircle);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MINPOSITION, &m_EffectDesc.vMinPosition);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAXPOSITION, &m_EffectDesc.vMaxPosition);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAX_STARTDIST, &MAX_STARTDISTANCE);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MIN_STARTDIST, &MIN_DISTANCE);
	}

#pragma endregion
	ImGui::Separator();
#pragma region Angle
	ImGui::Text("**Angle**");
	ImGui::InputFloat3("MinAngle", (float*)&m_EffectDesc.vMinAngle);
	ImGui::InputFloat3("MaxAngle", (float*)&m_EffectDesc.vMaxAngle);
	if (ImGui::Button("Set_Angle"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MINANGLE, &m_EffectDesc.vMinAngle);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAXANGLE, &m_EffectDesc.vMaxAngle);
	}
#pragma endregion
	ImGui::Separator();
#pragma region Speed
	ImGui::Text("**Speed**");
	ImGui::InputFloat("MinSpeed", (float*)&m_EffectDesc.fMinSpeed);
	ImGui::InputFloat("MaxSpeed", (float*)&m_EffectDesc.fMaxSpeed);
	if (ImGui::Button("Set_Speed"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MINSPEED, &m_EffectDesc.fMinSpeed);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAXSPEED, &m_EffectDesc.fMaxSpeed);
	}
#pragma endregion
	ImGui::Separator();
#pragma region Scale
	ImGui::Text("**Scale**");
	ImGui::InputFloat3("MinScale", (float*)&m_EffectDesc.vMinScale);
	ImGui::InputFloat3("MaxScale", (float*)&m_EffectDesc.vMaxScale);
	if (ImGui::Button("Set_Scale"))
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MAXSCALE, &m_EffectDesc.vMaxScale);
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_MINSCALE, &m_EffectDesc.vMinScale);
	}
	ImGui::InputFloat3("Start_SCale", (float*)&m_EffectDesc.vStartScale);
	ImGui::InputFloat3("End_Sca", (float*)&m_EffectDesc.vEndScale);


#pragma endregion
	ImGui::Separator();
#pragma region Color
	ImGui::Text("**Color**");
	if (m_EffectDesc.bTexture)
	{
		ImGui::ColorEdit3("TextureColor", (float*)&m_EffectDesc.vTextrueColor);

		if (ImGui::Button("Set_Color"))
		{
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_STARTCOLOR, &m_EffectDesc.vTextrueColor);
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_ENDCOLOR, &m_EffectDesc.vTextrueColor);
		}
	}
	else
	{
		ImGui::ColorEdit3("StartColor", (float*)&m_EffectDesc.vStartColor);
		ImGui::ColorEdit3("EndColor", (float*)&m_EffectDesc.vEndColor);
		if (ImGui::Button("Set_Color"))
		{
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_STARTCOLOR, &m_EffectDesc.vStartColor);
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_ENDCOLOR, &m_EffectDesc.vEndColor);
		}
	}

#pragma endregion

	ImGui::Checkbox("Spark", &m_EffectDesc.bSpark);
	ImGui::SameLine(100);
	ImGui::Checkbox("Particle_Loop", &m_EffectDesc.bParticleLoop);

	if (m_bPrevLoop != m_EffectDesc.bParticleLoop)
	{
		m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_LOOP, &m_EffectDesc.bParticleLoop);
		m_bPrevLoop = m_EffectDesc.bParticleLoop;
	}

#pragma region Figure
	ImGui::Text("**Figure**");
	if (!m_EffectDesc.bTexture)
	{
		_bool bRhombus = m_EffectDesc.bRhombus;
		_bool bRect = m_EffectDesc.bRect;
		_bool bTriangle = m_EffectDesc.bTriangle;

		ImGui::Checkbox("Rhombus", &m_EffectDesc.bRhombus);
		ImGui::SameLine(100);
		ImGui::Checkbox("Rect", &m_EffectDesc.bRect);
		ImGui::SameLine(200);
		ImGui::Checkbox("Triangle", &m_EffectDesc.bTriangle);

		if (bRhombus != m_EffectDesc.bRhombus)
		{
			m_EffectDesc.eFigure = ID_RHOMBUS;
			m_EffectDesc.bRect = false;
			m_EffectDesc.bTriangle = false;
		}
		else if (bRect != m_EffectDesc.bRect)
		{
			m_EffectDesc.eFigure = ID_RECT;
			m_EffectDesc.bRhombus = false;
			m_EffectDesc.bTriangle = false;
		}
		else if (bTriangle != m_EffectDesc.bTriangle)
		{
			m_EffectDesc.eFigure = ID_TRIANGLE;
			m_EffectDesc.bRhombus = false;
			m_EffectDesc.bRect = false;
		}

		if (!m_EffectDesc.bRhombus && !m_EffectDesc.bRect && !m_EffectDesc.bTriangle)
		{
			if (ID_RHOMBUS == m_EffectDesc.eFigure)
				m_EffectDesc.bRhombus = true;
			if (ID_RECT == m_EffectDesc.eFigure)
				m_EffectDesc.bRect = true;
			if (ID_TRIANGLE == m_EffectDesc.eFigure)
				m_EffectDesc.bTriangle = true;
		}
	}

#pragma endregion
	ImGui::Separator();
#pragma region Gravity
	if (!m_EffectDesc.bTexture)
	{
		ImGui::Checkbox("Gravity", &m_EffectDesc.bGravity);
		if (ImGui::Button("Set_Gravity"))
		{
			m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_GRAVITY, &m_EffectDesc.bGravity);

		}
		if (m_EffectDesc.bGravity)
		{
			ImGui::InputFloat("Gravity_Power", (float*)&m_EffectDesc.fGravityPower);

			if (ImGui::Button("Set_Gravity_Power"))
			{
				m_pVIBufferCom->Set_Desc(CVIBuffer_Particle::ParticleNum::ID_GRAVITY_POWER, &m_EffectDesc.fGravityPower);
			}
		}
	}
#pragma endregion
	ImGui::End();
}

void CParticle_Effect::Play_Effect(_float4x4 * pWorldMatrix, _bool bTracking)
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

EFFECT_DESC CParticle_Effect::Get_Effect_Desc()
{

	CVIBuffer_Particle::ParticleDesc PrticleDesc =  m_pVIBufferCom->Get_ParticleDesc();
	
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

	m_EffectDesc.vStartAngle.x = PrticleDesc.fMaxStartDist;
	m_EffectDesc.vStartAngle.y = PrticleDesc.fMinStartDist;

	return m_EffectDesc;
}

HRESULT CParticle_Effect::Add_Component(const EFFECT_DESC & ParticleDesc)
{
	
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER, L"com_renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC Transform_Desc;
	ZeroMemory(&Transform_Desc, sizeof Transform_Desc);

	Transform_Desc.fMoveSpeed = 5.f;
	Transform_Desc.fRotationSpeed = XMConvertToRadians(60.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM, L"com_transform", (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Snow_001", "../../Resource/Texture/Particle/snow_001.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Wenli_80023", "../../Resource/Texture/Wenli/T_Wenli_80023.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Sub_30001", "../../Resource/Texture/Sub/T_Sub_30001.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Sub_30008", "../../Resource/Texture/Sub/T_Sub_30008.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Sub_Smoke_60033", "../../Resource/Texture/Sub/T_Sub_Smoke_60033.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Sub_Smoke_20001", "../../Resource/Texture/Sub/T_Sub_Smoke_20001.dds")))
		return E_FAIL;

	if (FAILED(Add_Texture("T_Ring_009", "../../Resource/Texture/Ring/T_Ring_009.dds")))
		return E_FAIL;

	/*if (FAILED(Add_Texture("T_Sub_Yumao_30001", "../../Resource/Texture/Particle/T_Sub_Yumao_30001.dds")))
		return E_FAIL;*/


	m_pTextureCom = m_Textures.begin()->second;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, SHADER::POINT_INST, L"com_shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

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

	stParticleDesc.vMaxScale = m_EffectDesc.vMaxScale;
	stParticleDesc.vMinScale = m_EffectDesc.vMinScale;

	stParticleDesc.vMinPosition = m_EffectDesc.vMinPosition;
	stParticleDesc.vMaxPosition = m_EffectDesc.vMaxPosition;

	stParticleDesc.vStartColor = m_EffectDesc.vStartColor;
	stParticleDesc.vEndColor = m_EffectDesc.vEndColor;

	stParticleDesc.iNumInstance = m_EffectDesc.iNumInstance;

	stParticleDesc.fGravityPower = m_EffectDesc.fGravityPower;

	stParticleDesc.bGravuty = m_EffectDesc.bGravity;
	stParticleDesc.bLoop = m_EffectDesc.bParticleLoop;
	
	stParticleDesc.fMaxStartDist = MAX_STARTDISTANCE;
	stParticleDesc.fMinStartDist = MIN_DISTANCE;
	stParticleDesc.vRotation = CUMULATIVE_ROTATION;
	
		m_pVIBufferCom = CVIBuffer_Particle::Create(m_pDevice, m_pContext, stParticleDesc);

	return S_OK;
}

HRESULT CParticle_Effect::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetRawValue("g_fScale", &m_EffectDesc.vCurScale, sizeof(_float))))
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

HRESULT CParticle_Effect::Add_Texture(const char * pKeyTag, const char * pCharTag)
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
	strcpy_s(m_FileName[BUFFER_TEX][m_iFileCount[BUFFER_TEX]++], pKeyTag);
	m_Textures.emplace(pKeyTag, pTextureCom);

	return S_OK;
}

void CParticle_Effect::Set_Image(const char * pImageTag)
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

CParticle_Effect * CParticle_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const EFFECT_DESC & ParticleDesc)
{
	CParticle_Effect*		pInstance = new CParticle_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ParticleDesc)))
	{
		MSG_BOX("Failed to Created : CParticle_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Effect::Clone(void * pArg)
{
	CGameObject*		pInstance = new CParticle_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMainTransform);

	for (auto& iter : m_Textures)
	{
		Safe_Release(iter.second);
	}
	m_Textures.clear();
}
