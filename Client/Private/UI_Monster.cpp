
#include "stdafx.h"
#include "..\Public\UI_Monster.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "UI_MainScreen.h"
#include "UI_Mouse.h"
#include "P_PlayerGirl.h"
#include "DynamicCamera.h"
#include "PlayerState.h"
CUI_Monster::CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Monster::CUI_Monster(const CUI_Monster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUI_Monster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg))) // ?????? ???? ????? ???? ??? ????? ????on ?????
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	Load();
	MONINFO MonInfo;
	memcpy(&MonInfo, pArg, sizeof(MONINFO));
	m_MonsterLevel = MonInfo.Level;
	m_MonsterType = MonInfo.Type;
	return S_OK;
}

void CUI_Monster::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
}

void CUI_Monster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (pGameInstance->InputKey(DIK_MINUS) == KEY_STATE::TAP)
	{
		if (true == m_bNameRender)
		{
			m_bNameRender = false;
		}
		else
		{
			m_bNameRender = true;
		}
	}

	CommonHP();
	DecideRender();
	CommonLevel();
	Font((_float)TimeDelta);

	if (true == m_bHit)
	{
		HPBar(TimeDelta);

	}
	if (true == m_bRedStart)
	{
		HPRedBar(TimeDelta);
	}
	m_MonsterUV.x += (_float)TimeDelta*0.3f;
	//m_MonsterGauge += (_float)TimeDelta*0.2f;
}

void CUI_Monster::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Monster::Render()
{
	if (true == m_bRender)
	{
		for (auto& Desc : DamageList)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResourcesDamage(&Desc)))
				return E_FAIL;
			m_pShader->Begin(0);
			m_pVIBuffer->Render();
		}

		for (_uint i = 0; i < 4; ++i)
		{
			if (true == m_DescList[i].bRender)
			{
				if (FAILED(__super::Render()))
					return E_FAIL;
				if (FAILED(Setup_ShaderResourcesHP(i)))
					return E_FAIL;
				m_pShader->Begin(m_DescList[i].iPass);
				m_pVIBuffer->Render();
			}
		}

		for (_uint i = 4; i < 8; ++i)
		{
			if (true == m_DescList[i].bRender)
			{
				if (FAILED(__super::Render()))
					return E_FAIL;
				if (FAILED(Setup_ShaderResources(i)))
					return E_FAIL;
				m_pShader->Begin(m_DescList[i].iPass);
				m_pVIBuffer->Render();
			}
		}


		if (true == m_DescList[8].bRender)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResources(8)))
				return E_FAIL;
			m_pShader->Begin(m_DescList[8].iPass);
			m_pVIBuffer->Render();
		}

		for (_uint i = 9; i < 10; ++i)
		{
			if (true == m_DescList[i].bRender)
			{
				if (FAILED(__super::Render()))
					return E_FAIL;
				if (FAILED(Setup_ShaderResourcesMask(i)))
					return E_FAIL;
				m_pShader->Begin(m_DescList[i].iPass);
				m_pVIBuffer->Render();
			}
		}



		for (_uint i = 10; i < (_uint)m_DescList.size(); ++i)
		{
			if (true == m_DescList[i].bRender)
			{
				if (FAILED(__super::Render()))
					return E_FAIL;
				if (FAILED(Setup_ShaderResourcesBoss(i)))
					return E_FAIL;
				m_pShader->Begin(m_DescList[i].iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	return S_OK;
}

void CUI_Monster::RenderGUI()
{
}

void CUI_Monster::Font(_double TimeDelta)
{
	if (0 < (_int)DamageList.size())
	{
		list<DAMAGEDESC>::iterator iter = DamageList.begin();
		for (iter; iter != DamageList.end();)
		{
			if (-255.f > iter->Color.w)
			{
				iter = DamageList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	for (auto& Desc : DamageList)
	{
		if (78 == Desc.TextureNum)
		{
			Desc.Size.x += (_float)(TimeDelta * 500.f);
			Desc.Size.y -= (_float)(TimeDelta * 30.f);
			if (0.f >= Desc.Size.y)
			{
				Desc.Size.y = 0.f;
			}
		}


		Desc.Color.w -= (_float)(TimeDelta * 150.f);

		if (-20.f < Desc.Color.w)
		{
			Desc.Size.x += (_float)TimeDelta * 200.f;
			Desc.Size.y += (_float)TimeDelta * 200.f;
		}
		if ((-20.f > Desc.Color.w) && (-30.f > Desc.Color.w))
		{
			if (30.f >= Desc.Size.x)
			{
				Desc.Size.x = 30.f;
				Desc.Size.y = 30.f;
			}
			else
			{
				Desc.Size.x -= (_float)TimeDelta * 400.f;
				Desc.Size.y -= (_float)TimeDelta * 400.f;
			}
		}



		if (-30.f < Desc.Color.w)
		{
			Acc += (_float)(TimeDelta *1000.f);
			Desc.Pos.x += sinf(XMConvertToRadians(Acc) + 2.f);
			Desc.Pos.y += sinf(XMConvertToRadians(Acc));
		}
		if (-100.f < Desc.Color.w)
		{

			Desc.Pos.y += (_float)(TimeDelta * 40.f);
		}
		if (-180.f >= Desc.Color.w)
		{
			Desc.Pos.y += (_float)(TimeDelta * 100.f);
		}

		XMStoreFloat4x4(&(Desc.WorldMat), XMMatrixScaling(Desc.Size.x, Desc.Size.y, 0.f)
			* XMMatrixTranslation(Desc.Pos.x, Desc.Pos.y, 0.f));
	}

}

void	CUI_Monster::Damage(_float Damage)
{
	FontColor();
	_int iDamage = (_int)Damage;
	_int Damage1000, Damage100, Damage10, Damage1;
	Damage1000 = iDamage / 1000;
	iDamage = iDamage % 1000;
	Damage100 = iDamage / 100;
	iDamage = iDamage % 100;
	Damage10 = iDamage / 10;
	Damage1 = iDamage % 10;
	++m_HitCount;
	if (10 == m_HitCount)
		m_HitCount = 1;
	srand((unsigned int)time(NULL));
	_float pos = _float(rand() % (100 + 1 - (-100)) + (-100)); 

	if (0 == m_HitCount % 2)
	{
		pos = (_float)(pos * 1.3);
	}

	if (0 != Damage1000)
	{
		DAMAGEDESC Desc1000;
		Desc1000.HitCount = m_HitCount;
		Desc1000.Pos = _float3{ m_DescList[0].fX + pos, m_DescList[0].fY + pos, 0.f };
		Desc1000.Size = _float2{ 30.f, 30.f };
		Desc1000.TextureNum = -Damage1000 + 33;
		Desc1000.Color = fFontColor;
		if (0 == m_HitCount / 2)
			Desc1000.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc1000.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc1000);

		DAMAGEDESC Desc100;
		Desc100.HitCount = m_HitCount;
		Desc100.Pos = _float3{ Desc1000.Pos.x + (15.f) , Desc1000.Pos.y, 0.f };
		Desc100.Size = _float2{ 30.f, 30.f };
		Desc100.TextureNum = -Damage100 + 33;
		Desc100.Color = fFontColor;
		if (0 == m_HitCount / 2)
			Desc100.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc100.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc100);

		DAMAGEDESC Desc10;
		Desc10.HitCount = m_HitCount;
		Desc10.Pos = _float3{ Desc1000.Pos.x + (30.f) , Desc1000.Pos.y, 0.f };
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = fFontColor;
		if (0 == m_HitCount / 2)
			Desc10.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc10.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.HitCount = m_HitCount;
		Desc1.Pos = _float3{ Desc1000.Pos.x + (45.f), Desc1000.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		if (0 == m_HitCount / 2)
			Desc1.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc1.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc1);
		if (0 == m_HitCount / 3)
		{
			DAMAGEDESC Desc;
			Desc.HitCount = m_HitCount;
			Desc.Pos = _float3{ (Desc1000.Pos.x + Desc1.Pos.x) / 2.f, Desc1000.Pos.y , 0.01f };
			Desc.Size = _float2{ 30.f, 20.f };
			Desc.TextureNum = 78;
			Desc.Color = _float4{ 0.f, 0.f,0.f,0.f };
			XMStoreFloat4x4(&Desc.WorldMat, XMMatrixIdentity());
			DamageList.push_back(Desc);
		}

	}
	else if ((0 == Damage1000) && (0 != Damage100))
	{

		DAMAGEDESC Desc100;
		Desc100.HitCount = m_HitCount;
		Desc100.Pos = _float3{ m_DescList[0].fX + pos, m_DescList[0].fY + pos, 0.f };
		Desc100.Size = _float2{ 30.f, 30.f };
		Desc100.TextureNum = -Damage100 + 33;
		Desc100.Color = fFontColor;
		if (0 == m_HitCount / 3)
			Desc100.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc100.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc100);

		DAMAGEDESC Desc10;
		Desc10.HitCount = m_HitCount;
		Desc10.Pos = _float3{ Desc100.Pos.x + (15.f) , Desc100.Pos.y ,0.f };
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = fFontColor;
		if (0 == m_HitCount / 3)
			Desc10.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc10.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.HitCount = m_HitCount;
		Desc1.Pos = _float3{ Desc100.Pos.x + (30.f), Desc100.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		if (0 == m_HitCount / 3)
			Desc1.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc1.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc1);
		if (0 == m_HitCount / 3)
		{
			DAMAGEDESC Desc;
			Desc.HitCount = m_HitCount;
			Desc.Pos = _float3{ (Desc100.Pos.x + Desc1.Pos.x) / 2.f, Desc100.Pos.y, 0.01f };
			Desc.Size = _float2{ 30.f, 20.f };
			Desc.TextureNum = 78;
			Desc.Color = fFontColor;
			XMStoreFloat4x4(&Desc.WorldMat, XMMatrixIdentity());
			DamageList.push_back(Desc);
		}

	}
	else if ((0 == Damage1000) && (0 == Damage100) && (0 != Damage10))
	{
		DAMAGEDESC Desc10;
		Desc10.HitCount = m_HitCount;
		Desc10.Pos = _float3{ m_DescList[0].fX + pos, m_DescList[0].fY + pos, 0.f };
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = _float4{ 0.f, 0.f,0.f,0.f };
		if (0 == m_HitCount / 3)
			Desc10.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc10.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.HitCount = m_HitCount;
		Desc1.Pos = _float3{ Desc10.Pos.x + (15.f), Desc10.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = _float4{ 0.f, 0.f,0.f,0.f };
		if (0 == m_HitCount / 3)
			Desc1.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc1.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc1);
		if (0 == m_HitCount / 3)
		{
			DAMAGEDESC Desc;
			Desc.HitCount = m_HitCount;
			Desc.Pos = _float3{ (Desc10.Pos.x + Desc1.Pos.x) / 2.f,Desc10.Pos.y,0.01f };
			Desc.Size = _float2{ 30.f, 20.f };
			Desc.TextureNum = 78;
			Desc.Color = _float4{ 0.f, 0.f,0.f,0.f };
			XMStoreFloat4x4(&Desc.WorldMat, XMMatrixIdentity());
			DamageList.push_back(Desc);
		}

	}
	else
	{
		DAMAGEDESC Desc1;
		Desc1.HitCount = m_HitCount;
		Desc1.Pos = _float3{ m_DescList[0].fX + pos, m_DescList[0].fY + pos, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = _float4{ 0.f, 0.f,0.f,0.f };
		if (0 == m_HitCount / 2)
			Desc1.Color = _float4{ -10.f, -7.f, -90.f, 0.f };
		XMStoreFloat4x4(&Desc1.WorldMat, XMMatrixIdentity());
		DamageList.push_back(Desc1);
		if (0 == m_HitCount / 3)
		{
			DAMAGEDESC Desc;
			Desc.HitCount = m_HitCount;
			Desc.Pos = _float3{ Desc1.Pos.x, Desc1.Pos.y ,0.01f };
			Desc.Size = _float2{ 30.f, 20.f };
			Desc.TextureNum = 78;
			Desc.Color = _float4{ 0.f, 0.f,0.f,0.f };
			XMStoreFloat4x4(&Desc.WorldMat, XMMatrixIdentity());
			DamageList.push_back(Desc);
		}
	}

}

void CUI_Monster::FontColor()
{
	switch (m_pPlayerStateClass->Get_MainCharacterState()->eElement)
	{
	case ELEMENT::ELMT_SPECTRA:
	{
		fFontColor = _float4(214.959f , 208.637f , 126.347f , 0.f );
	}
	break;
	case ELEMENT::ELMT_CONDUCTO:
	{
		fFontColor = _float4( 107.f, 234.f, 219.f, 0.f );
	}
	break;
	case ELEMENT::ELMT_FUSION:
	{
		fFontColor = _float4(158.058f, -255.f, -231.818f, 0.f);
	}
	break;
	}

}
HRESULT CUI_Monster::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*???? ?????  ????????? ??? , ?ех???? ????  ????? ??? ???. ????? ?????????*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIFIGHT,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 0.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	return S_OK;

}


HRESULT CUI_Monster::Setup_ShaderResourcesHP(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index].iTexNum)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index].WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	if (true == m_bRedStart)
	{
		if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetRawValue("g_fwhiteBar", &m_fWhiteBar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Monster::Setup_ShaderResources(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index].iTexNum)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index].WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Monster::Setup_ShaderResourcesMask(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_GraphUV", &(m_MonsterUV), sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_GMask", m_MaskTextureNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_GMask2", m_MaskTextureNum2)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_SkillRadian", &m_MonsterGauge, sizeof(_float)))) 
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index].WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


HRESULT CUI_Monster::Setup_ShaderResourcesBoss(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_GraphUV", &(m_MonsterUV), sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_GMask", m_MaskTextureNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_GMask2", m_MaskTextureNum2)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_SkillRadian", &m_MonsterGauge, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index].WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index].fColorA), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fwhiteBar", &m_fWhiteBar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


HRESULT CUI_Monster::Setup_ShaderResourcesDamage(DAMAGEDESC* pDamage)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", pDamage->TextureNum)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(pDamage->WorldMat))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(pDamage->Color.x), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(pDamage->Color.y), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(pDamage->Color.z), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(pDamage->Color.w), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


CUI_Monster* CUI_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Monster* pInstance = new CUI_Monster(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Monster";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Monster::Clone(void* pArg)
{
	CUI_Monster* pInstance = new CUI_Monster(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Monster";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Monster::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pMainTransform);

}

void CUI_Monster::HPBar(_double TimeDelta)
{
	if (0.f < m_fWhiteBar)
	{
		if (0 < m_Damage) // ??
		{
			if (m_PreHp + m_Damage < m_CurrentHp) // ?? ????????? ??
			{
				m_bHit = false;
				m_PreHp = m_RedHP = m_CurrentHp;
			}
			m_CurrentHp += m_Damage * (_float)TimeDelta;
			m_fWhiteBar = m_CurrentHp / m_HP;
		}
		else
		{
			m_CurrentHp += m_Damage;
			m_fWhiteBar = m_CurrentHp / m_HP; // ???????/?????? 
			m_PreHp = m_CurrentHp;
			m_bHit = false;
			m_bRedStart = true;
		}
	}
}

void CUI_Monster::HPRedBar(_double TimeDelta)
{
	if (m_CurrentHp < m_RedHP)
	{
		m_RedHP += (m_CurrentHp - m_RedHP) * (_float)TimeDelta;
		m_fRedBar = m_RedHP / m_HP;
	}
	else
	{
		// ??? ?? ???????
		m_bRedStart = false;

	}
}

void CUI_Monster::CommonHP()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	_float4 fCharacterPos;
	_vector View, ViewProj;
	View = XMVector3TransformCoord(m_vCharacterPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));
	ViewProj = XMVector3TransformCoord(View, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ));
	XMStoreFloat4(&fCharacterPos, ViewProj);
	m_DescList[0].fX = (fCharacterPos.x) * (0.5f * g_iWinSizeX);
	m_DescList[0].fY = (fCharacterPos.y) * (0.5f * g_iWinSizeY);


	for (_uint i = 0; i < 4; ++i)
	{
		m_DescList[i].fX = m_DescList[0].fX;
		m_DescList[i].fY = m_DescList[0].fY;

		XMStoreFloat4x4(&(m_DescList[i].WorldMatrix), XMMatrixScaling((m_DescList[i]).fWidth, (m_DescList[i]).fHeight, 1.f)
			* XMMatrixTranslation((m_DescList[i]).fX, (m_DescList[i]).fY, (m_DescList[i]).fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		m_DescList[i].bRender = true;
	}
}

void CUI_Monster::CommonLevel()
{
	_int LevelTen = m_MonsterLevel / 10;
	_int LevelOne = m_MonsterLevel % 10; // 0 = 33 1 = 34

	if (1> LevelTen)
	{
		m_DescList[5].bRender = false;
		m_DescList[6].iTexNum = LevelOne + 79;
	}
	else
	{
		m_DescList[5].bRender = true;
		m_DescList[5].iTexNum = LevelTen + 79;
		m_DescList[6].iTexNum = LevelOne + 79;
	}

	if (3 < (_uint)m_DescList.size())
	{
		for (_uint i = 4; i < 9; ++i)
		{
			m_DescList[i].fX = m_DescList[0].fX + m_DescList[i].PosSet.x;
			m_DescList[i].fY = m_DescList[0].fY + m_DescList[i].PosSet.y;

			XMStoreFloat4x4(&(m_DescList[i].WorldMatrix), XMMatrixScaling((m_DescList[i]).fWidth, (m_DescList[i]).fHeight, 1.f)
				* XMMatrixTranslation((m_DescList[i]).fX, (m_DescList[i]).fY, (m_DescList[i]).fZ));
			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		}
	}
}

void CUI_Monster::DecideRender()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix CamMat = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_vector CamLook = CamMat.r[2];
	_vector CamPos = CamMat.r[3];
	XMVectorSetY(CamLook, 0.f);
	XMVectorSetY(CamPos, 0.f);
	_vector vNormailzeDir = XMVector4Normalize(m_vCharacterPos - CamPos);
	_vector vNormailzeLook = XMVector4Normalize(CamLook);

	_float Get = XMVectorGetX(XMVector4Dot(vNormailzeLook, vNormailzeDir));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, m_vCharacterPos);
	for (auto& Desc : m_DescList)
	{
		_float Dsit = m_pMainTransform->Compute_Distance(CamPos);
		if (Dsit > 30.f)
		{
			Desc.bRender = false;
		}
		else 
		{
				if (-0.2f > Get) 
				{
					Desc.bRender = false;

				}
			else
				{
						switch (m_MonsterType)
						{
						case Client::CUI_Monster::MONSTERTYPE::TYPE0: 
						{
							m_DescList[8].bRender = false;
							m_DescList[9].bRender = false;


							if (true == m_bNameRender)
							{
								m_DescList[4].bRender = false;
								m_DescList[5].bRender = false;
								m_DescList[6].bRender = false;
								m_DescList[7].bRender = true;
							}
							else
							{
								m_DescList[4].bRender = true;
								m_DescList[5].bRender = true;
								m_DescList[6].bRender = true;
								m_DescList[7].bRender = false;
							}
						}
						break;
						case Client::CUI_Monster::MONSTERTYPE::TYPE1: 
						{
							m_DescList[8].bRender = true;
							m_DescList[9].bRender = true;


							if (true == m_bNameRender)
							{
								m_DescList[4].bRender = false;
								m_DescList[5].bRender = false;
								m_DescList[6].bRender = false;
								m_DescList[7].bRender = true;
							}
							else
							{
								m_DescList[4].bRender = true;
								m_DescList[5].bRender = true;
								m_DescList[6].bRender = true;
								m_DescList[7].bRender = false;
							}

						}
						break;
						case Client::CUI_Monster::MONSTERTYPE::BOSS:
						{
							for (_uint i = 0; i < 10; ++i)
							{
								m_DescList[i].bRender = false;
							}
							for (_uint i = 10; i < (_uint)m_DescList.size(); ++i)
							{
								m_DescList[i].bRender = true;
								XMStoreFloat4x4(&(m_DescList[i].WorldMatrix), XMMatrixScaling(m_DescList[i].fWidth, m_DescList[i].fHeight, 1.f) *
									XMMatrixTranslation(m_DescList[i].fX, m_DescList[i].fY, m_DescList[i].fZ));
								XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
								XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
							}

							_int LevelTen = m_MonsterLevel / 10;
							_int LevelOne = m_MonsterLevel % 10; // 0 = 33 1 = 34

							if (1> LevelTen)
							{
								//11,12,13
								m_DescList[13].bRender = false;
								m_DescList[14].iTexNum = LevelOne + 79;
							}
							else
							{
								m_DescList[13].bRender = true;
								m_DescList[13].iTexNum = LevelTen + 79;
								m_DescList[14].iTexNum = LevelOne + 79;
							}
						}
						break;
						}

				}
		}
	}

}

void CUI_Monster::Load()
{
	_uint index = 19;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/UIMonster%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MONSTERDESC  Desc;
		ZeroMemory(&Desc, sizeof(MONSTERDESC));
		XMStoreFloat4x4(&Desc.WorldMatrix, XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fDegree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.PosSet), sizeof(_float2), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);

		Desc.bRender = true;
		m_DescList.push_back(Desc);

	}

}
