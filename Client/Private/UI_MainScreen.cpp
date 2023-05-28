#include "stdafx.h"
#include "..\Public\UI_MainScreen.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "P_PlayerGirl.h"
#include "Character.h"
#include "Terrain.h"
#include "PlayerState.h"

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

	Add_PlayerNum();
	--m_HadPlayerNum;

	return S_OK;
}

void CUI_MainScreen::Start()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	m_pPlayer = static_cast<CP_PlayerGirl*>(pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Player")));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	
	SetPlayerColor();
	SetStaticSkillCoolTime(); // 메인캐릭터로 전체 쿨타임 설정
	SetHP();
	
}

void CUI_MainScreen::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	SetPlayerColor(); // 각 슬롯에 맞는 플레이어 색깔 설정, 현재 보유한 캐릭터 수
	 // skillcool
	 // 0 = Q, 1 = E , 2 = R
	 SetCurCoolTime(); // 메인캐릭터로 현재 쿨타임 설정 
	 SetCurCoolRadian(); // 각 라디안 설정
	 
	 // 보유 캐릭터에 따라서 랜더onoff
	 switch (m_HavePlayerNum)
	 {
	 case 1:
	 {
		// 무조건 현재 플레이어
		 //플레이어스테이트에 현재플레이어 Set++() 만들기->플레이어 추가 확인용
		 // SetCurSkill() -> 현재 플레이어에 따라 스킬 바꿔줘야함
		 // SetQTEFull() -> 51, 52, 53번 체크는 QTE 다 차면 랜더,효과 추가해야함 -> x,y 사이즈 커지면서 알파 줄어듦 -> 알파 사라지면 사이즈,알파 초기화 -> QTECur가 0이 될 때 멈춤
		 // SetRRFull() -> RR 다 차면 효과 추가해야함
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

	 if (0.f != TCoolTime)
	 {
		 TCoolRender();
		 _int Units = (_int)TCoolTime * 10 / 10;
		 _int Tenths = (_int)(TCoolTime * 10.f) % 10;
		 m_CutDescList[71]->iTexNum = 190 + Units;
		 m_CutDescList[72]->iTexNum = 190 + Tenths;

	 }
	 if((0.1f > TCoolTime)&&(TCoolTime > 0.f)) // 현재 플레이어가 쓴 쿨타임이 끝났다면
	 {
		 TCoolRenderOff();
		 bTEnd = true;//엔드모션
	 }
	 if (true == bTEnd)
	 {
		 TEnd(TimeDelta); // bool값 반환 
	 }

	 //
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

	 //
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

	 //
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

	 if (true == m_bHit)
	 {
		 HPBar(TimeDelta);

	 }
	 if (true == m_bRedStart)
	 {
		 HPRedBar(TimeDelta);
	 }
	 UVWave(TimeDelta);
}

void CUI_MainScreen::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}


HRESULT CUI_MainScreen::Render()
{
	if (true == m_bRender)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

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

void CUI_MainScreen::SerectUI()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameObject* pMouse = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("layer_UI"), TEXT("UI_Mouse"));
	if (nullptr != pMouse)
	{
		_float3	fMousePos = dynamic_cast<CUI_Mouse*>(pMouse)->Get_MousePos();
		_vector vMousePos = XMLoadFloat3(&fMousePos);

		for (auto& pCutDesc : m_CutDescList)
		{
			_float Dist = 1.f;
			// 버퍼의 각 꼭지점
			_vector P0 = XMVectorSet(pCutDesc->fXCut - pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut + pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P1 = XMVectorSet(pCutDesc->fXCut + pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut + pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P2 = XMVectorSet(pCutDesc->fXCut + pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut - pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P3 = XMVectorSet(pCutDesc->fXCut - pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut - pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);

			// UI크기에 맞춰서 범위체크
			if ((XMVectorGetX(P0) < XMVectorGetX(vMousePos)) && (XMVectorGetX(P1) > XMVectorGetX(vMousePos)))
			{
				if ((XMVectorGetY(P0) > XMVectorGetY(vMousePos)) && (XMVectorGetY(P2) < XMVectorGetY(vMousePos)))
				{
					pCutDesc->OnRect = true;
					CGameObject* pMouse = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("UI_Mouse"));
					dynamic_cast<CUI_Mouse*>(pMouse)->Set_Texchange(true);
				}

			}
		}
	}
}

void CUI_MainScreen::SetHP()
{
	m_HP = m_pPlayerStateClass->Get_MainCharacterState()->fMaxHP;
	m_CurrentHp = m_pPlayerStateClass->Get_MainCharacterState()->fCurHP;
}

void CUI_MainScreen::SetStaticSkillCoolTime()
{
	// 0 = Q, 1 = E , 2 = R
	StaticHookCoolTime		 = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[0]; // 후크
	StaticLevitationCoolTime = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[1]; // 체공
	StaticScannerCoolTimel	 = m_pPlayerStateClass->Get_PlayerState()->fMaxToolCooltime[2]; // 스캐너
	StaticQSkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[0];
	StaticESkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[1];
	StaticRSkillTime = m_pPlayerStateClass->Get_MainCharacterState()->fMaxCooltime[2];
	RRMaxGauge = m_pPlayerStateClass->Get_MainCharacterState()->fMaxGauge[2];
	QTEMaxGauge = m_pPlayerStateClass->Get_PlayerState()->fMaxQTEGauge;
	SkillMaxGauge = m_pPlayerStateClass->Get_MainCharacterState()->fMaxGauge[1]; // E스킬게이지


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
	RRCurGauge = m_pPlayerStateClass->Get_MainCharacterState()->fCurGauge[2];
	QTECurGauge = m_pPlayerStateClass->Get_PlayerState()->fCurQTEGauge;
	SkillCurGauge = m_pPlayerStateClass->Get_MainCharacterState()->fCurGauge[1]; // E스킬게이지
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
	//QTERadian = QTECurGauge / QTEMaxGauge;
	QTERadian += 0.001f; // 테스트
	//SkillRadian = SkillCurGauge / SkillMaxGauge;
	SkillRadian += 0.001f; // 테스트
}

void CUI_MainScreen::SetPlayerColor()
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



HRESULT CUI_MainScreen::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UI,
		TEXT("com_texFunc"), (CComponent**)&m_pTexFunc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}
void CUI_MainScreen::Start_Go(_double TimeDelta)
{
	// 사라지는 타이밍 잡아서 호출
	DisappearIcon(0, 3, TimeDelta);
	DisappearIcon(10, 19, TimeDelta);
	DisappearIcon(45, 57, TimeDelta);
	DisappearIcon(58, 61, TimeDelta);
	DisappearIcon(64, 67, TimeDelta);

}

void CUI_MainScreen::Start_Come(_double TimeDelta)
{
	// 나타나는 타이밍 잡아서 호출
	AppearIcon(0, 3, TimeDelta);
	AppearIcon(10, 19, TimeDelta);
	AppearIcon(45, 57, TimeDelta);
	AppearIcon(58, 61, TimeDelta);
	AppearIcon(64, 67, TimeDelta);

}


void CUI_MainScreen::DisappearIcon(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector AimPos = XMVectorSet(m_CutDescList[i]->AimPos.x, m_CutDescList[i]->AimPos.y, 0.01f, 1.f);
		_vector OriPos = XMVectorSet(m_CutDescList[i]->OriPos.x, m_CutDescList[i]->OriPos.y, 0.01f, 1.f);
		_vector vDir = AimPos - OriPos;

		_vector CurrentPos = XMVectorSet(m_CutDescList[i]->fXCut, m_CutDescList[i]->fYCut, 0.0f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta;

		_vector pos = CurrentPos;
		pos += (AimPos - CurrentPos) * (_float)TimeDelta;
		_float Dist = Distance(AimPos, CurrentPos);



		m_CutDescList[i]->fXCut = XMVectorGetX(pos);
		m_CutDescList[i]->fYCut = XMVectorGetY(pos);

		if (0.1f > Dist)
		{
			m_CutDescList[i]->fXCut = m_CutDescList[i]->AimPos.x;
		}
	}
}

void CUI_MainScreen::AppearIcon(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector AimPos = XMVectorSet(m_CutDescList[i]->AimPos.x, m_CutDescList[i]->AimPos.y, 0.01f, 1.f);
		_vector OriPos = XMVectorSet(m_CutDescList[i]->OriPos.x, m_CutDescList[i]->OriPos.y, 0.01f, 1.f);
		_vector vDir = OriPos - AimPos;

		_vector CurrentPos = XMVectorSet(m_CutDescList[i]->fXCut, m_CutDescList[i]->fYCut, 0.0f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta;

		_vector pos = CurrentPos;
		pos += (OriPos - CurrentPos) * (_float)TimeDelta;
		_float Dist = Distance(OriPos, CurrentPos);


		if (0.1f < Dist)
		{
			m_CutDescList[i]->fXCut = XMVectorGetX(pos);
			m_CutDescList[i]->fYCut = XMVectorGetY(pos);
		}
		else
		{
			m_CutDescList[i]->fXCut = m_CutDescList[i]->OriPos.x;
		}

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

void CUI_MainScreen::CountDownCool(_double TimeDelta)
{

	if (m_iPrePlayer == eKeyType::DANSUN )
	{
		m_CutDescList[45]->iTexNum = 190;
		if (Time >= 1.f - m_CutDescList[13]->TimeAcc)
		{
			Time -= 0.1f;
			m_CutDescList[46]->iTexNum -= 1;
			if (190 >= m_CutDescList[46]->iTexNum)
			{
				m_CutDescList[46]->iTexNum = 190;
			}
		}
	}
	if (m_iPrePlayer == eKeyType::YANGYANG)
	{
		m_CutDescList[47]->iTexNum = 190;
		if (Time >= 1.f - m_CutDescList[14]->TimeAcc)
		{
			Time -= 0.1f;
			m_CutDescList[48]->iTexNum -= 1;
			if (190 >= m_CutDescList[48]->iTexNum)
			{
				m_CutDescList[48]->iTexNum = 190;
			}
		}
	}
	if (m_iPrePlayer == eKeyType::RED)
	{
		m_CutDescList[49]->iTexNum = 190;
		if (Time >= 1.f - m_CutDescList[15]->TimeAcc)
		{
			Time -= 0.1f;
			m_CutDescList[50]->iTexNum -= 1;
			if (190 >= m_CutDescList[50]->iTexNum)
			{
				m_CutDescList[50]->iTexNum = 190;
			}
		}
	}

}



void CUI_MainScreen::CoolTimeEnd(_double TimeDelta)
{

	CUTRECT* pDesc = nullptr;
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
	}

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

void	CUI_MainScreen::RenderCurrentPlayer1()
{
	/*
	레벨
	경험치
	공
	방
	체
	크리확률
	*/
	m_CutDescList[0]->bRender = true;
	m_CutDescList[16]->bRender = true;
	m_CutDescList[51]->bRender = true;
	m_CutDescList[54]->bRender = true;
	m_CutDescList[58]->bRender = true;

	m_CutDescList[10]->bRender = false;
	m_CutDescList[13]->bRender = false;
	m_CutDescList[45]->bRender = false;
	m_CutDescList[46]->bRender = false;
	m_CutDescList[66]->bRender = false;

	//스킬
	m_CutDescList[5]->iTexNum = 124;
	m_CutDescList[6]->iTexNum = 158;
	m_CutDescList[7]->iTexNum = 125;

}

void CUI_MainScreen::RenderCurrentPlayer2()
{
	m_CutDescList[1]->bRender = true;
	m_CutDescList[17]->bRender = true;
	m_CutDescList[52]->bRender = true;
	m_CutDescList[55]->bRender = true;
	m_CutDescList[59]->bRender = true;

	m_CutDescList[11]->bRender = false;
	m_CutDescList[14]->bRender = false;
	m_CutDescList[47]->bRender = false;
	m_CutDescList[48]->bRender = false;
	m_CutDescList[65]->bRender = false;

	//스킬
	m_CutDescList[5]->iTexNum = 78;
	m_CutDescList[6]->iTexNum = 79;
	m_CutDescList[7]->iTexNum = 80;
}

void CUI_MainScreen::RenderCurrentPlayer3()
{
	m_CutDescList[2]->bRender = true;
	m_CutDescList[18]->bRender = true;
	m_CutDescList[53]->bRender = true;
	m_CutDescList[56]->bRender = true;
	m_CutDescList[60]->bRender = true;

	m_CutDescList[12]->bRender = false;
	m_CutDescList[15]->bRender = false;
	m_CutDescList[49]->bRender = false;
	m_CutDescList[50]->bRender = false;
	m_CutDescList[64]->bRender = false;


	//스킬
	m_CutDescList[5]->iTexNum = 82;
	m_CutDescList[6]->iTexNum = 83;
	m_CutDescList[7]->iTexNum = 84;
}


void CUI_MainScreen::RenderPlayer1()
{

	m_CutDescList[0]->bRender = true;
	m_CutDescList[10]->bRender = true;
	m_CutDescList[51]->bRender = true;
	m_CutDescList[54]->bRender = true;
	m_CutDescList[58]->bRender = true;


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
	m_CutDescList[52]->bRender = true;
	m_CutDescList[55]->bRender = true;
	m_CutDescList[59]->bRender = true;


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
	m_CutDescList[53]->bRender = true;
	m_CutDescList[56]->bRender = true;
	m_CutDescList[60]->bRender = true;


	m_CutDescList[15]->bRender = false;
	m_CutDescList[18]->bRender = false;
	m_CutDescList[49]->bRender = false;
	m_CutDescList[50]->bRender = false;
	m_CutDescList[64]->bRender = false;
}


void CUI_MainScreen::RenderOffPlayer1()
{
	m_CutDescList[0]->bRender = false;
	m_CutDescList[10]->bRender = false;
	m_CutDescList[13]->bRender = false;
	m_CutDescList[16]->bRender = false;
	m_CutDescList[45]->bRender = false;
	m_CutDescList[46]->bRender = false;
	m_CutDescList[51]->bRender = false;
	m_CutDescList[54]->bRender = false;
	m_CutDescList[58]->bRender = false;
	m_CutDescList[66]->bRender = false;
}

void CUI_MainScreen::RenderOffPlayer2()
{
	m_CutDescList[1]->bRender = false;
	m_CutDescList[11]->bRender = false;
	m_CutDescList[14]->bRender = false;
	m_CutDescList[17]->bRender = false;
	m_CutDescList[47]->bRender = false;
	m_CutDescList[48]->bRender = false;
	m_CutDescList[52]->bRender = false;
	m_CutDescList[55]->bRender = false;
	m_CutDescList[59]->bRender = false;
	m_CutDescList[65]->bRender = false;
}

void CUI_MainScreen::RenderOffPlayer3()
{
	m_CutDescList[2]->bRender = false;
	m_CutDescList[12]->bRender = false;
	m_CutDescList[15]->bRender = false;
	m_CutDescList[18]->bRender = false;
	m_CutDescList[49]->bRender = false;
	m_CutDescList[50]->bRender = false;
	m_CutDescList[53]->bRender = false;
	m_CutDescList[56]->bRender = false;
	m_CutDescList[60]->bRender = false;
	m_CutDescList[64]->bRender = false;
}

void CUI_MainScreen::AddPlayerRender()
{
	if (1 == m_HavePlayerNum)
	{
		RenderCurrentPlayer1();
		RenderOffPlayer2();
		RenderOffPlayer3();
	}
	else if (2 == m_HavePlayerNum)
	{
		switch (m_iCurrentPlayer)
		{
		case Client::CUI_MainScreen::eKeyType::YANGYANG:
		{
			RenderCurrentPlayer1();
			RenderPlayer2();
			RenderOffPlayer3();
		}
		break;
		case Client::CUI_MainScreen::eKeyType::DANSUN:
		{
			RenderCurrentPlayer2();
			RenderPlayer1();
			RenderOffPlayer3();
		}
		break;
		default:
			break;
		}

	}
	else
	{
		switch (m_iCurrentPlayer)
		{
		case Client::CUI_MainScreen::eKeyType::YANGYANG:
		{
			RenderCurrentPlayer1();
			RenderPlayer2();
			RenderPlayer3();
		}
		break;
		case Client::CUI_MainScreen::eKeyType::DANSUN:
		{
			RenderCurrentPlayer2();
			RenderPlayer1();
			RenderPlayer3();
		}
		break;
		case Client::CUI_MainScreen::eKeyType::RED:
		{
			RenderCurrentPlayer3();
			RenderPlayer1();
			RenderPlayer2();
		}
		break;
		default:
			break;
		}
	}
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
				m_PreHp = m_RedDamageACC = m_CurrentHp;
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
	if (m_CurrentHp < m_RedDamageACC)
	{
		m_RedDamageACC += (m_CurrentHp - m_RedDamageACC) * (_float)TimeDelta;
		m_fRedBar = m_RedDamageACC / m_HP;
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


void CUI_MainScreen::ChoicePlayer1()
{

	if ((nullptr != m_CutDescList[14]) && (false == m_CutDescList[13]->bCoolTime) && (false == m_CutDescList[14]->bCoolTime) && (false == m_CutDescList[15]->bCoolTime))
	{

		m_iPrePlayer = m_iCurrentPlayer;
		m_iCurrentPlayer = eKeyType::DANSUN ;
		m_CutDescList[8]->Cool = m_CutDescList[54]->Cool; // 플레이어 재선택쿨
		m_CutDescList[57]->iTexNum = m_CutDescList[58]->iTexNum;
		m_CutDescList[36]->iTexNum = 202;
		m_CutDescList[54]->fColorRCut = m_CutDescList[8]->fColorRCut = m_CutDescList[70]->fColorRCut = m_CutDescList[16]->fColorRCut;
		m_CutDescList[54]->fColorGCut = m_CutDescList[8]->fColorGCut = m_CutDescList[70]->fColorGCut = m_CutDescList[16]->fColorGCut;
		m_CutDescList[54]->fColorBCut = m_CutDescList[8]->fColorBCut = m_CutDescList[70]->fColorBCut = m_CutDescList[16]->fColorBCut;


		if ((eKeyType::YANGYANG == m_iPrePlayer) && (1 < m_HavePlayerNum)) // 2번에서 1번 플레이어 선택
		{
			RenderCurrentPlayer1();
			m_CutDescList[14]->bRender = true;
			m_CutDescList[14]->bCoolTime = true; //14 쿨타임 시작
			m_CutDescList[11]->bRender = true;  //번호나타남
			m_CutDescList[17]->bRender = false; //연기 사라짐

			m_CutDescList[47]->bRender = true;
			m_CutDescList[48]->bRender = true;
			m_CutDescList[65]->bRender = true;

		}
		if ((eKeyType::RED == m_iPrePlayer) && (2 < m_HavePlayerNum)) // 3번에서 1번 플레이어 선택
		{
			RenderCurrentPlayer1();
			m_CutDescList[15]->bRender = true;
			m_CutDescList[15]->bCoolTime = true; //15 쿨타임 시작 
			m_CutDescList[12]->bRender = true;  // 번호 나타남
			m_CutDescList[18]->bRender = false; // 연기 사라짐

			m_CutDescList[49]->bRender = true;
			m_CutDescList[50]->bRender = true;
			m_CutDescList[64]->bRender = true;
		}

	}
}

void CUI_MainScreen::ChoicePlayer2()
{
	if ((nullptr != m_CutDescList[13]) && (false == m_CutDescList[13]->bCoolTime) && (false == m_CutDescList[14]->bCoolTime) && (false == m_CutDescList[15]->bCoolTime))
	{

		m_iPrePlayer = m_iCurrentPlayer;
		m_iCurrentPlayer = eKeyType::YANGYANG ;
		m_CutDescList[8]->Cool = m_CutDescList[55]->Cool;
		m_CutDescList[57]->iTexNum = m_CutDescList[59]->iTexNum;
		m_CutDescList[36]->iTexNum = 201;
		m_CutDescList[55]->fColorRCut = m_CutDescList[8]->fColorRCut = m_CutDescList[70]->fColorRCut = m_CutDescList[17]->fColorRCut;
		m_CutDescList[55]->fColorGCut = m_CutDescList[8]->fColorGCut = m_CutDescList[70]->fColorGCut = m_CutDescList[17]->fColorGCut;
		m_CutDescList[55]->fColorBCut = m_CutDescList[8]->fColorBCut = m_CutDescList[70]->fColorBCut = m_CutDescList[17]->fColorBCut;


		if ((eKeyType::DANSUN == m_iPrePlayer) && (1 < m_HavePlayerNum)) // 1번에서 2번 플레이어 선택
		{
			RenderCurrentPlayer2();
			m_CutDescList[13]->bRender = true;
			m_CutDescList[13]->bCoolTime = true; // 13 쿨타임 시작
			m_CutDescList[10]->bRender = true; // 번호 나타남
			m_CutDescList[16]->bRender = false; //연기 사라짐

			m_CutDescList[45]->bRender = true;
			m_CutDescList[46]->bRender = true;
			m_CutDescList[66]->bRender = true;
		}
		if ((eKeyType::RED == m_iPrePlayer) && (2 < m_HavePlayerNum)) // 3번에서 2번 플레이어 선택
		{
			RenderCurrentPlayer2();
			m_CutDescList[15]->bRender = true;
			m_CutDescList[15]->bCoolTime = true;
			m_CutDescList[12]->bRender = true;  // 번호 나타남
			m_CutDescList[18]->bRender = false; // 연기 사라짐

			m_CutDescList[49]->bRender = true;
			m_CutDescList[50]->bRender = true;
			m_CutDescList[64]->bRender = true;
		}
	}
}
void CUI_MainScreen::ChoicePlayer3()
{
	if ((nullptr != m_CutDescList[13]) && (false == m_CutDescList[13]->bCoolTime) && (false == m_CutDescList[14]->bCoolTime) && (false == m_CutDescList[15]->bCoolTime))
	{
		if (2 < m_HavePlayerNum)
		{
			m_iPrePlayer = m_iCurrentPlayer;
			m_iCurrentPlayer = eKeyType::RED;
			m_CutDescList[8]->Cool = m_CutDescList[56]->Cool;
			m_CutDescList[57]->iTexNum = m_CutDescList[60]->iTexNum;
			m_CutDescList[36]->iTexNum = 203;
			m_CutDescList[56]->fColorRCut = m_CutDescList[8]->fColorRCut = m_CutDescList[70]->fColorRCut = m_CutDescList[18]->fColorRCut;
			m_CutDescList[56]->fColorGCut = m_CutDescList[8]->fColorGCut = m_CutDescList[70]->fColorGCut = m_CutDescList[18]->fColorGCut;
			m_CutDescList[56]->fColorBCut = m_CutDescList[8]->fColorBCut = m_CutDescList[70]->fColorBCut = m_CutDescList[18]->fColorBCut;


			if (eKeyType::DANSUN == m_iPrePlayer) // 1번에서 3번 플레이어 선택
			{
				RenderCurrentPlayer3();
				m_CutDescList[13]->bRender = true;
				m_CutDescList[13]->bCoolTime = true;
				m_CutDescList[10]->bRender = true;  // 번호 나타남
				m_CutDescList[16]->bRender = false; //연기 사라짐


				m_CutDescList[45]->bRender = true;
				m_CutDescList[46]->bRender = true;
				m_CutDescList[66]->bRender = true;
			}

			if (eKeyType::YANGYANG  == m_iPrePlayer) // 2번에서 3번 플레이어 선택
			{
				RenderCurrentPlayer3();
				m_CutDescList[14]->bRender = true;
				m_CutDescList[14]->bCoolTime = true;
				m_CutDescList[11]->bRender = true;  //번호 나타남
				m_CutDescList[17]->bRender = false; // 연기 사라짐

				m_CutDescList[47]->bRender = true;
				m_CutDescList[48]->bRender = true;
				m_CutDescList[65]->bRender = true;
			}
		}
	}
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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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
