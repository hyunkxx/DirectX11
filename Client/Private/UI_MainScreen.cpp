#include "stdafx.h"
#include "..\Public\UI_MainScreen.h"
#include "GameInstance.h"
#include "RenderTarget.h"
#include "UI_Mouse.h"
#include "Character.h"
#include "Terrain.h"
#include "UI_Terminal.h"

CUI_MainScreen::CUI_MainScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
}

CUI_MainScreen::CUI_MainScreen(const CUI_MainScreen& rhs)
	: CGameObject(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_MainMaskNum(rhs.m_MainMaskNum)
	, m_MainMaskNum2(rhs.m_MainMaskNum2)
	, m_Mini0MaskNum(rhs.m_Mini0MaskNum)
	, m_Mini0MaskNum2(rhs.m_Mini0MaskNum2)
	, m_Mini1MaskNum(rhs.m_Mini1MaskNum)
	, m_Mini1MaskNum2(rhs.m_Mini1MaskNum2)
	, m_Mini2MaskNum(rhs.m_Mini2MaskNum)
	, m_Mini2MaskNum2(rhs.m_Mini2MaskNum2)

{
	for (auto& Buffer : rhs.m_BufferCutList)
	{
		m_BufferCutList.push_back(Buffer);
		Safe_AddRef(Buffer);
	}
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

	
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

 	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMINIMAP, TEXT("layer_UI"), TEXT("UI_Minimap"))))
		return E_FAIL;

	Add_PlayerNum();
	--m_HadPlayerNum;

	return S_OK;
}

void CUI_MainScreen::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->InputKey(DIK_1) == KEY_STATE::TAP)
	{
		ChoicePlayer1();
	}
	if (pGameInstance->InputKey(DIK_2) == KEY_STATE::TAP)
	{
		ChoicePlayer2();
	}
	if (pGameInstance->InputKey(DIK_3) == KEY_STATE::TAP)
	{
		ChoicePlayer3();
	}
	if (pGameInstance->InputKey(DIK_4) == KEY_STATE::TAP)
	{
		Add_PlayerNum();
	}
	

	//마우스커서
	SerectUI();
	for (auto& Desc : m_CutDescList)
	{
		if (true == Desc->bAlphaM)
			AlphaM(Desc, TimeDelta);
		if (true == Desc->bAlphaP)
			AlphaP(Desc, TimeDelta);
		if (true == Desc->bCoolTime)
		{
			if (((eKeyType)(Desc->eKeyType) == eKeyType::YANGYANG)
				|| ((eKeyType)(Desc->eKeyType) == eKeyType::DANSUN)
				|| ((eKeyType)(Desc->eKeyType) == eKeyType::RED))
			{
				CountDownCool(TimeDelta);

				if (CoolTime(Desc, TimeDelta))
				{
					Coolend = true;
					Time = 1.0f;
				}
			}
		}
		if (true == Coolend)
		{
			CoolTimeEnd(TimeDelta);
		}
	}

	UVWave(TimeDelta);
	if (true == m_bHit)
	{
		HPBar(TimeDelta);
	}
	if (true == m_bRedStart)
	{
		HPRedBar(TimeDelta);
	}

	T(TimeDelta);
	Q(TimeDelta);
	E(TimeDelta);
	R(TimeDelta);
	switch (m_iCurrentPlayer)
	{
	case Client::CUI_MainScreen::eKeyType::YANGYANG:
	{

		m_CutDescList[74]->Cool = TCoolRadian[0];
		m_CutDescList[33]->Cool = ECoolRadian[0];
		m_CutDescList[34]->Cool = QCoolRadian[0];
		m_CutDescList[35]->Cool = RCoolRadian[0];
		m_CutDescList[36]->Cool = RRRadian[0];
		m_CutDescList[8]->Cool = IconRadian[0];

		if (false == UsedTSkill[0])
		{
			if ((true == UsedTSkill[1]) || (true == UsedTSkill[2]))
			{
				TCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_T) == KEY_STATE::TAP) && (UsedTSkill[0] == false))
		{
			TCoolRender();
			UsedTSkill[0] = true;
			m_CutDescList[71]->iTexNum = 195;
			m_CutDescList[72]->iTexNum = 190;
		}
		if (0.f > TCoolTime[0]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			TCoolRenderOff();
			UsedTSkill[0] = false;
			TCoolRadian[0] = 0.f;
			TTimeAcc[0] = 0.f;
			TCoolTime[0] = StaticTSkillTime;
		}

		if (UsedTSkill[0] == true)
		{
			if (m_CutDescList[71]->iTexNum == 189)
			{
				m_CutDescList[71]->iTexNum = 190;
			}
			if ((m_CutDescList[72]->iTexNum == 189) && (0.f < TCoolTime[0]))
			{
				m_CutDescList[72]->iTexNum = 199;
			}
			if (TCoolTime[0] > StaticTSkillTime - TTimeAcc[0])
			{
				TCoolTime[0] -= 0.1f;
				m_CutDescList[72]->iTexNum -= 1;
				m_CutDescList[71]->iTexNum = 190 + (_int)TCoolTime[0];
			}
		}

		// E
		if (false == UsedESkill[0])
		{
			if ((true == UsedESkill[1]) || (true == UsedESkill[2]))
			{
				ECoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_E) == KEY_STATE::TAP) && (UsedESkill[0] == false))
		{
			ECoolRender();
			UsedESkill[0] = true;
			m_CutDescList[39]->iTexNum = 195;
			m_CutDescList[40]->iTexNum = 190;
		}
		if (0.f > ECoolTime[0]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			ECoolRenderOff();
			UsedESkill[0] = false;
			ECoolRadian[0] = 0.f;
			ETimeAcc[0] = 0.f;
			ECoolTime[0] = StaticESkillTime;
		}

		if (UsedESkill[0] == true)
		{
			if (m_CutDescList[39]->iTexNum == 189)
			{
				m_CutDescList[40]->iTexNum = 190;
			}
			if ((m_CutDescList[40]->iTexNum == 189) && (0.f < ECoolTime[0]))
			{
				m_CutDescList[40]->iTexNum = 199;
			}
			if (ECoolTime[0] > StaticESkillTime - ETimeAcc[0])
			{
				ECoolTime[0] -= 0.1f;
				m_CutDescList[40]->iTexNum -= 1;
				m_CutDescList[39]->iTexNum = 190 + (_int)ECoolTime[0];
			}
		}

		//Q
		if (false == UsedQSkill[0])
		{
			if ((true == UsedQSkill[1]) || (true == UsedQSkill[2]))
			{
				QCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_Q) == KEY_STATE::TAP) && (UsedQSkill[0] == false))
		{
			QCoolRender();
			UsedQSkill[0] = true;
			m_CutDescList[41]->iTexNum = 195;
			m_CutDescList[42]->iTexNum = 190;
		}
		if (0.f > QCoolTime[0]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			QCoolRenderOff();
			UsedQSkill[0] = false;
			QCoolRadian[0] = 0.f;
			QTimeAcc[0] = 0.f;
			QCoolTime[0] = StaticQSkillTime;
		}

		if (UsedQSkill[0] == true)
		{
			if (m_CutDescList[41]->iTexNum == 189)
			{
				m_CutDescList[41]->iTexNum = 190;
			}
			if ((m_CutDescList[42]->iTexNum == 189) && (0.f < QCoolTime[0]))
			{
				m_CutDescList[42]->iTexNum = 199;
			}
			if (QCoolTime[0] > StaticQSkillTime - QTimeAcc[0])
			{
				QCoolTime[0] -= 0.1f;
				m_CutDescList[42]->iTexNum -= 1;
				m_CutDescList[41]->iTexNum = 190 + (_int)QCoolTime[0];
			}
		}

		//
		if (false == UsedRSkill[0])
		{
			if ((true == UsedRSkill[1]) || (true == UsedRSkill[2]))
			{
				RCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_R) == KEY_STATE::TAP) && (UsedRSkill[0] == false) && (1.f == RRRadian[0]))
		{
			RCoolRender();
			UsedRSkill[0] = true;
			m_CutDescList[43]->iTexNum = 195;
			m_CutDescList[44]->iTexNum = 190;
			RRRadian[0] = 0.f;
		}
		if (0.f > RCoolTime[0]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			RCoolRenderOff();
			UsedRSkill[0] = false;
			RCoolRadian[0] = 0.f;
			RTimeAcc[0] = 0.f;
			RCoolTime[0] = StaticRSkillTime;
		}

		if (UsedRSkill[0] == true)
		{
			if (m_CutDescList[43]->iTexNum == 189)
			{
				m_CutDescList[43]->iTexNum = 190;
			}
			if ((m_CutDescList[44]->iTexNum == 189) && (0.f < RCoolTime[0]))
			{
				m_CutDescList[44]->iTexNum = 199;
			}
			if (RCoolTime[0] > StaticRSkillTime - RTimeAcc[0])
			{
				RCoolTime[0] -= 0.1f;
				m_CutDescList[44]->iTexNum -= 1;
				m_CutDescList[43]->iTexNum = 190 + (_int)RCoolTime[0];
			}
		}

		if ((pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP) && (UsedRSkill[0] == false))
		{
			RRRadian[0] += (_float)TimeDelta;
			if (1.f <= RRRadian[0])
				RRRadian[0] = 1.f;

			IconRadian[0] += (_float)TimeDelta * 1.2f;
			if (1.f <= IconRadian[0])
				IconRadian[0] = 1.f;
		}



	}
	break;

	case Client::CUI_MainScreen::eKeyType::DANSUN:
	{
		m_CutDescList[74]->Cool = TCoolRadian[1];
		m_CutDescList[33]->Cool = ECoolRadian[1];
		m_CutDescList[34]->Cool = QCoolRadian[1];
		m_CutDescList[35]->Cool = RCoolRadian[1];
		m_CutDescList[36]->Cool = RRRadian[1];
		m_CutDescList[8]->Cool = IconRadian[1];
		if (false == UsedTSkill[1])
		{
			if ((true == UsedTSkill[0]) || (true == UsedTSkill[2]))
			{
				TCoolRenderOff();
			}
		}


		if ((pGameInstance->InputKey(DIK_T) == KEY_STATE::TAP) && (UsedTSkill[1] == false))
		{
			TCoolRender();
			UsedTSkill[1] = true;
			m_CutDescList[71]->iTexNum = 195;
			m_CutDescList[72]->iTexNum = 190;
		}
		if (0.f > TCoolTime[1]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			TCoolRenderOff();
			UsedTSkill[1] = false;
			TCoolRadian[1] = 0.f;
			TTimeAcc[1] = 0.f;
			TCoolTime[1] = StaticTSkillTime;
		}

		if (UsedTSkill[1] == true)
		{
			if (m_CutDescList[71]->iTexNum == 189)
			{
				m_CutDescList[71]->iTexNum = 190;
			}
			if ((m_CutDescList[72]->iTexNum == 189) && (0.f < TCoolTime[1]))
			{
				m_CutDescList[72]->iTexNum = 199;
			}
			if (TCoolTime[1] > StaticTSkillTime - TTimeAcc[1])
			{
				TCoolTime[1] -= 0.1f;
				m_CutDescList[72]->iTexNum -= 1;
				m_CutDescList[71]->iTexNum = 190 + (_int)TCoolTime[1];
			}
		}

		//E
		if (false == UsedESkill[1])
		{
			if ((true == UsedESkill[0]) || (true == UsedESkill[2]))
			{
				ECoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_E) == KEY_STATE::TAP) && (UsedESkill[1] == false))
		{
			ECoolRender();
			UsedESkill[1] = true;
			m_CutDescList[39]->iTexNum = 195;
			m_CutDescList[40]->iTexNum = 190;
		}
		if (0.f > ECoolTime[1]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			ECoolRenderOff();
			UsedESkill[1] = false;
			ECoolRadian[1] = 0.f;
			ETimeAcc[1] = 0.f;
			ECoolTime[1] = StaticESkillTime;
		}

		if (UsedESkill[1] == true)
		{
			if (m_CutDescList[39]->iTexNum == 189)
			{
				m_CutDescList[40]->iTexNum = 190;
			}
			if ((m_CutDescList[40]->iTexNum == 189) && (0.f < ECoolTime[1]))
			{
				m_CutDescList[40]->iTexNum = 199;
			}
			if (ECoolTime[1] > StaticESkillTime - ETimeAcc[1])
			{
				ECoolTime[1] -= 0.1f;
				m_CutDescList[40]->iTexNum -= 1;
				m_CutDescList[39]->iTexNum = 190 + (_int)ECoolTime[1];
			}
		}

		//Q
		if (false == UsedQSkill[1])
		{
			if ((true == UsedQSkill[0]) || (true == UsedQSkill[2]))
			{
				QCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_Q) == KEY_STATE::TAP) && (UsedQSkill[1] == false))
		{
			QCoolRender();
			UsedQSkill[1] = true;
			m_CutDescList[41]->iTexNum = 195;
			m_CutDescList[42]->iTexNum = 190;
		}
		if (0.f > QCoolTime[1]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			QCoolRenderOff();
			UsedQSkill[1] = false;
			QCoolRadian[1] = 0.f;
			QTimeAcc[1] = 0.f;
			QCoolTime[1] = StaticQSkillTime;
		}

		if (UsedQSkill[1] == true)
		{
			if (m_CutDescList[41]->iTexNum == 189)
			{
				m_CutDescList[41]->iTexNum = 190;
			}
			if ((m_CutDescList[42]->iTexNum == 189) && (0.f < QCoolTime[1]))
			{
				m_CutDescList[42]->iTexNum = 199;
			}
			if (QCoolTime[1] > StaticQSkillTime - QTimeAcc[1])
			{
				QCoolTime[1] -= 0.1f;
				m_CutDescList[42]->iTexNum -= 1;
				m_CutDescList[41]->iTexNum = 190 + (_int)QCoolTime[1];
			}
		}

		//
		if (false == UsedRSkill[1])
		{
			if ((true == UsedRSkill[0]) || (true == UsedRSkill[2]))
			{
				RCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_R) == KEY_STATE::TAP) && (UsedRSkill[1] == false) && (1.f == RRRadian[1]))
		{
			RCoolRender();
			UsedRSkill[1] = true;
			m_CutDescList[43]->iTexNum = 195;
			m_CutDescList[44]->iTexNum = 190;
		}
		if (0.f > RCoolTime[1]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			RCoolRenderOff();
			UsedRSkill[1] = false;
			RCoolRadian[1] = 0.f;
			RTimeAcc[1] = 0.f;
			RCoolTime[1] = StaticRSkillTime;
			RRRadian[1] = 0.f;
		}

		if (UsedRSkill[1] == true)
		{
			if (m_CutDescList[43]->iTexNum == 189)
			{
				m_CutDescList[43]->iTexNum = 190;
			}
			if ((m_CutDescList[44]->iTexNum == 189) && (0.f < RCoolTime[1]))
			{
				m_CutDescList[44]->iTexNum = 199;
			}
			if (RCoolTime[1] > StaticRSkillTime - RTimeAcc[1])
			{
				RCoolTime[1] -= 0.1f;
				m_CutDescList[44]->iTexNum -= 1;
				m_CutDescList[43]->iTexNum = 190 + (_int)RCoolTime[1];
			}
		}

		if ((pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP) && (UsedRSkill[1] == false))
		{
			RRRadian[1] += (_float)TimeDelta;
			if (1.f <= RRRadian[1])
				RRRadian[1] = 1.f;

			IconRadian[1] += (_float)TimeDelta *1.2f;
			if (1.f <= IconRadian[1])
				IconRadian[1] = 1.f;
		}

	}
	break;

	case Client::CUI_MainScreen::eKeyType::RED:
	{
		m_CutDescList[74]->Cool = TCoolRadian[2];
		m_CutDescList[33]->Cool = ECoolRadian[2];
		m_CutDescList[34]->Cool = QCoolRadian[2];
		m_CutDescList[35]->Cool = RCoolRadian[2];
		m_CutDescList[36]->Cool = RRRadian[2];
		m_CutDescList[8]->Cool = IconRadian[1];

		if (false == UsedTSkill[2])
		{

			if ((true == UsedTSkill[0]) || (true == UsedTSkill[1]))
			{
				TCoolRenderOff();
			}
		}


		if ((pGameInstance->InputKey(DIK_T) == KEY_STATE::TAP) && (UsedTSkill[2] == false))
		{
			TCoolRender();
			UsedTSkill[2] = true;
			m_CutDescList[71]->iTexNum = 195;
			m_CutDescList[72]->iTexNum = 190;
		}
		if (0.f > TCoolTime[2]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			TCoolRenderOff();
			UsedTSkill[2] = false;
			TCoolRadian[2] = 0.f;
			TTimeAcc[2] = 0.f;
			TCoolTime[2] = StaticTSkillTime;
		}

		if (UsedTSkill[2] == true)
		{
			if (m_CutDescList[71]->iTexNum == 189)
			{
				m_CutDescList[71]->iTexNum = 190;
			}
			if ((m_CutDescList[72]->iTexNum == 189) && (0.f < TCoolTime[2]))
			{
				m_CutDescList[72]->iTexNum = 199;
			}
			if (TCoolTime[2] > StaticTSkillTime - TTimeAcc[2])
			{
				TCoolTime[2] -= 0.1f;
				m_CutDescList[72]->iTexNum -= 1;
				m_CutDescList[71]->iTexNum = 190 + (_int)TCoolTime[2];
			}
		}

		//E
		if (false == UsedESkill[2])
		{
			if ((true == UsedESkill[0]) || (true == UsedESkill[1]))
			{
				ECoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_E) == KEY_STATE::TAP) && (UsedESkill[2] == false))
		{
			ECoolRender();
			UsedESkill[2] = true;
			m_CutDescList[39]->iTexNum = 195;
			m_CutDescList[40]->iTexNum = 190;
		}
		if (0.f > ECoolTime[2]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			ECoolRenderOff();
			UsedESkill[2] = false;
			ECoolRadian[2] = 0.f;
			ETimeAcc[2] = 0.f;
			ECoolTime[2] = StaticESkillTime;
		}

		if (UsedESkill[2] == true)
		{
			if (m_CutDescList[39]->iTexNum == 189)
			{
				m_CutDescList[40]->iTexNum = 190;
			}
			if ((m_CutDescList[40]->iTexNum == 189) && (0.f < ECoolTime[2]))
			{
				m_CutDescList[40]->iTexNum = 199;
			}
			if (ECoolTime[2] > StaticESkillTime - ETimeAcc[2])
			{
				ECoolTime[2] -= 0.1f;
				m_CutDescList[40]->iTexNum -= 1;
				m_CutDescList[39]->iTexNum = 190 + (_int)ECoolTime[2];
			}
		}


		//
		if (false == UsedQSkill[2])
		{
			if ((true == UsedQSkill[0]) || (true == UsedQSkill[1]))
			{
				QCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_Q) == KEY_STATE::TAP) && (UsedQSkill[2] == false))
		{
			QCoolRender();
			UsedQSkill[2] = true;
			m_CutDescList[41]->iTexNum = 195;
			m_CutDescList[42]->iTexNum = 190;
		}
		if (0.f > QCoolTime[2]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			QCoolRenderOff();
			UsedQSkill[2] = false;
			QCoolRadian[2] = 0.f;
			QTimeAcc[2] = 0.f;
			QCoolTime[2] = StaticQSkillTime;
		}

		if (UsedQSkill[2] == true)
		{
			if (m_CutDescList[41]->iTexNum == 189)
			{
				m_CutDescList[41]->iTexNum = 190;
			}
			if ((m_CutDescList[42]->iTexNum == 189) && (0.f < QCoolTime[2]))
			{
				m_CutDescList[42]->iTexNum = 199;
			}
			if (QCoolTime[2] > StaticQSkillTime - QTimeAcc[2])
			{
				QCoolTime[2] -= 0.1f;
				m_CutDescList[42]->iTexNum -= 1;
				m_CutDescList[41]->iTexNum = 190 + (_int)QCoolTime[2];
			}
		}

		//
		if (false == UsedRSkill[2])
		{
			if ((true == UsedRSkill[0]) || (true == UsedRSkill[1]))
			{
				RCoolRenderOff();
			}
		}

		if ((pGameInstance->InputKey(DIK_R) == KEY_STATE::TAP) && (UsedRSkill[2] == false) && (1.f == RRRadian[2]))
		{
			RCoolRender();
			UsedRSkill[2] = true;
			m_CutDescList[43]->iTexNum = 195;
			m_CutDescList[44]->iTexNum = 190;
		}
		if (0.f > RCoolTime[2]) // 현재 플레이어가 쓴 쿨타임이 끝났다면
		{
			RCoolRenderOff();
			UsedRSkill[2] = false;
			RCoolRadian[2] = 0.f;
			RTimeAcc[2] = 0.f;
			RCoolTime[2] = StaticRSkillTime;
		}

		if (UsedRSkill[2] == true)
		{
			if (m_CutDescList[43]->iTexNum == 189)
			{
				m_CutDescList[43]->iTexNum = 190;
			}
			if ((m_CutDescList[44]->iTexNum == 189) && (0.f < RCoolTime[2]))
			{
				m_CutDescList[44]->iTexNum = 199;
			}
			if (RCoolTime[2] > StaticRSkillTime - RTimeAcc[2])
			{
				RCoolTime[2] -= 0.1f;
				m_CutDescList[44]->iTexNum -= 1;
				m_CutDescList[43]->iTexNum = 190 + (_int)RCoolTime[2];
			}
		}

		if ((pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP) && (UsedRSkill[2] == false))
		{
			RRRadian[2] += (_float)TimeDelta;
			if (1.f <= RRRadian[2])
				RRRadian[2] = 1.f;

			IconRadian[2] += (_float)TimeDelta *1.2f;
			if (1.f <= IconRadian[2])
				IconRadian[2] = 1.f;
		}


	}
	break;

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
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (_uint i = 0; i<(_uint)m_CutDescList.size(); ++i)
	{
		if (true == m_CutDescList[i]->bRender)
		{
			if (eKeyType::T == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesT(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::Q == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesQ(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::E == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesE(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::R == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesR(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::I0 == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesI0(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::I1 == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesI1(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::I2 == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesI2(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}

			else if (eKeyType::RR == (eKeyType)m_CutDescList[i]->eKeyType)
			{

				if (FAILED(Setup_ShaderResourcesRR(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::GRAPH0 == (eKeyType)m_CutDescList[i]->eKeyType)
			{
				if (FAILED(Setup_ShaderResourcesGraphs0(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::GRAPH1 == (eKeyType)m_CutDescList[i]->eKeyType)
			{
				if (FAILED(Setup_ShaderResourcesGraphs1(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if (eKeyType::GRAPH2 == (eKeyType)m_CutDescList[i]->eKeyType)
			{
				if (FAILED(Setup_ShaderResourcesGraphs2(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else if ((true == m_CutDescList[13]->bCoolTime) || (true == m_CutDescList[14]->bCoolTime) || (true == m_CutDescList[15]->bCoolTime))
			{
				if (FAILED(Setup_ShaderResourcesPlayer(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
			else
			{
				if (FAILED(Setup_ShaderResourcesCut(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_BufferCutList[i]->Render();
			}
		}


	}

	/*
	if (true == m_bUIRender)
	{
	if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_Sprite", 248)))
	return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_CurrentCount", &(m_fCurrentSpriteIndex), sizeof(_float))))
	return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_SpriteXY", &(m_SpriteSize), sizeof(_float2))))
	return E_FAIL;
	m_Sprite.WorldMatrix = m_CutDescList[0]->WorldMatrixCut;

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_Sprite.WorldMatrix))))
	return E_FAIL;
	m_pShader->Begin(15);
	m_pVIBufferCut->Render();
	}*/

	return S_OK;
}

void CUI_MainScreen::RenderGUI()
{
	//ImGui::Begin("MainScreen ID");
	//ImGui::InputInt("ID", &m_iObjectID);
	//ImGui::End();
		
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

	if (m_iPrePlayer == eKeyType::YANGYANG)
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
	if (m_iPrePlayer == eKeyType::DANSUN)
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
	if (m_iPrePlayer == eKeyType::YANGYANG)
	{
		pDesc = m_CutDescList[0];

	}
	if (m_iPrePlayer == eKeyType::DANSUN)
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
		case Client::CUI_MainScreen::eKeyType::YANGYANG:
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
		case Client::CUI_MainScreen::eKeyType::DANSUN:
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

void CUI_MainScreen::T(_double TimeDelta)
{


	if (true == UsedTSkill[0])
	{
		TTimeAcc[0] += (_float)TimeDelta;
		TCoolRadian[0] += (_float)TimeDelta / StaticTSkillTime;
	}
	if ((true == UsedTSkill[0]) && (TCoolTime[0] >= StaticTSkillTime - TTimeAcc[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		TCoolTime[0] -= 0.1f;
	}
	if ((0.f > TCoolTime[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		UsedTSkill[0] = false;
		TCoolRadian[0] = 0.f;
		TTimeAcc[0] = 0.f;
		TCoolTime[0] = StaticTSkillTime;
	}

	//
	if (true == UsedTSkill[1])
	{
		TTimeAcc[1] += (_float)TimeDelta;
		TCoolRadian[1] += (_float)TimeDelta / StaticTSkillTime;
	}
	if ((true == UsedTSkill[1]) && (TCoolTime[1] >= StaticTSkillTime - TTimeAcc[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		TCoolTime[1] -= 0.1f;
	}
	if ((0.f > TCoolTime[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		UsedTSkill[1] = false;
		TCoolRadian[1] = 0.f;
		TTimeAcc[1] = 0.f;
		TCoolTime[1] = StaticTSkillTime;
	}

	//
	if (true == UsedTSkill[2])
	{
		TTimeAcc[2] += (_float)TimeDelta;
		TCoolRadian[2] += (_float)TimeDelta / StaticTSkillTime;
	}
	if ((true == UsedTSkill[2]) && (TCoolTime[2] >= StaticTSkillTime - TTimeAcc[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		TCoolTime[2] -= 0.1f;
	}
	if ((0.f > TCoolTime[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		UsedTSkill[2] = false;
		TCoolRadian[2] = 0.f;
		TTimeAcc[2] = 0.f;
		TCoolTime[2] = StaticTSkillTime;
	}

}

void CUI_MainScreen::Q(_double TimeDelta)
{


	if (true == UsedQSkill[0])
	{
		QTimeAcc[0] += (_float)TimeDelta;
		QCoolRadian[0] += (_float)TimeDelta / StaticQSkillTime;
	}
	if ((true == UsedQSkill[0]) && (QCoolTime[0] >= StaticQSkillTime - QTimeAcc[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		QCoolTime[0] -= 0.1f;
	}
	if ((0.f > QCoolTime[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		UsedQSkill[0] = false;
		QCoolRadian[0] = 0.f;
		QTimeAcc[0] = 0.f;
		QCoolTime[0] = StaticQSkillTime;
	}

	//
	if (true == UsedQSkill[1])
	{
		QTimeAcc[1] += (_float)TimeDelta;
		QCoolRadian[1] += (_float)TimeDelta / StaticQSkillTime;
	}
	if ((true == UsedQSkill[1]) && (QCoolTime[1] >= StaticQSkillTime - QTimeAcc[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		QCoolTime[1] -= 0.1f;
	}
	if ((0.f > QCoolTime[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		UsedQSkill[1] = false;
		QCoolRadian[1] = 0.f;
		QTimeAcc[1] = 0.f;
		QCoolTime[1] = StaticQSkillTime;
	}

	//
	if (true == UsedQSkill[2])
	{
		QTimeAcc[2] += (_float)TimeDelta;
		QCoolRadian[2] += (_float)TimeDelta / StaticQSkillTime;
	}
	if ((true == UsedQSkill[2]) && (QCoolTime[2] >= StaticQSkillTime - QTimeAcc[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		QCoolTime[2] -= 0.1f;
	}
	if ((0.f > QCoolTime[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		UsedQSkill[2] = false;
		QCoolRadian[2] = 0.f;
		QTimeAcc[2] = 0.f;
		QCoolTime[2] = StaticQSkillTime;
	}

}

void CUI_MainScreen::E(_double TimeDelta)
{


	if (true == UsedESkill[0])
	{
		ETimeAcc[0] += (_float)TimeDelta;
		ECoolRadian[0] += (_float)TimeDelta / StaticESkillTime;
	}
	if ((true == UsedESkill[0]) && (ECoolTime[0] >= StaticESkillTime - ETimeAcc[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		ECoolTime[0] -= 0.1f;
	}
	if ((0.f > ECoolTime[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		UsedESkill[0] = false;
		ECoolRadian[0] = 0.f;
		ETimeAcc[0] = 0.f;
		ECoolTime[0] = StaticESkillTime;
	}

	//
	if (true == UsedESkill[1])
	{
		ETimeAcc[1] += (_float)TimeDelta;
		ECoolRadian[1] += (_float)TimeDelta / StaticESkillTime;
	}
	if ((true == UsedESkill[1]) && (ECoolTime[1] >= StaticESkillTime - ETimeAcc[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		ECoolTime[1] -= 0.1f;
	}
	if ((0.f > ECoolTime[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		UsedESkill[1] = false;
		ECoolRadian[1] = 0.f;
		ETimeAcc[1] = 0.f;
		ECoolTime[1] = StaticESkillTime;
	}

	//
	if (true == UsedESkill[2])
	{
		ETimeAcc[2] += (_float)TimeDelta;
		ECoolRadian[2] += (_float)TimeDelta / StaticESkillTime;
	}
	if ((true == UsedESkill[2]) && (ECoolTime[2] >= StaticESkillTime - ETimeAcc[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		ECoolTime[2] -= 0.1f;
	}
	if ((0.f > ECoolTime[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		UsedESkill[2] = false;
		ECoolRadian[2] = 0.f;
		ETimeAcc[2] = 0.f;
		ECoolTime[2] = StaticESkillTime;
	}

}

void CUI_MainScreen::R(_double TimeDelta)
{


	if (true == UsedRSkill[0])
	{
		RTimeAcc[0] += (_float)TimeDelta;
		RCoolRadian[0] += (_float)TimeDelta / StaticRSkillTime;
	}
	if ((true == UsedRSkill[0]) && (RCoolTime[0] >= StaticRSkillTime - RTimeAcc[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		RCoolTime[0] -= 0.1f;
	}
	if ((0.f > RCoolTime[0]) && (m_iCurrentPlayer != eKeyType::YANGYANG))
	{
		UsedRSkill[0] = false;
		RCoolRadian[0] = 0.f;
		RTimeAcc[0] = 0.f;
		RCoolTime[0] = StaticRSkillTime;
	}

	//
	if (true == UsedRSkill[1])
	{
		RTimeAcc[1] += (_float)TimeDelta;
		RCoolRadian[1] += (_float)TimeDelta / StaticRSkillTime;
	}
	if ((true == UsedRSkill[1]) && (RCoolTime[1] >= StaticRSkillTime - RTimeAcc[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		RCoolTime[1] -= 0.1f;
	}
	if ((0.f > RCoolTime[1]) && (m_iCurrentPlayer != eKeyType::DANSUN))
	{
		UsedRSkill[1] = false;
		RCoolRadian[1] = 0.f;
		RTimeAcc[1] = 0.f;
		RCoolTime[1] = StaticRSkillTime;
	}

	//
	if (true == UsedRSkill[2])
	{
		RTimeAcc[2] += (_float)TimeDelta;
		RCoolRadian[2] += (_float)TimeDelta / StaticRSkillTime;
	}
	if ((true == UsedRSkill[2]) && (RCoolTime[2] >= StaticRSkillTime - RTimeAcc[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		RCoolTime[2] -= 0.1f;
	}
	if ((0.f > RCoolTime[2]) && (m_iCurrentPlayer != eKeyType::RED))
	{
		UsedRSkill[2] = false;
		RCoolRadian[2] = 0.f;
		RTimeAcc[2] = 0.f;
		RCoolTime[2] = StaticRSkillTime;
	}

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
	m_CutDescList[5]->iTexNum = 170;
	m_CutDescList[6]->iTexNum = 114;
	m_CutDescList[7]->iTexNum = 115;

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

			m_pVIBufferCut = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBufferCut)
				return;

			m_BufferCutList.push_back(m_pVIBufferCut);
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
{ //83->빨 82->흰
	//흰색 줄어듬
	if (0.f < m_fWhiteBar)
	{
		if (0 < m_Damage) // 힐
		{
			if (m_PreHp + m_Damage <m_CurrentHp) // 힐 받은만큼만 참
			{
				m_bHit = false;
				m_PreHp = m_RedDamageACC = m_CurrentHp;
			}
			m_CurrentHp += m_Damage * (_float)TimeDelta;
			m_fWhiteBar = m_CurrentHp / m_CutDescList[31]->fWidthCut;
		}
		else
		{
			m_CurrentHp += m_Damage;
			m_fWhiteBar = m_CurrentHp / m_CutDescList[31]->fWidthCut;
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
		m_fRedBar = m_RedDamageACC / m_CutDescList[32]->fWidthCut;
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
		m_iCurrentPlayer = eKeyType::YANGYANG;
		m_CutDescList[8]->Cool = m_CutDescList[54]->Cool; // 플레이어 재선택쿨
		m_CutDescList[57]->iTexNum = m_CutDescList[58]->iTexNum;
		m_CutDescList[36]->iTexNum = 201;
		m_CutDescList[54]->fColorRCut = m_CutDescList[8]->fColorRCut = m_CutDescList[70]->fColorRCut = m_CutDescList[16]->fColorRCut;
		m_CutDescList[54]->fColorGCut = m_CutDescList[8]->fColorGCut = m_CutDescList[70]->fColorGCut = m_CutDescList[16]->fColorGCut;
		m_CutDescList[54]->fColorBCut = m_CutDescList[8]->fColorBCut = m_CutDescList[70]->fColorBCut = m_CutDescList[16]->fColorBCut;


		if ((eKeyType::DANSUN == m_iPrePlayer) && (1 < m_HavePlayerNum)) // 2번에서 1번 플레이어 선택
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
		m_iCurrentPlayer = eKeyType::DANSUN;
		m_CutDescList[8]->Cool = m_CutDescList[55]->Cool;
		m_CutDescList[57]->iTexNum = m_CutDescList[59]->iTexNum;
		m_CutDescList[36]->iTexNum = 202;
		m_CutDescList[55]->fColorRCut = m_CutDescList[8]->fColorRCut = m_CutDescList[70]->fColorRCut = m_CutDescList[17]->fColorRCut;
		m_CutDescList[55]->fColorGCut = m_CutDescList[8]->fColorGCut = m_CutDescList[70]->fColorGCut = m_CutDescList[17]->fColorGCut;
		m_CutDescList[55]->fColorBCut = m_CutDescList[8]->fColorBCut = m_CutDescList[70]->fColorBCut = m_CutDescList[17]->fColorBCut;


		if ((eKeyType::YANGYANG == m_iPrePlayer) && (1 < m_HavePlayerNum)) // 1번에서 2번 플레이어 선택
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


			if (eKeyType::YANGYANG == m_iPrePlayer) // 1번에서 3번 플레이어 선택
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

			if (eKeyType::DANSUN == m_iPrePlayer) // 2번에서 3번 플레이어 선택
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
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_MainMaskNum)))
				return E_FAIL;
			if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_MainMaskNum2)))
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


		if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini0MaskNum)))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini0MaskNum2)))
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

HRESULT CUI_MainScreen::Setup_ShaderResourcesGraphs1(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini1MaskNum)))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini1MaskNum2)))
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

HRESULT CUI_MainScreen::Setup_ShaderResourcesGraphs2(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask", m_Mini2MaskNum)))
			return E_FAIL;
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_PMask2", m_Mini2MaskNum2)))
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

HRESULT CUI_MainScreen::Setup_ShaderResourcesT(_uint Bufferindex)
{
	if (nullptr != m_CutDescList[Bufferindex])
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CutDescList[Bufferindex]->iTexNum)))
			return E_FAIL;

		if (m_CutDescList[74]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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

		if (m_CutDescList[33]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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

		if (m_CutDescList[34]->bCoolTime)
		{
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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
			if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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
		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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


		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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


		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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


		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_CutDescList[Bufferindex]->Cool, sizeof(_float))))
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
	m_pVIBufferCut = nullptr;
	Safe_Release(m_pVIBufferCut);


	for (auto& Buffer : m_BufferCutList)
	{
		Safe_Release(Buffer);
	}
	m_BufferCutList.clear();

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
