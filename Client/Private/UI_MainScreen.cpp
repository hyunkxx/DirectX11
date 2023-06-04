#include "stdafx.h"
#include "..\Public\UI_MainScreen.h"
#include "GameInstance.h"
#include "GameMode.h"
#include "UI_Mouse.h"
#include "P_PlayerGirl.h"
#include "Character.h"
#include "Terrain.h"
#include "PlayerState.h"
#include "TerminalUI.h"
#include "UI_Tip.h"

CUI_MainScreen::CUI_MainScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_MainScreen::CUI_MainScreen(const CUI_MainScreen& rhs)
	: CGameObject(rhs)
	, m_MainMaskNum(rhs.m_MainMaskNum)
	, m_MainMaskNum2(rhs.m_MainMaskNum2)
	, m_Mini0MaskNum(rhs.m_Mini0MaskNum)
	, m_Mini0MaskNum2(rhs.m_Mini0MaskNum2)
	, m_Mini1MaskNum(rhs.m_Mini1MaskNum)
	, m_Mini1MaskNum2(rhs.m_Mini1MaskNum2)
	, m_Mini2MaskNum(rhs.m_Mini2MaskNum)
	, m_Mini2MaskNum2(rhs.m_Mini2MaskNum2)

{
	for (auto& Desc : rhs.m_CutDescList)
	{
		m_CutDescList.push_back(Desc);
	}

}

HRESULT CUI_MainScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Load();
	return S_OK;
}

HRESULT CUI_MainScreen::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	return S_OK;
}

void CUI_MainScreen::Start()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	m_pPlayer = static_cast<CP_PlayerGirl*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, TEXT("Player")));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pTerminalUI = static_cast<CTerminalUI*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Terminal"));
	m_pTip = static_cast<CUI_Tip*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Tip"));
	
	SetPlayer();
	SetHP();
	SetStaticSkillCoolTime(); // 메인캐릭터로 전체 쿨타임 설정
	
}

void CUI_MainScreen::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (pGameInstance->InputKey(DIK_NUMPADENTER) == KEY_STATE::TAP) // 임시
	{
		m_pPlayerStateClass->AddPlayer();
	}
	if (m_pTerminalUI->IsActive())
		m_bRender = false;
	else if (m_pTip->IsActive())
		m_bRender = false;
	else
		m_bRender = true;

	//OtherobjIsActive(TimeDelta); // 서서히 사라지고 나타나기 수정해야함
	SetPlayer(); // 각 슬롯에 맞는 플레이어 색깔 설정, 스킬 텍스처 설정, 보유캐릭터
	if (m_HadPlayerNum != m_HavePlayerNum)
	{
		SetSlotRender();
	}
	// skillcool
	// 0 = Q, 1 = E , 2 = R
	SetCurCoolTime(); // 메인캐릭터로 현재 쿨타임 설정, 태그쿨
	SetCurCoolRadian(); // 각 라디안 설정
	T(TimeDelta);
	E(TimeDelta);
	Q(TimeDelta);
	R(TimeDelta);
	HP(TimeDelta);
	UVWave(TimeDelta);
	Font(TimeDelta);

	if (QTEFull())
	{
		QTEAct(TimeDelta);
	}
	if (RRFull())
	{
		RRAct(TimeDelta);
	}

}

void CUI_MainScreen::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MainScreen::Render()
{
	if (true == m_bRender) // 다른ui창 활성화때 전체off
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		for (auto& Desc : DamageList)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResourcesDamage(&Desc)))
				return E_FAIL;
			m_pShader->Begin(0);
			m_pVIBuffer->Render();
		}

		for (_uint i = 0; i < (_uint)m_CutDescList.size(); ++i)
		{
			if (true == m_CutDescList[i]->bRender)
			{
				if (eKeyType::T == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesT(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::Q == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesQ(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::E == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesE(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::R == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesR(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::I0 == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesI0(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::I1 == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesI1(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::I2 == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesI2(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}

				else if (eKeyType::RR == (eKeyType)m_CutDescList[i]->eKeyType)
				{

					if (FAILED(Setup_ShaderResourcesRR(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::GRAPH0 == (eKeyType)m_CutDescList[i]->eKeyType)
				{
					if (FAILED(Setup_ShaderResourcesGraphs0(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::GRAPH1 == (eKeyType)m_CutDescList[i]->eKeyType)
				{
					if (FAILED(Setup_ShaderResourcesGraphs1(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if (eKeyType::GRAPH2 == (eKeyType)m_CutDescList[i]->eKeyType)
				{
					if (FAILED(Setup_ShaderResourcesGraphs2(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else if ((true == m_CutDescList[13]->bCoolTime) || (true == m_CutDescList[14]->bCoolTime) || (true == m_CutDescList[15]->bCoolTime))
				{
					if (FAILED(Setup_ShaderResourcesPlayer(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
				else
				{
					if (FAILED(Setup_ShaderResourcesCut(i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
	}
	return S_OK;
}

void CUI_MainScreen::RenderGUI()
{
}

HRESULT CUI_MainScreen::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UI,
		TEXT("com_texFunc"), (CComponent**)&m_pTexFunc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

void CUI_MainScreen::OtherobjIsActive(_double TimeDelta)
{
	if (m_pTerminalUI->IsActive())
	{
		OffRender(TimeDelta);
	}
	else if (m_pTip->IsActive())
	{
		OffRender(TimeDelta);
	}
	else
	{
		Counting();
		if (false == m_bRenderCheck)
		{
			OnRender(TimeDelta);
		}
	}

}

void CUI_MainScreen::OffRender(_double TimeDelta)
{
	for (auto& Desc : m_CutDescList)
	{
		if (true == Desc->bRender)
		{
			if (Desc->fColorACut > -255.f)
			{
				Desc->fColorACut -= (_float)TimeDelta * 50.f;
			}
			else
			{
				Desc->fColorACut = -255.f;
			}
		}
	}
}

void CUI_MainScreen::OnRender(_double TimeDelta)
{
	for (auto& Desc : m_CutDescList)
	{
		if (true == Desc->bRender)
		{
			if (Desc->fColorACut < Desc->ColorCut.w)
			{
				Desc->fColorACut += (_float)TimeDelta * 50.f;
			}
			else
			{
				Desc->fColorACut = Desc->ColorCut.w;
			}
		}
	}
}
void CUI_MainScreen::Counting()
{
	for (auto& Desc : m_CutDescList)
	{
		if (true == Desc->bRender)
		{
			++m_EntireCount;
			if (Desc->fColorACut >= Desc->ColorCut.w)
			{
				++m_Count;
			}
		}
	}
	if (m_EntireCount != m_Count)
	{
		m_bRenderCheck = false;
		m_EntireCount = 0;
		m_Count = 0;
	}
	else
	{
		m_bRenderCheck = true;
		m_EntireCount = 0;
		m_Count = 0;
	}
}


void CUI_MainScreen::QTEAct(_double TimeDelta)
{
	// 38 사이즈 알파 100 * 100
	if (-255.f < m_CutDescList[38]->fColorACut)
	{
		m_CutDescList[38]->fColorACut -= (_float)TimeDelta * 100.f;
		m_CutDescList[38]->fWidthCut += (_float)TimeDelta * 100.f;
		m_CutDescList[38]->fHeightCut += (_float)TimeDelta * 100.f;
	}
	else
	{
		m_CutDescList[38]->fColorACut = 0.f;
		m_CutDescList[38]->fWidthCut = 100.f;
		m_CutDescList[38]->fHeightCut = 100.f;
	}
}

_bool CUI_MainScreen::QTEFull()
{
	if (1.f <= QTERadian)
	{
		m_CutDescList[38]->bRender = true;
		m_CutDescList[51]->bRender = true;
		return true;
	}
	m_CutDescList[38]->bRender = false;
	m_CutDescList[51]->bRender = false;
	return false;
}
void CUI_MainScreen::RRAct(_double TimeDelta)
{
	// 37사이즈 알파 94*94


	if (-255.f < m_CutDescList[37]->fColorACut)
	{
		m_CutDescList[37]->fColorACut -= (_float)TimeDelta * 200.f;
		m_CutDescList[37]->fWidthCut += (_float)TimeDelta * 300.f;
		m_CutDescList[37]->fHeightCut += (_float)TimeDelta * 300.f;
	}
	else
	{
		m_CutDescList[37]->fColorACut = 0.f;
		m_CutDescList[37]->fWidthCut = 110.f;
		m_CutDescList[37]->fHeightCut = 110.f;
	}

	if (-255.f < m_CutDescList[9]->fColorACut)
	{
		m_CutDescList[9]->fColorACut -= (_float)TimeDelta * 300.f;
		m_CutDescList[9]->fWidthCut += (_float)TimeDelta * 40.f;
		m_CutDescList[9]->fHeightCut += (_float)TimeDelta * 40.f;
	}
	else
	{
		m_CutDescList[9]->fColorACut = 0.f;
		m_CutDescList[9]->fWidthCut = 56.f;
		m_CutDescList[9]->fHeightCut = 56.f;
	}
}

_bool CUI_MainScreen::RRFull()
{
	if (1.f <= RRRadian)
	{
		m_CutDescList[37]->bRender = true;
		return true;
	}
	m_CutDescList[37]->fColorACut = 0.f;
	m_CutDescList[37]->fWidthCut = 56.f;
	m_CutDescList[37]->fHeightCut = 56.f;

	m_CutDescList[9]->fColorACut = 0.f;
	m_CutDescList[9]->fWidthCut = 56.f;
	m_CutDescList[9]->fHeightCut = 56.f;

	m_CutDescList[37]->bRender = false;
	return false;
}

void CUI_MainScreen::SetHP()
{
	m_HP = m_pPlayerStateClass->Get_MainCharacterState()->fMaxHP;
	m_CurrentHp = m_pPlayerStateClass->Get_MainCharacterState()->fCurHP;
}

void CUI_MainScreen::SetStaticSkillCoolTime()
{
	MaxTagCool = m_pPlayerStateClass->Get_PlayerState()->fMaxTagCooltime;
	// 0 = Q, 1 = E , 2 = R
	StaticHookCoolTime = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[0]; // 후크
	StaticLevitationCoolTime = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[1]; // 체공
	StaticScannerCoolTimel = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[2]; // 스캐너
	StaticQSkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[0];
	StaticESkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[1];
	StaticRSkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[2];
	RRMaxGauge = m_pPlayerStateClass->Get_MainCharacterState()->fMaxGauge[1];
	QTEMaxGauge = m_pPlayerStateClass->Get_PlayerState()->fMaxQTEGauge;
	SkillMaxGauge = m_pPlayerStateClass->Get_MainCharacterState()->fMaxGauge[0]; // E스킬게이지


	switch (m_pPlayerStateClass->Get_PlayerState()->iCurToolID)
	{
	case 0:
	{
		_int TUnits = (_int)StaticHookCoolTime * 10 / 10; // 1의자리
		_int TTenths = (_int)StaticHookCoolTime * 10 % 10; //0.1자리
		m_CutDescList[71]->iTexNum = 190 + TUnits;
		m_CutDescList[72]->iTexNum = 190 + TTenths;
	}
	break;
	case 1:
	{
		_int TUnits = (_int)StaticLevitationCoolTime * 10 / 10; // 1의자리
		_int TTenths = (_int)StaticLevitationCoolTime * 10 % 10; //0.1자리
		m_CutDescList[71]->iTexNum = 190 + TUnits;
		m_CutDescList[72]->iTexNum = 190 + TTenths;
	}
	break;
	case 2:
	{
		_int TUnits = (_int)StaticScannerCoolTimel * 10 / 10; // 1의자리
		_int TTenths = (_int)StaticScannerCoolTimel * 10 % 10; //0.1자리
		m_CutDescList[71]->iTexNum = 190 + TUnits;
		m_CutDescList[72]->iTexNum = 190 + TTenths;
	}
	break;
	}

	_int EUnits = (_int)StaticESkillTime * 10 / 10; // 1의자리
	_int ETenths = (_int)StaticESkillTime * 10 % 10; //0.1자리
	m_CutDescList[39]->iTexNum = 190 + EUnits;
	m_CutDescList[40]->iTexNum = 190 + EUnits;


	_int QUnits = (_int)StaticQSkillTime * 10 / 10; // 1의자리
	_int QTenths = (_int)StaticQSkillTime * 10 % 10; //0.1자리
	m_CutDescList[41]->iTexNum = 190 + QUnits;
	m_CutDescList[42]->iTexNum = 190 + QTenths;


	_int RUnits = (_int)StaticRSkillTime * 10 / 10; // 1의자리
	_int RTenths = (_int)StaticRSkillTime * 10 % 10; //0.1자리
	m_CutDescList[43]->iTexNum = 190 + RUnits;
	m_CutDescList[44]->iTexNum = 190 + RTenths;
}

void CUI_MainScreen::SetCurCoolTime()
{

	switch (m_pPlayerStateClass->Get_PlayerState()->iCurToolID)
	{
	case 0:
	{
		TCoolTime = m_pPlayerStateClass->Get_PlayerState()->fCurToolCooltime[0];
	}
	break;
	case 1:
	{
		TCoolTime = m_pPlayerStateClass->Get_PlayerState()->fCurToolCooltime[1];
	}
	break;
	case 2:
	{
		TCoolTime = m_pPlayerStateClass->Get_PlayerState()->fCurToolCooltime[2];
	}
	break;
	}

	// 0 = Q, 1 = E , 2 = R
	QCoolTime = m_pPlayerStateClass->Get_MainCharacterState()->fCurCooltime[0];
	ECoolTime = m_pPlayerStateClass->Get_MainCharacterState()->fCurCooltime[1];
	RCoolTime = m_pPlayerStateClass->Get_MainCharacterState()->fCurCooltime[2];
	RRCurGauge = m_pPlayerStateClass->Get_MainCharacterState()->fCurGauge[1];
	QTECurGauge = m_pPlayerStateClass->Get_PlayerState()->fCurQTEGauge;
	SkillCurGauge = m_pPlayerStateClass->Get_MainCharacterState()->fCurGauge[0]; // E스킬게이지
	CurTagCool = m_pPlayerStateClass->Get_PlayerState()->fCurTagCooltime; //태그쿨
}
void CUI_MainScreen::SetCurCoolRadian()
{
	switch (m_pPlayerStateClass->Get_PlayerState()->iCurToolID)
	{
	case 0:
	{
		TCoolRadian = TCoolTime / StaticHookCoolTime;
	}
	break;
	case 1:
	{
		TCoolRadian = TCoolTime / StaticLevitationCoolTime;
	}
	break;
	case 2:
	{
		TCoolRadian = TCoolTime / StaticScannerCoolTimel;
	}
	break;
	}

	QCoolRadian = QCoolTime / StaticQSkillTime;
	ECoolRadian = ECoolTime / StaticESkillTime;
	RCoolRadian = RCoolTime / StaticRSkillTime;
	RRRadian = RRCurGauge / RRMaxGauge;
	QTERadian = QTECurGauge / QTEMaxGauge;
	//QTERadian = 1.f; // 테스트
	SkillRadian = SkillCurGauge / SkillMaxGauge;
	//SkillRadian += 0.001f; // 테스트
	TagRadian = CurTagCool / MaxTagCool;
}

void CUI_MainScreen::SetPlayer()
{
	m_HavePlayerNum = m_pPlayerStateClass->Get_PlayerState()->iCharCount;

	switch (m_pPlayerStateClass->Get_MainCharacterState()->eElement)
	{
	case ELEMENT::ELMT_SPECTRA:
	{
		_float3{ 214.959f * 255.f, 208.637f * 255.f, 126.347f * 255.f }; // 노란색
		m_CutDescList[0]->iTexNum = 4;
		m_CutDescList[51]->iTexNum = 209; // 체크
		m_CutDescList[57]->iTexNum = 205; // 문양
		m_CutDescList[58]->iTexNum = 205; // 문양

		m_CutDescList[16]->fColorRCut = 214.959f;
		m_CutDescList[16]->fColorGCut = 208.637f;
		m_CutDescList[16]->fColorBCut = 126.347f;

		m_CutDescList[54]->fColorRCut = 214.959f;
		m_CutDescList[54]->fColorGCut = 208.637f;
		m_CutDescList[54]->fColorBCut = 126.347f;


		m_CutDescList[8]->fColorRCut = 214.959f;
		m_CutDescList[8]->fColorGCut = 208.637f;
		m_CutDescList[8]->fColorBCut = 126.347f;


		m_CutDescList[70]->fColorRCut = 214.959f;
		m_CutDescList[70]->fColorGCut = 208.637f;
		m_CutDescList[70]->fColorBCut = 126.347f;

		m_CutDescList[36]->fColorRCut = 214.959f;
		m_CutDescList[36]->fColorGCut = 208.637f;
		m_CutDescList[36]->fColorBCut = 126.347f;

		// 스킬 텍스처
		m_CutDescList[5]->iTexNum = 124;
		m_CutDescList[6]->iTexNum = 158;
		m_CutDescList[7]->iTexNum = 125;
	}
	break;
	case ELEMENT::ELMT_CONDUCTO:
	{
		_float3{ 107.f * 255.f, 234.f * 255.f, 219.f * 255.f };
		m_CutDescList[0]->iTexNum = 3;
		m_CutDescList[51]->iTexNum = 208; // 체크
		m_CutDescList[57]->iTexNum = 204; // 문양
		m_CutDescList[58]->iTexNum = 204; // 문양

		m_CutDescList[16]->fColorRCut = 107.f;
		m_CutDescList[16]->fColorGCut = 234.f;
		m_CutDescList[16]->fColorBCut = 219.f;

		m_CutDescList[54]->fColorRCut = 107.f;
		m_CutDescList[54]->fColorGCut = 234.f;
		m_CutDescList[54]->fColorBCut = 219.f;


		m_CutDescList[8]->fColorRCut = 107.f;
		m_CutDescList[8]->fColorGCut = 234.f;
		m_CutDescList[8]->fColorBCut = 219.f;


		m_CutDescList[70]->fColorRCut = 107.f;
		m_CutDescList[70]->fColorGCut = 234.f;
		m_CutDescList[70]->fColorBCut = 219.f;

		m_CutDescList[36]->fColorRCut = 107.f;
		m_CutDescList[36]->fColorGCut = 234.f;
		m_CutDescList[36]->fColorBCut = 219.f;

		// 스킬 텍스처
		m_CutDescList[5]->iTexNum = 114;
		m_CutDescList[6]->iTexNum = 150;
		m_CutDescList[7]->iTexNum = 115;
	}
	break;
	case ELEMENT::ELMT_FUSION:
	{
		_float3{ 158.058f * 255.f, -255.f * 255.f, -231.818f * 255.f };
		m_CutDescList[0]->iTexNum = 5;
		m_CutDescList[51]->iTexNum = 207; // 체크
		m_CutDescList[57]->iTexNum = 206; // 문양
		m_CutDescList[58]->iTexNum = 206; // 문양

		m_CutDescList[16]->fColorRCut = 158.058f;
		m_CutDescList[16]->fColorGCut = 255.f;
		m_CutDescList[16]->fColorBCut = 231.818f;

		m_CutDescList[54]->fColorRCut = 158.058f;
		m_CutDescList[54]->fColorGCut = 255.f;
		m_CutDescList[54]->fColorBCut = 231.818f;


		m_CutDescList[8]->fColorRCut = 158.058f;
		m_CutDescList[8]->fColorGCut = 255.f;
		m_CutDescList[8]->fColorBCut = 231.818f;


		m_CutDescList[70]->fColorRCut = 158.058f;
		m_CutDescList[70]->fColorGCut = 255.f;
		m_CutDescList[70]->fColorBCut = 231.818f;

		m_CutDescList[36]->fColorRCut = 158.058f;
		m_CutDescList[36]->fColorGCut = 255.f;
		m_CutDescList[36]->fColorBCut = 231.818f;

		// 스킬 텍스처
		m_CutDescList[5]->iTexNum = 131;
		m_CutDescList[6]->iTexNum = 149;
		m_CutDescList[7]->iTexNum = 132;
	}
	break;
	default:
		break;
	}
	switch (m_pPlayerStateClass->Get_SecondCharacterState()->eElement)
	{
	case ELEMENT::ELMT_SPECTRA:
	{
		_float3{ 214.959f * 255.f, 208.637f * 255.f, 126.347f * 255.f }; // 노란색
		m_CutDescList[1]->iTexNum = 4;
		m_CutDescList[52]->iTexNum = 209; // 체크
		m_CutDescList[59]->iTexNum = 205; // 문양

		m_CutDescList[55]->fColorRCut = 214.959f * 255.f;
		m_CutDescList[55]->fColorGCut = 208.637f * 255.f;
		m_CutDescList[55]->fColorBCut = 126.347f * 255.f;

	}
	break;
	case ELEMENT::ELMT_CONDUCTO:
	{
		_float3{ 107.f * 255.f, 234.f * 255.f, 219.f * 255.f };
		m_CutDescList[1]->iTexNum = 3;
		m_CutDescList[52]->iTexNum = 208; // 체크
		m_CutDescList[59]->iTexNum = 204; // 문양

		m_CutDescList[55]->fColorRCut = 107.f * 255.f;
		m_CutDescList[55]->fColorGCut = 234.f * 255.f;
		m_CutDescList[55]->fColorBCut = 219.f * 255.f;

	}
	break;
	case ELEMENT::ELMT_FUSION:
	{
		_float3{ 158.058f * 255.f, -255.f * 255.f, -231.818f * 255.f };
		m_CutDescList[1]->iTexNum = 5;
		m_CutDescList[52]->iTexNum = 207; // 체크
		m_CutDescList[59]->iTexNum = 206; // 문양

		m_CutDescList[55]->fColorRCut = 158.058f * 255.f;
		m_CutDescList[55]->fColorGCut = 255.f * 255.f;
		m_CutDescList[55]->fColorBCut = 231.818f * 255.f;

	}
	break;
	default:
		break;
	}

	switch (m_pPlayerStateClass->Get_ThirdCharacterState()->eElement)
	{
	case ELEMENT::ELMT_SPECTRA:
	{
		_float3{ 214.959f * 255.f, 208.637f * 255.f, 126.347f * 255.f }; // 노란색
		m_CutDescList[2]->iTexNum = 4;
		m_CutDescList[53]->iTexNum = 209; // 체크
		m_CutDescList[60]->iTexNum = 205; // 문양

		m_CutDescList[56]->fColorRCut = 214.959f * 255.f;
		m_CutDescList[56]->fColorGCut = 208.637f * 255.f;
		m_CutDescList[56]->fColorBCut = 126.347f * 255.f;
	}
	break;
	case ELEMENT::ELMT_CONDUCTO:
	{
		_float3{ 107.f * 255.f, 234.f * 255.f, 219.f * 255.f };
		m_CutDescList[2]->iTexNum = 3;
		m_CutDescList[53]->iTexNum = 208; // 체크
		m_CutDescList[60]->iTexNum = 204; // 문양

		m_CutDescList[56]->fColorRCut = 107.f * 255.f;
		m_CutDescList[56]->fColorGCut = 234.f * 255.f;
		m_CutDescList[56]->fColorBCut = 219.f * 255.f;

	}
	break;
	case ELEMENT::ELMT_FUSION:
	{
		_float3{ 158.058f * 255.f, -255.f * 255.f, -231.818f * 255.f };
		m_CutDescList[2]->iTexNum = 5;
		m_CutDescList[53]->iTexNum = 207; // 체크
		m_CutDescList[60]->iTexNum = 206; // 문양

		m_CutDescList[56]->fColorRCut = 158.058f * 255.f;
		m_CutDescList[56]->fColorGCut = 255.f * 255.f;
		m_CutDescList[56]->fColorBCut = 231.818f * 255.f;

	}
	break;
	default:
		break;
	}
}
void CUI_MainScreen::SetSlotRender()
{
	switch (m_HavePlayerNum)
	{
	case 1:
	{
		// 플레이어 랜더 쿨타임 추가해야함
		// 새로운 플레이어 추가되면 New Player 흘리고 -> 플레이어 사진만 랜더 상태에서 커졌다 원래 사이즈로 -> 평소 렌더상태
	}
	break;
	case 2:
	{
		RenderPlayer2();
	}
	break;
	case 3:
	{
		RenderPlayer2();
		RenderPlayer3();
	}
	break;
	}
}

void CUI_MainScreen::Font(_double TimeDelta)
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
		Desc.Color.w -= (_float)TimeDelta * 150.f;

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
			Acc += (_float)TimeDelta *1000.f;
			Desc.Pos.x += sinf(XMConvertToRadians(Acc) + 2.f);
			Desc.Pos.y += sinf(XMConvertToRadians(Acc));
		}
		if (-100.f < Desc.Color.w)
		{

			Desc.Pos.y += (_float)TimeDelta * 40.f;
		}
		if (-180.f >= Desc.Color.w)
		{
			Desc.Pos.y += (_float)TimeDelta * 100.f;
		}

		XMStoreFloat4x4(&(Desc.WorldMat), XMMatrixScaling(Desc.Size.x, Desc.Size.y, 0.f)
			* XMMatrixTranslation(Desc.Pos.x, Desc.Pos.y, 0.f));
	}

}

void	CUI_MainScreen::Damage(_float Damage)
{
	_int iDamage = (_int)Damage;
	_int Damage1000, Damage100, Damage10, Damage1;
	Damage1000 = iDamage / 1000;
	iDamage = iDamage % 1000;
	Damage100 = iDamage / 100;
	iDamage = iDamage % 100;
	Damage10 = iDamage / 10;
	Damage1 = iDamage % 10;

	if (Damage < -40)
	{
		fFontColor = _float4{ 255.f, -255.f, -255.f,0.f };
	}
	_float3 PlayerPos;
	XMStoreFloat3(&PlayerPos, m_pPlayer->Get_Position());

	if (0 != Damage1000)
	{
		DAMAGEDESC Desc1000;
		Desc1000.Pos = PlayerPos;
		Desc1000.Size = _float2{ 30.f, 30.f };
		Desc1000.TextureNum = -Damage1000 + 33;
		Desc1000.Color = fFontColor;
		DamageList.push_back(Desc1000);

		DAMAGEDESC Desc100;
		Desc100.Pos = _float3{ Desc1000.Pos.x + (15.f) , Desc1000.Pos.y, 0.f };
		Desc100.Size = _float2{ 30.f, 30.f };
		Desc100.TextureNum = -Damage100 + 33;
		Desc100.Color = fFontColor;
		DamageList.push_back(Desc100);

		DAMAGEDESC Desc10;
		Desc10.Pos = _float3{ Desc1000.Pos.x + (30.f) , Desc1000.Pos.y, 0.f };
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = fFontColor;
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.Pos = _float3{ Desc1000.Pos.x + (45.f), Desc1000.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		DamageList.push_back(Desc1);

	}
	else if ((0 == Damage1000) && (0 != Damage100))
	{

		DAMAGEDESC Desc100;
		Desc100.Pos = PlayerPos;
		Desc100.Size = _float2{ 30.f, 30.f };
		Desc100.TextureNum = -Damage100 + 33;
		Desc100.Color = fFontColor;
		DamageList.push_back(Desc100);

		DAMAGEDESC Desc10;
		Desc10.Pos = _float3{ Desc100.Pos.x + (15.f) , Desc100.Pos.y ,0.f };
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = fFontColor;
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.Pos = _float3{ Desc100.Pos.x + (30.f), Desc100.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		DamageList.push_back(Desc1);

	}
	else if ((0 == Damage1000) && (0 == Damage100) && (0 != Damage10))
	{
		DAMAGEDESC Desc10;
		Desc10.Pos = PlayerPos;
		Desc10.Size = _float2{ 30.f, 30.f };
		Desc10.TextureNum = -Damage10 + 33;
		Desc10.Color = fFontColor;
		DamageList.push_back(Desc10);

		DAMAGEDESC Desc1;
		Desc1.Pos = _float3{ Desc10.Pos.x + (15.f), Desc10.Pos.y, 0.f };
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		DamageList.push_back(Desc1);

	}
	else
	{
		DAMAGEDESC Desc1;
		Desc1.Pos = PlayerPos;
		Desc1.Size = _float2{ 30.f, 30.f };
		Desc1.TextureNum = -Damage1 + 33;
		Desc1.Color = fFontColor;
		DamageList.push_back(Desc1);
	}

}

void CUI_MainScreen::T(_double TimeDelta)
{
	if (0.f != TCoolTime)
	{
		TCoolRender();
		_int Units = (_int)TCoolTime * 10 / 10;
		_int Tenths = (_int)(TCoolTime * 10.f) % 10;
		m_CutDescList[71]->iTexNum = 190 + Units;
		m_CutDescList[72]->iTexNum = 190 + Tenths;

	}
	if ((0.1f > TCoolTime) && (TCoolTime > 0.f)) // 현재 플레이어가 쓴 쿨타임이 끝났다면
	{
		TCoolRenderOff();
		bTEnd = true;//엔드모션
	}
	if (true == bTEnd)
	{
		TEnd(TimeDelta); // bool값 반환 
	}
}

void CUI_MainScreen::E(_double TimeDelta)
{
	if (0.f != ECoolTime)
	{
		ECoolRender();
		_int Units = (_int)ECoolTime * 10 / 10;
		_int Tenths = (_int)(ECoolTime * 10.f) % 10;
		m_CutDescList[39]->iTexNum = 190 + Units;
		m_CutDescList[40]->iTexNum = 190 + Tenths;

	}
	if ((0.1f > ECoolTime) && (ECoolTime > 0.f)) // 현재 플레이어가 쓴 쿨타임이 끝났다면
	{
		ECoolRenderOff();
		bEEnd = true;//엔드모션
	}
	if (true == bEEnd)
	{
		EEnd(TimeDelta); // bool값 반환 
	}
}

void CUI_MainScreen::Q(_double TimeDelta)
{
	if (0.f != QCoolTime)
	{
		QCoolRender();
		_int Units = (_int)QCoolTime * 10 / 10;
		_int Tenths = (_int)(QCoolTime * 10.f) % 10;
		m_CutDescList[41]->iTexNum = 190 + Units;
		m_CutDescList[42]->iTexNum = 190 + Tenths;

	}
	if ((0.1f > QCoolTime) && (QCoolTime > 0.f)) // 현재 플레이어가 쓴 쿨타임이 끝났다면
	{
		QCoolRenderOff();
		bQEnd = true;//엔드모션
	}
	if (true == bQEnd)
	{
		QEnd(TimeDelta); // bool값 반환 
	}
}
void CUI_MainScreen::R(_double TimeDelta)
{
	if (0.f != RCoolTime)
	{
		RCoolRender();
		_int Units = (_int)RCoolTime * 10 / 10;
		_int Tenths = (_int)(RCoolTime * 10.f) % 10;
		m_CutDescList[43]->iTexNum = 190 + Units;
		m_CutDescList[44]->iTexNum = 190 + Tenths;

	}
	if ((0.1f > RCoolTime) && (RCoolTime > 0.f)) // 현재 플레이어가 쓴 쿨타임이 끝났다면
	{
		RCoolRenderOff();
		bREnd = true;//엔드모션
	}
	if (true == bREnd)
	{
		REnd(TimeDelta); // bool값 반환 
	}
}

void CUI_MainScreen::HP(_double TimeDelta)
{
	if (true == m_bHit)
	{
		HPBar(TimeDelta);

	}
	if (true == m_bRedStart)
	{
		HPRedBar(TimeDelta);
	}
}

_float CUI_MainScreen::Distance(_fvector TargetPos, _fvector CurrentPos)
{
	return XMVectorGetX(XMVector3Length(TargetPos - CurrentPos));
}


_bool CUI_MainScreen::SizeXYP(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	_float fWidth = pDesc->fWidthCut + pDesc->fSpeedWidth * (_float)TimeDelta;
	pDesc->fWidthCut = pDesc->WorldMatrixCut._11 = fWidth;

	_float fHeight = pDesc->fHeightCut + pDesc->fSpeedHeight * (_float)TimeDelta;
	pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		return true;
	}
	return false;
}

_bool CUI_MainScreen::SizeXYM(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	_float fWidth = pDesc->fWidthCut - pDesc->fSpeedWidth * (_float)TimeDelta;
	pDesc->fWidthCut = pDesc->WorldMatrixCut._11 = fWidth;

	_float fHeight = pDesc->fHeightCut - pDesc->fSpeedHeight * (_float)TimeDelta;
	pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		return true;
	}
	return false;
}


void CUI_MainScreen::AlphaM(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bAlphaM = false;
	}

	pDesc->fColorACut -= pDesc->fSpeedAlpha * (_float)TimeDelta;
}

void CUI_MainScreen::AlphaP(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bAlphaP = false;
	}

	pDesc->fColorACut += pDesc->fSpeedAlpha * (_float)TimeDelta;
}



void CUI_MainScreen::CoolTimeEnd(_double TimeDelta)
{

	CUTRECT* pDesc = nullptr;/*
	if (m_iPrePlayer == eKeyType::DANSUN )
	{
		pDesc = m_CutDescList[0];

	}
	if (m_iPrePlayer == eKeyType::YANGYANG)
	{
		pDesc = m_CutDescList[1];
	}
	if (m_iPrePlayer == eKeyType::RED)
	{
		pDesc = m_CutDescList[2];
	}*/

	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc < (pDesc->Duration) / 2.f)
	{
		_float fWidth = pDesc->fWidthCut + pDesc->fSpeedWidth * (_float)TimeDelta;
		pDesc->fWidthCut = pDesc->WorldMatrixCut._11 = fWidth;

		_float fHeight = pDesc->fHeightCut + pDesc->fSpeedHeight * (_float)TimeDelta;
		pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;
	}
	else if ((pDesc->TimeAcc < pDesc->Duration) && (pDesc->TimeAcc >(pDesc->Duration) / 2.f))
	{
		_float fWidth = pDesc->fWidthCut - pDesc->fSpeedWidth * (_float)TimeDelta;
		pDesc->fWidthCut = pDesc->WorldMatrixCut._11 = fWidth;

		_float fHeight = pDesc->fHeightCut - pDesc->fSpeedHeight * (_float)TimeDelta;
		pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;
	}
	else
	{
		pDesc->fWidthCut = 60.f;
		pDesc->fHeightCut = 60.f;
		pDesc->TimeAcc = 0.f;
		Coolend = false;

	}
}

_bool CUI_MainScreen::CoolTime(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (1.f < pDesc->Cool)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bCoolTime = false;
		pDesc->Cool = 0.f;
		switch ((eKeyType)pDesc->eKeyType)
		{
		case Client::CUI_MainScreen::eKeyType::DANSUN :
		{
			m_CutDescList[13]->bRender = false;
			m_CutDescList[45]->bRender = false;
			m_CutDescList[46]->bRender = false;
			m_CutDescList[66]->bRender = false;
			m_CutDescList[45]->iTexNum = 190;
			m_CutDescList[46]->iTexNum = 199;

			// 쿨타임껍데기랑 카운트 랜더 끄기
		}
		break;
		case Client::CUI_MainScreen::eKeyType::YANGYANG:
		{
			m_CutDescList[14]->bRender = false;
			m_CutDescList[47]->bRender = false;
			m_CutDescList[48]->bRender = false;
			m_CutDescList[65]->bRender = false;
			m_CutDescList[47]->iTexNum = 190;
			m_CutDescList[48]->iTexNum = 199;

		}
		break;
		case Client::CUI_MainScreen::eKeyType::RED:
		{
			m_CutDescList[15]->bRender = false;
			m_CutDescList[49]->bRender = false;
			m_CutDescList[50]->bRender = false;
			m_CutDescList[64]->bRender = false;
			m_CutDescList[49]->iTexNum = 190;
			m_CutDescList[50]->iTexNum = 199;

		}
		break;
		default:
			break;
		}
		return true;
	}

	if (1.f > pDesc->Cool)
	{
		pDesc->Cool += (_float)TimeDelta / pDesc->Duration; /*uv좌표 기준으로 m_Cool를 1로 잡고 원하는 쿨타임 == 매 틱/ 전체시간, 텍스처 크기로 잡을 필요x */
	}
	return false;
}


_bool CUI_MainScreen::TEnd(_double TimeDelta)
{

	m_CutDescList[4]->TimeAcc += (_float)TimeDelta;

	if (m_CutDescList[4]->TimeAcc < (m_CutDescList[4]->Duration) / 2.f)
	{
		_float fWidth = m_CutDescList[4]->fWidthCut + m_CutDescList[4]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[4]->fWidthCut = m_CutDescList[4]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[4]->fHeightCut + m_CutDescList[4]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[4]->fHeightCut = m_CutDescList[4]->WorldMatrixCut._22 = fHeight;
	}
	else if ((m_CutDescList[4]->TimeAcc < m_CutDescList[4]->Duration) && (m_CutDescList[4]->TimeAcc >(m_CutDescList[4]->Duration) / 2.f))
	{
		_float fWidth = m_CutDescList[4]->fWidthCut - m_CutDescList[4]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[4]->fWidthCut = m_CutDescList[4]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[4]->fHeightCut - m_CutDescList[4]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[4]->fHeightCut = m_CutDescList[4]->WorldMatrixCut._22 = fHeight;
	}
	else
	{
		m_CutDescList[4]->fWidthCut = 56.f;
		m_CutDescList[4]->fHeightCut = 56.f;
		m_CutDescList[4]->TimeAcc = 0.f;
		bTEnd = false;
		return true;
	}
	return false;
}

_bool CUI_MainScreen::EEnd(_double TimeDelta)
{

	m_CutDescList[5]->TimeAcc += (_float)TimeDelta;

	if (m_CutDescList[5]->TimeAcc < (m_CutDescList[5]->Duration) / 2.f)
	{
		_float fWidth = m_CutDescList[5]->fWidthCut + m_CutDescList[5]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[5]->fWidthCut = m_CutDescList[5]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[5]->fHeightCut + m_CutDescList[5]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[5]->fHeightCut = m_CutDescList[5]->WorldMatrixCut._22 = fHeight;
	}
	else if ((m_CutDescList[5]->TimeAcc < m_CutDescList[5]->Duration) && (m_CutDescList[5]->TimeAcc >(m_CutDescList[5]->Duration) / 2.f))
	{
		_float fWidth = m_CutDescList[5]->fWidthCut - m_CutDescList[5]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[5]->fWidthCut = m_CutDescList[5]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[5]->fHeightCut - m_CutDescList[5]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[5]->fHeightCut = m_CutDescList[5]->WorldMatrixCut._22 = fHeight;
	}
	else
	{
		m_CutDescList[5]->fWidthCut = 56.f;
		m_CutDescList[5]->fHeightCut = 56.f;
		m_CutDescList[5]->TimeAcc = 0.f;
		bEEnd = false;
		return true;
	}
	return false;
}

_bool CUI_MainScreen::QEnd(_double TimeDelta)
{
	m_CutDescList[6]->TimeAcc += (_float)TimeDelta;

	if (m_CutDescList[6]->TimeAcc < (m_CutDescList[6]->Duration) / 2.f)
	{
		_float fWidth = m_CutDescList[6]->fWidthCut + m_CutDescList[6]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[6]->fWidthCut = m_CutDescList[6]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[6]->fHeightCut + m_CutDescList[6]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[6]->fHeightCut = m_CutDescList[6]->WorldMatrixCut._22 = fHeight;
	}
	else if ((m_CutDescList[6]->TimeAcc < m_CutDescList[6]->Duration) && (m_CutDescList[6]->TimeAcc >(m_CutDescList[6]->Duration) / 2.f))
	{
		_float fWidth = m_CutDescList[6]->fWidthCut - m_CutDescList[6]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[6]->fWidthCut = m_CutDescList[6]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[6]->fHeightCut - m_CutDescList[6]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[6]->fHeightCut = m_CutDescList[6]->WorldMatrixCut._22 = fHeight;
	}
	else
	{
		m_CutDescList[6]->fWidthCut = 56.f;
		m_CutDescList[6]->fHeightCut = 56.f;
		m_CutDescList[6]->TimeAcc = 0.f;
		bQEnd = false;
		return true;
	}
	return false;
}

_bool CUI_MainScreen::REnd(_double TimeDelta)
{
	m_CutDescList[7]->TimeAcc += (_float)TimeDelta;

	if (m_CutDescList[7]->TimeAcc < (m_CutDescList[7]->Duration) / 2.f)
	{
		_float fWidth = m_CutDescList[7]->fWidthCut + m_CutDescList[7]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[7]->fWidthCut = m_CutDescList[7]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[7]->fHeightCut + m_CutDescList[7]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[7]->fHeightCut = m_CutDescList[7]->WorldMatrixCut._22 = fHeight;
	}
	else if ((m_CutDescList[7]->TimeAcc < m_CutDescList[7]->Duration) && (m_CutDescList[7]->TimeAcc >(m_CutDescList[7]->Duration) / 2.f))
	{
		_float fWidth = m_CutDescList[7]->fWidthCut - m_CutDescList[7]->fSpeedWidth * (_float)TimeDelta;
		m_CutDescList[7]->fWidthCut = m_CutDescList[7]->WorldMatrixCut._11 = fWidth;

		_float fHeight = m_CutDescList[7]->fHeightCut - m_CutDescList[7]->fSpeedHeight * (_float)TimeDelta;
		m_CutDescList[7]->fHeightCut = m_CutDescList[7]->WorldMatrixCut._22 = fHeight;
	}
	else
	{
		m_CutDescList[7]->fWidthCut = 56.f;
		m_CutDescList[7]->fHeightCut = 56.f;
		m_CutDescList[7]->TimeAcc = 0.f;
		bREnd = false;
		return true;
	}
	return false;
}


void CUI_MainScreen::RenderPlayer1()
{

	m_CutDescList[0]->bRender = true;
	m_CutDescList[10]->bRender = true;
	m_CutDescList[54]->bRender = true;
	m_CutDescList[58]->bRender = true;


	m_CutDescList[51]->bRender = false;
	m_CutDescList[13]->bRender = false;
	m_CutDescList[16]->bRender = false;
	m_CutDescList[45]->bRender = false;
	m_CutDescList[46]->bRender = false;
	m_CutDescList[66]->bRender = false;
}

void CUI_MainScreen::RenderPlayer2()
{
	m_CutDescList[1]->bRender = true;
	m_CutDescList[11]->bRender = true;
	m_CutDescList[55]->bRender = true;
	m_CutDescList[59]->bRender = true;


	m_CutDescList[52]->bRender = false;
	m_CutDescList[14]->bRender = false;
	m_CutDescList[17]->bRender = false;
	m_CutDescList[47]->bRender = false;
	m_CutDescList[48]->bRender = false;
	m_CutDescList[65]->bRender = false;
}

void CUI_MainScreen::RenderPlayer3()
{
	m_CutDescList[2]->bRender = true;
	m_CutDescList[12]->bRender = true;
	m_CutDescList[56]->bRender = true;
	m_CutDescList[60]->bRender = true;


	m_CutDescList[53]->bRender = false;
	m_CutDescList[15]->bRender = false;
	m_CutDescList[18]->bRender = false;
	m_CutDescList[49]->bRender = false;
	m_CutDescList[50]->bRender = false;
	m_CutDescList[64]->bRender = false;
}



void CUI_MainScreen::Load()
{
	_uint index = 75;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/MainScreen%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		CUTRECT*  Desc = new CUTRECT;
		ZeroMemory(Desc, sizeof(CUTRECT));
		XMStoreFloat4x4(&(Desc->WorldMatrixCut), XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc->fXCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fYCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidthCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeightCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorACut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorRCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorGCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorBCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->TimeAcc), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bCoolTime), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->eKeyType), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRender), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->AimPos), sizeof(_float2), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->OriPos), sizeof(_float2), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);
		if (109 == Desc->iTexNum)
		{
			Desc->fColorRCut = 255.f;
			Desc->fColorGCut = 255.f;
			Desc->fColorBCut = 255.f;
		}
		if (32 == i)
		{
			Desc->fColorRCut = 255.f;
			Desc->fColorGCut = -255.f;
			Desc->fColorBCut = -255.f;
		}

		if (nullptr != Desc)
		{
			Desc->ColorCut.w = Desc->fColorACut;
			Desc->OriPos = _float2(Desc->fXCut, Desc->fYCut);
			Desc->AimPos = _float2(640.f + Desc->fWidthCut /2.f , -360.f - Desc->fHeightCut / 2.f);
			XMStoreFloat4x4(&(Desc->WorldMatrixCut), XMMatrixScaling(Desc->fWidthCut, Desc->fHeightCut, 1.f)
				* XMMatrixTranslation(Desc->fXCut, Desc->fYCut, Desc->fZCut));

			m_CutDescList.push_back(Desc);

		}
	}

	TCHAR	szFileName[128] = L"";
	wsprintf(szFileName, L"../../Data/UI/Maskindex.dat");
	HANDLE hFile = CreateFile(
		szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_ulong dwByte = 0;

	ReadFile(hFile, &m_MainMaskNum, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_MainMaskNum2, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini0MaskNum, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini0MaskNum2, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini1MaskNum, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini1MaskNum2, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini2MaskNum, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &m_Mini2MaskNum2, sizeof(_int), &dwByte, nullptr);

	CloseHandle(hFile);

}


void CUI_MainScreen::UVWave(_double TimeDelta)
{
	m_fUV.x += (_float)TimeDelta * 0.3f;
	if (1.f <= m_fUV.x)
	{
		m_fUV.x = 0.f;
	}
}

void CUI_MainScreen::HPBar(_double TimeDelta)
{
	if (0.f < m_fWhiteBar)
	{
		if (0 < m_Damage) // 힐
		{
			if (m_PreHp + m_Damage < m_CurrentHp) // 힐 받은만큼만 참
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
			m_fWhiteBar = m_CurrentHp / m_HP; // 현재체력/전체체력 
			m_PreHp = m_CurrentHp;
			m_bHit = false;
			m_bRedStart = true;
		}
	}
}

void CUI_MainScreen::HPRedBar(_double TimeDelta)
{
	if (m_CurrentHp < m_RedHP)
	{
		m_RedHP += (m_CurrentHp - m_RedHP) * (_float)TimeDelta;
		m_fRedBar = m_RedHP / m_HP;
	}
	else
	{
		// 피가 다 닳았으면
		m_bRedStart = false;

	}
}

void CUI_MainScreen::Set_Texchange(_int Texindex)
{
	m_CutDescList[4]->iTexNum = Texindex;
}

//on
void CUI_MainScreen::TCoolRender()
{
	m_CutDescList[74]->bCoolTime = true;
	m_CutDescList[71]->bRender = true;
	m_CutDescList[72]->bRender = true;
	m_CutDescList[73]->bRender = true;
	m_CutDescList[74]->bRender = true;
}

void CUI_MainScreen::QCoolRender()
{
	m_CutDescList[34]->bCoolTime = true;
	m_CutDescList[41]->bRender = true;
	m_CutDescList[42]->bRender = true;
	m_CutDescList[62]->bRender = true;
	m_CutDescList[34]->bRender = true;
}

void CUI_MainScreen::ECoolRender()
{
	m_CutDescList[33]->bCoolTime = true;
	m_CutDescList[39]->bRender = true;
	m_CutDescList[40]->bRender = true;
	m_CutDescList[61]->bRender = true;
	m_CutDescList[33]->bRender = true;
}

void CUI_MainScreen::RCoolRender()
{
	m_CutDescList[35]->bCoolTime = true;
	m_CutDescList[43]->bRender = true;
	m_CutDescList[44]->bRender = true;
	m_CutDescList[63]->bRender = true;
	m_CutDescList[35]->bRender = true;
}

//off
void CUI_MainScreen::TCoolRenderOff()
{
	m_CutDescList[71]->bRender = false;
	m_CutDescList[72]->bRender = false;
	m_CutDescList[73]->bRender = false;
	m_CutDescList[74]->bRender = false;
}

void CUI_MainScreen::QCoolRenderOff()
{
	m_CutDescList[41]->bRender = false;
	m_CutDescList[42]->bRender = false;
	m_CutDescList[62]->bRender = false;
	m_CutDescList[34]->bRender = false;
}

void CUI_MainScreen::ECoolRenderOff()
{
	m_CutDescList[39]->bRender = false;
	m_CutDescList[40]->bRender = false;
	m_CutDescList[61]->bRender = false;
	m_CutDescList[33]->bRender = false;
}

void CUI_MainScreen::RCoolRenderOff()
{
	m_CutDescList[43]->bRender = false;
	m_CutDescList[44]->bRender = false;
	m_CutDescList[63]->bRender = false;
	m_CutDescList[35]->bRender = false;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesDamage(DAMAGEDESC* pDamage)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", pDamage->TextureNum)))
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

HRESULT CUI_MainScreen::Setup_ShaderResourcesCut(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (10 == m_CutDescList[Bufferindex]->iPass)
		{
			if (FAILED(m_pShader->SetRawValue("g_GraphUV", &m_fUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_GMask", m_MainMaskNum)))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_GMask2", m_MainMaskNum2)))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_SkillRadian", &SkillRadian, sizeof(_float))))// 70번 스킬강화게이지 오른쪽으로 어디까지 찼는지
				return E_FAIL;
		}
		
		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f) 
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		if (true == m_bRedStart)
		{
			if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
				return E_FAIL;
		}
		if (FAILED(m_pShader->SetRawValue("g_fLoading", &m_fWhiteBar, sizeof(_float))))
			return E_FAIL;


		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesGraphs0(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		// 메인 그래프 -> 패스 10번으로 바꾸고 g_GraphUV로 바꾸기 -> 오른쪽에서 짤리는 기능 추가
		// 플레이어가 때릴때마다 차는 0~1 사이의 uv변수 추가해서 추가로 던져주고 uv.x보다 크면 discard 하기 
		// 미니그래프들은 셋리소스 분리해서 pass 6번으로 하고 왼쪽으로 갈수록 연해지는 기능 추가 g_UV 사용
		// uv흐르는속도는 메인,미니 동일해야해서 변수추가할필요x 
		// 플레이어 바꿀때 기본텍스처랑 마스크 이미지만 바꿔줄것

		if (6 == m_CutDescList[Bufferindex]->iPass)
		{
			if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini0MaskNum)))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini0MaskNum2)))
				return E_FAIL;
		}
		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesGraphs1(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (6 == m_CutDescList[Bufferindex]->iPass)
		{
			if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini0MaskNum)))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini0MaskNum2)))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesGraphs2(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (6 == m_CutDescList[Bufferindex]->iPass)
		{
			if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini0MaskNum)))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini0MaskNum2)))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesT(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (m_CutDescList[74]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &TCoolRadian, sizeof(_float))))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesQ(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (m_CutDescList[34]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &QCoolRadian, sizeof(_float))))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesE(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (m_CutDescList[33]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &ECoolRadian, sizeof(_float))))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesR(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (m_CutDescList[35]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &RCoolRadian, sizeof(_float))))
				return E_FAIL;
		}

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesRR(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &RRRadian, sizeof(_float))))
			return E_FAIL;


		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesI0(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &QTERadian, sizeof(_float)))) // 8번 QTE
			return E_FAIL;

		

		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}
HRESULT CUI_MainScreen::Setup_ShaderResourcesI1(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;


		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &QTERadian, sizeof(_float)))) // 8번 QTE
			return E_FAIL;


		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}
HRESULT CUI_MainScreen::Setup_ShaderResourcesI2(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;


		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &QTERadian, sizeof(_float)))) // 8번 QTE
			return E_FAIL;


		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

HRESULT CUI_MainScreen::Setup_ShaderResourcesPlayer(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		// 쿨타임시 보낼 텍스처
		if (m_CutDescList[13]->bCoolTime)
		{
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 3))) // 3->캐릭터텍스처의 main%d
				return E_FAIL;
		}
		if (m_CutDescList[14]->bCoolTime)
		{
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 4)))
				return E_FAIL;
		}
		if (m_CutDescList[15]->bCoolTime)
		{
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 5)))
				return E_FAIL;
		}


		XMStoreFloat4x4(&(m_CutDescList[Bufferindex]->WorldMatrixCut), XMMatrixScaling(m_CutDescList[Bufferindex]->fWidthCut, m_CutDescList[Bufferindex]->fHeightCut, 1.f)
			* XMMatrixTranslation(m_CutDescList[Bufferindex]->fXCut, m_CutDescList[Bufferindex]->fYCut, m_CutDescList[Bufferindex]->fZCut));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CutDescList[Bufferindex]->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CutDescList[Bufferindex]->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CutDescList[Bufferindex]->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CutDescList[Bufferindex]->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CutDescList[Bufferindex]->fColorACut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fCoolTime", &(m_CutDescList[Bufferindex]->Cool), sizeof(_float))))
			return E_FAIL;

		m_iPass = m_CutDescList[Bufferindex]->iPass;
	}
	return S_OK;
}

CUI_MainScreen* CUI_MainScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MainScreen* pInstance = new CUI_MainScreen(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_MainScreen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MainScreen::Clone(void* pArg)
{
	CUI_MainScreen* pInstance = new CUI_MainScreen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_MainScreen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainScreen::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexFunc);
	Safe_Release(m_pVIBuffer);

	if (!m_bClone)
	{
		for (auto& Desc : m_CutDescList)
		{
			delete Desc;
			Desc = nullptr;
		}
		m_CutDescList.clear();

	}
}
