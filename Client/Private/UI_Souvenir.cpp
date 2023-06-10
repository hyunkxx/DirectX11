#include "stdafx.h"
#include "..\Public\UI_Souvenir.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "Input_Device.h"
#include "UI_Mouse.h"
#include "UI_MainScreen.h"
#include "UI_Minimap.h"
#include "PlayerState.h"
#include "ItemDB.h"
#include "Item.h"
#include "Inventory.h"


CUI_Souvenir::CUI_Souvenir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Souvenir::CUI_Souvenir(const CUI_Souvenir& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Souvenir::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Souvenir::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	Load();
	// 위치이동 임시코드
	
	for (_int i=0; i< (_int)m_0Slot.size(); ++i)
	{
		m_1Slot[i].fX = m_0Slot[i].fX;
		m_1Slot[i].fY = m_0Slot[i].fY-82.f;
		m_1Slot[i].fZ = m_0Slot[i].fZ;
		m_1Slot[i].iPass = m_0Slot[i].iPass;
		m_1Slot[i].iTexNum = m_0Slot[i].iTexNum;
		m_1Slot[i].fHeight = m_0Slot[i].fHeight;
		m_1Slot[i].fWidth = m_0Slot[i].fWidth;
		m_1Slot[i].fColorR = m_0Slot[i].fColorR;
		m_1Slot[i].fColorG = m_0Slot[i].fColorG;
		m_1Slot[i].fColorB = m_0Slot[i].fColorB;
		m_1Slot[i].fColorA = m_0Slot[i].fColorA;

		XMStoreFloat4x4(&(m_1Slot[i].WorldMatrix), XMMatrixScaling(m_1Slot[i].fWidth, m_1Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_1Slot[i].fX, m_1Slot[i].fY, m_1Slot[i].fZ));
	}

	for (_int i = 0; i< 9; ++i)
	{
		m_2Slot[i].fX	   = m_1Slot[i].fX;
		m_2Slot[i].fY	   = m_1Slot[i].fY - 82.f;
		m_2Slot[i].fZ	   = m_1Slot[i].fZ;
		m_2Slot[i].iPass   = m_1Slot[i].iPass;
		m_2Slot[i].iTexNum = m_1Slot[i].iTexNum;
		m_2Slot[i].fHeight = m_1Slot[i].fHeight;
		m_2Slot[i].fWidth  = m_1Slot[i].fWidth;
		m_2Slot[i].fColorR = m_1Slot[i].fColorR;
		m_2Slot[i].fColorG = m_1Slot[i].fColorG;
		m_2Slot[i].fColorB = m_1Slot[i].fColorB;
		m_2Slot[i].fColorA = m_1Slot[i].fColorA;

		XMStoreFloat4x4(&(m_2Slot[i].WorldMatrix), XMMatrixScaling(m_2Slot[i].fWidth, m_2Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_2Slot[i].fX, m_2Slot[i].fY, m_2Slot[i].fZ));
	}

	for (_int i = 0; i< 9; ++i)
	{
		m_3Slot[i].fX	   = m_2Slot[i].fX;
		m_3Slot[i].fY	   = m_2Slot[i].fY - 82.f;
		m_3Slot[i].fZ	   = m_2Slot[i].fZ;
		m_3Slot[i].iPass   = m_2Slot[i].iPass;
		m_3Slot[i].iTexNum = m_2Slot[i].iTexNum;
		m_3Slot[i].fHeight = m_2Slot[i].fHeight;
		m_3Slot[i].fWidth  = m_2Slot[i].fWidth;
		m_3Slot[i].fColorR = m_2Slot[i].fColorR;
		m_3Slot[i].fColorG = m_2Slot[i].fColorG;
		m_3Slot[i].fColorB = m_2Slot[i].fColorB;
		m_3Slot[i].fColorA = m_2Slot[i].fColorA;

		XMStoreFloat4x4(&(m_3Slot[i].WorldMatrix), XMMatrixScaling(m_3Slot[i].fWidth, m_3Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_3Slot[i].fX, m_3Slot[i].fY, m_3Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_4Slot[i].fX      = m_3Slot[i].fX;
		m_4Slot[i].fY      = m_3Slot[i].fY - 82.f;
		m_4Slot[i].fZ	   = m_3Slot[i].fZ;
		m_4Slot[i].iPass   = m_3Slot[i].iPass;
		m_4Slot[i].iTexNum = m_3Slot[i].iTexNum;
		m_4Slot[i].fHeight = m_3Slot[i].fHeight;
		m_4Slot[i].fWidth  = m_3Slot[i].fWidth;
		m_4Slot[i].fColorR = m_3Slot[i].fColorR;
		m_4Slot[i].fColorG = m_3Slot[i].fColorG;
		m_4Slot[i].fColorB = m_3Slot[i].fColorB;
		m_4Slot[i].fColorA = m_3Slot[i].fColorA;

		XMStoreFloat4x4(&(m_4Slot[i].WorldMatrix), XMMatrixScaling(m_4Slot[i].fWidth, m_4Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_4Slot[i].fX, m_4Slot[i].fY, m_4Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_5Slot[i].fX      = m_4Slot[i].fX;
		m_5Slot[i].fY      = m_4Slot[i].fY - 82.f;
		m_5Slot[i].fZ      = m_4Slot[i].fZ;
		m_5Slot[i].iPass   = m_4Slot[i].iPass;
		m_5Slot[i].iTexNum = m_4Slot[i].iTexNum;
		m_5Slot[i].fHeight = m_4Slot[i].fHeight;
		m_5Slot[i].fWidth  = m_4Slot[i].fWidth;
		m_5Slot[i].fColorR = m_4Slot[i].fColorR;
		m_5Slot[i].fColorG = m_4Slot[i].fColorG;
		m_5Slot[i].fColorB = m_4Slot[i].fColorB;
		m_5Slot[i].fColorA = m_4Slot[i].fColorA;

		XMStoreFloat4x4(&(m_5Slot[i].WorldMatrix), XMMatrixScaling(m_5Slot[i].fWidth, m_5Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_5Slot[i].fX, m_5Slot[i].fY, m_5Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_6Slot[i].fX      = m_5Slot[i].fX;
		m_6Slot[i].fY      = m_5Slot[i].fY - 82.f;
		m_6Slot[i].fZ      = m_5Slot[i].fZ;
		m_6Slot[i].iPass   = m_5Slot[i].iPass;
		m_6Slot[i].iTexNum = m_5Slot[i].iTexNum;
		m_6Slot[i].fHeight = m_5Slot[i].fHeight;
		m_6Slot[i].fWidth  = m_5Slot[i].fWidth;
		m_6Slot[i].fColorR = m_5Slot[i].fColorR;
		m_6Slot[i].fColorG = m_5Slot[i].fColorG;
		m_6Slot[i].fColorB = m_5Slot[i].fColorB;
		m_6Slot[i].fColorA = m_5Slot[i].fColorA;

		XMStoreFloat4x4(&(m_6Slot[i].WorldMatrix), XMMatrixScaling(m_6Slot[i].fWidth, m_6Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_6Slot[i].fX, m_6Slot[i].fY, m_6Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_7Slot[i].fX      = m_6Slot[i].fX;
		m_7Slot[i].fY      = m_6Slot[i].fY - 82.f;
		m_7Slot[i].fZ      = m_6Slot[i].fZ;
		m_7Slot[i].iPass   = m_6Slot[i].iPass;
		m_7Slot[i].iTexNum = m_6Slot[i].iTexNum;
		m_7Slot[i].fHeight = m_6Slot[i].fHeight;
		m_7Slot[i].fWidth  = m_6Slot[i].fWidth;
		m_7Slot[i].fColorR = m_6Slot[i].fColorR;
		m_7Slot[i].fColorG = m_6Slot[i].fColorG;
		m_7Slot[i].fColorB = m_6Slot[i].fColorB;
		m_7Slot[i].fColorA = m_6Slot[i].fColorA;

		XMStoreFloat4x4(&(m_7Slot[i].WorldMatrix), XMMatrixScaling(m_7Slot[i].fWidth, m_7Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_7Slot[i].fX, m_7Slot[i].fY, m_7Slot[i].fZ));
	}



	m_pDB = CItemDB::GetInstance();
	// 고정 박스, 보상메세지 박스 색상
	itemDesc[0] = m_pDB->GetItemData(ITEM::SWORD0);
	itemDesc[1] = m_pDB->GetItemData(ITEM::SWORD1);
	itemDesc[2] = m_pDB->GetItemData(ITEM::SWORD2);
	itemDesc[3] = m_pDB->GetItemData(ITEM::SWORD3);
	itemDesc[4] = m_pDB->GetItemData(ITEM::SWORD4);
	itemDesc[5] = m_pDB->GetItemData(ITEM::GUN0);
	itemDesc[6] = m_pDB->GetItemData(ITEM::GUN1);
	itemDesc[7] = m_pDB->GetItemData(ITEM::GUN2);
	itemDesc[8] = m_pDB->GetItemData(ITEM::GUN3);
	itemDesc[9] = m_pDB->GetItemData(ITEM::GUN4);
	itemDesc[10] = m_pDB->GetItemData(ITEM::TACTITE_COIN);
	itemDesc[11] = m_pDB->GetItemData(ITEM::EXP3);
	itemDesc[12] = m_pDB->GetItemData(ITEM::GEM);


	_float3 color0 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[0].eItemGrade);
	m_0Slot[1].fColorR = -(255.f - (color0.x * 255.f));
	m_0Slot[1].fColorG = -(255.f - (color0.y * 255.f));
	m_0Slot[1].fColorB = -(255.f - (color0.z * 255.f));

	_float3 color1 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[1].eItemGrade);
	m_1Slot[1].fColorR = -(255.f - (color1.x * 255.f));
	m_1Slot[1].fColorG = -(255.f - (color1.y * 255.f));
	m_1Slot[1].fColorB = -(255.f - (color1.z * 255.f));

	_float3 color2 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[2].eItemGrade);
	m_2Slot[1].fColorR = -(255.f - (color2.x * 255.f));
	m_2Slot[1].fColorG = -(255.f - (color2.y * 255.f));
	m_2Slot[1].fColorB = -(255.f - (color2.z * 255.f));

	_float3 color3 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[3].eItemGrade);
	m_3Slot[1].fColorR = -(255.f - (color3.x * 255.f));
	m_3Slot[1].fColorG = -(255.f - (color3.y * 255.f));
	m_3Slot[1].fColorB = -(255.f - (color3.z * 255.f));

	_float3 color4 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[4].eItemGrade);
	m_4Slot[1].fColorR = -(255.f - (color4.x * 255.f));
	m_4Slot[1].fColorG = -(255.f - (color4.y * 255.f));
	m_4Slot[1].fColorB = -(255.f - (color4.z * 255.f));

	_float3 color5 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[5].eItemGrade);
	m_5Slot[1].fColorR = -(255.f - (color5.x * 255.f));
	m_5Slot[1].fColorG = -(255.f - (color5.y * 255.f));
	m_5Slot[1].fColorB = -(255.f - (color5.z * 255.f));

	return S_OK;
}

void CUI_Souvenir::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	
	SetState(DISABLE);

	
	//CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 3); // 나중에 주석 풀기
	//ItemNum = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 4); // 나중에 주석 풀기


	

	// 들어오기, 나가기 
	/* NPC랑 플레이어랑 충돌하면 Setstate()로 활성화 , 비활성화*/
}

void CUI_Souvenir::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	switch (Situation)
	{
	case Client::CUI_Souvenir::MEET:
	{
		m_pUIMouse->Set_RenderMouse(true);
		m_bMouseActive = true;

		if (SelectUI(&m_SouList[0]))
		{
			if (false == m_SouList[0].OnRect)
			{
				for (auto& Sou : m_SouList)
				{
					Sou.fColorA = Sou.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_SouList[0].OnRect = true;
			}
		}

		if (true == m_SouList[0].OnRect)
		{
			if (MinusAlphaW(&m_SouList, TimeDelta))
			{
				m_SouList[0].OnRect = false;
				Situation = CUI_Souvenir::MENU;
			}
		}
		else
		{
			AddAlphaW(&m_SouList, TimeDelta);
		}
	}
		break;
	case Client::CUI_Souvenir::MENU:
	{
		if (m_MenuRenderStart)
		{
			if (AddAlphaW(&m_MenuList, TimeDelta))
				m_MenuRenderStart = false;
		}
		if (SelectUI(&m_MenuList[0])) // 랜더 도중에 메뉴에 마우스를 올리면
		{
			if ((false == m_MenuList[3].OnRect) && (false == m_MenuList[0].OnRect))
			{
				m_MenuList[0].iTexNum = 37;
				m_MenuList[3].iTexNum = 38;

				for (auto& Mer : m_MenuList)
				{
					Mer.fColorA = Mer.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_MenuList[0].OnRect = true;
			}
		}
		else if (SelectUI(&m_MenuList[3]))
		{
			if ((false == m_MenuList[3].OnRect) && (false == m_MenuList[0].OnRect))
			{
				m_MenuList[0].iTexNum = 38;
				m_MenuList[3].iTexNum = 37;
				for (auto& Mer : m_MenuList)
				{
					Mer.fColorA = Mer.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_MenuList[3].OnRect = true;
			}
		}

		if (true == m_MenuList[0].OnRect) // 인메뉴로 이동
		{
			if (MinusAlphaW(&m_MenuList, TimeDelta))
			{
				m_MenuRenderStart = true;
				m_MenuList[0].OnRect = false;
				Situation = CUI_Souvenir::INMENU;
			}
		}
		if (true == m_MenuList[3].OnRect) // 떠나다
		{
			if (MinusAlphaW(&m_MenuList, TimeDelta))
			{
				m_MenuRenderStart = true;
				m_MenuList[3].OnRect = false;
				Situation = CUI_Souvenir::MEET;
			}
		}
	}
		break;
	case Client::CUI_Souvenir::INMENU:
	{
		if (m_InMenuRenderStart)
			InMenuOpen(TimeDelta); // m_InMenuRnderStart 를 false로 
		
		//드래그바 움직임
		if (SelectUI(&m_CommonList[11]) && (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP))
		{
			m_bMouseMoveStart = true;
		}
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
		{
			m_bMouseMoveStart = false;
		}
		if(m_bMouseMoveStart)
			MouseMove();
		// 슬롯 위에 올리면 테두리
		IsMouseinRect();

		// 은화 현재 보유 수량
		SettingOwnTexNum();

		// 캔슬 누르면 목록으로 돌아가기, 텍스처 바꾸기
		if (SelectUI(&m_CommonList[4]))
		{
			m_CommonList[4].iTexNum = 8;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_InMenuRenderStart = false;
				Situation = CUI_Souvenir::MENU;
			}
		}
		else
			m_CommonList[4].iTexNum = 7;
		if (CUI_Souvenir::MENU == Situation)
		{
			MinusAlphaW(&m_CommonList, TimeDelta);
		}
	}
		break;
	case Client::CUI_Souvenir::DETAILS:
	{
		if (m_DetailRenderStart)
		{
			(AddAlphaW(&m_DetailsList, TimeDelta) ? m_DetailRenderStart = false : m_DetailRenderStart = true);
		}
		// 은화 현재 보유 수량
		SettingOwnTexNum();
		// 구매제한 수량 아이템 별로 나눌까봐 함수로 분리
		SettingLimitTexNum();
		SettingBuyTexNum();
		// QE ,클릭 수량 설정
		if (pGameInstance->InputKey(DIK_Q) == KEY_STATE::TAP)
		{
			// 구매수량 정할 때 0보다 작을 수 없음
			if (Limited == BuyNum)
			{
				m_DetailsList[14].bRender = true;
				m_DetailsList[16].bRender = true;
			}

			(0 < BuyNum ? --BuyNum : BuyNum = 0);

			if (0 == BuyNum)
			{
				m_DetailsList[13].bRender = false;
				m_DetailsList[15].bRender = false;
			}

		}
		if (SelectUI(&m_DetailsList[15]))
		{
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (Limited == BuyNum)
				{
					m_DetailsList[14].bRender = true;
					m_DetailsList[16].bRender = true;
				}

				(0 < BuyNum ? --BuyNum : BuyNum = 0);

				if (0 == BuyNum)
				{
					m_DetailsList[13].bRender = false;
					m_DetailsList[15].bRender = false;
				}

			}
		}

		//
		if (pGameInstance->InputKey(DIK_E) == KEY_STATE::TAP)
		{
			// 구매수량 정할 때 리미트를 넘길 수 없음
			if (0 == BuyNum)
			{
				m_DetailsList[13].bRender = true;
				m_DetailsList[15].bRender = true;
			}

			(Limited > BuyNum ? ++BuyNum : BuyNum = Limited);

			if (Limited == BuyNum)
			{
				m_DetailsList[14].bRender = false;
				m_DetailsList[16].bRender = false;
			}
		}
		if (SelectUI(&m_DetailsList[16]))
		{
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (0 == BuyNum)
				{
					m_DetailsList[13].bRender = true;
					m_DetailsList[15].bRender = true;
				}

				(Limited > BuyNum ? ++BuyNum : BuyNum = Limited);

				if (Limited == BuyNum)
				{
					m_DetailsList[14].bRender = false;
					m_DetailsList[16].bRender = false;
				}

			}
		}
		// 선택수량 * 아이템 비용 =  은화 총 비용 설정
		Total();

		// 캔슬 누르면 목록으로 돌아가기, 텍스처 바꾸기
		if (SelectUI(&m_DetailsList[1]))
		{
			m_DetailsList[1].iTexNum = 8;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				BuyNum = 0;
				m_DetailRenderStart = false;
				Situation = CUI_Souvenir::INMENU;
				(m_DetailsList[13].bRender == false ? m_DetailsList[13].bRender = true : m_DetailsList[13].bRender = true);
				(m_DetailsList[14].bRender == false ? m_DetailsList[14].bRender = true : m_DetailsList[14].bRender = true);
				(m_DetailsList[15].bRender == false ? m_DetailsList[15].bRender = true : m_DetailsList[15].bRender = true);
				(m_DetailsList[16].bRender == false ? m_DetailsList[16].bRender = true : m_DetailsList[16].bRender = true);
			}
		}
		else
			m_DetailsList[1].iTexNum = 7;

		if (SelectUI(&m_DetailsList[26]))
		{
			m_DetailsList[26].iTexNum = 20;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				BuyNum = 0;
				m_DetailRenderStart = false;
				Situation = CUI_Souvenir::INMENU;
				(m_DetailsList[13].bRender == false ? m_DetailsList[13].bRender = true : m_DetailsList[13].bRender = true);
				(m_DetailsList[14].bRender == false ? m_DetailsList[14].bRender = true : m_DetailsList[14].bRender = true);
				(m_DetailsList[15].bRender == false ? m_DetailsList[15].bRender = true : m_DetailsList[15].bRender = true);
				(m_DetailsList[16].bRender == false ? m_DetailsList[16].bRender = true : m_DetailsList[16].bRender = true);
			}
		}
		else
			m_DetailsList[26].iTexNum = 21;


		if (CUI_Souvenir::INMENU == Situation)
		{
			MinusAlphaW(&m_DetailsList, TimeDelta);
		}

		// 구매 누르면 확인창 뜨기,테스처 바꾸기
		if ((SelectUI(&m_DetailsList[27])) && (0<BuyNum) && (CurrentOwn >= iTotal))
		{
			m_DetailsList[27].iTexNum = 20;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				BuyNum = 0;
				Situation = CUI_Souvenir::CONFRIM;
				(m_DetailsList[13].bRender == false ? m_DetailsList[13].bRender = true : m_DetailsList[13].bRender = true);
				(m_DetailsList[14].bRender == false ? m_DetailsList[14].bRender = true : m_DetailsList[14].bRender = true);
				(m_DetailsList[15].bRender == false ? m_DetailsList[15].bRender = true : m_DetailsList[15].bRender = true);
				(m_DetailsList[16].bRender == false ? m_DetailsList[16].bRender = true : m_DetailsList[16].bRender = true);
			}
		}
		else
			m_DetailsList[27].iTexNum = 21;

	}
		break;
	case Client::CUI_Souvenir::CONFRIM:
	{
		if (m_ConfirmRenderStart)
			(AddAlphaW(&m_FinalList, TimeDelta) ? m_ConfirmRenderStart = false : m_ConfirmRenderStart = true);

		// 확인 누르면 목록으로 돌아가기, 색 변경
		// 구매수량이 0이거나 은화가 부족하면 구매X
		if (SelectUI(&m_FinalList[10]))
		{
			ColorP(&m_FinalList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
			ColorP(&m_FinalList[11], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_ConfirmRenderStart = true;
				//Limited += buynum;
				Situation = CUI_Souvenir::INMENU;
			}
		}
		else
		{
			ColorM(&m_FinalList[10], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
			ColorM(&m_FinalList[11], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
		}


		// 구매수량에 맞게 박스에 숫자 띄우기


	}
		break;
	case Client::CUI_Souvenir::BYE:
		break;
	}
}


void CUI_Souvenir::InMenuOpen(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (AddAlphaW(&m_CommonList, TimeDelta))
		++Count;
	if (AddAlphaW(&m_0Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_1Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_2Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_3Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_4Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_5Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_6Slot, TimeDelta))
		++Count;
	if (8 == Count)
		m_InMenuRenderStart = false;
}

_bool CUI_Souvenir::InMenuEnd(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (MinusAlphaW(&m_CommonList, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_0Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_1Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_2Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_3Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_4Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_5Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_6Slot, TimeDelta))
		++Count;
	

	if (8 == Count)
		return true;
	else
		return false;
}

void CUI_Souvenir::Total()
{
	iTotal = BuyNum * Cost;

	if (99 < iTotal)
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[23].iTexNum = iTotal / 100 + 27;
		m_DetailsList[24].iTexNum = (iTotal - (iTotal / 100) * 100) / 10 + 27;
		m_DetailsList[25].iTexNum = (iTotal - (iTotal / 100) * 100) % 10 + 27;
	}
	else if (9 < iTotal)
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = false;
		m_DetailsList[23].iTexNum = iTotal / 10 + 27;
		m_DetailsList[24].iTexNum = (iTotal - (iTotal / 10) * 10) + 27;
	}
	else
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = false;
		m_DetailsList[25].bRender = false;
		m_DetailsList[23].iTexNum = iTotal + 27;
	}
}

void CUI_Souvenir::SettingLimitTexNum()
{
		m_DetailsList[11].iTexNum = Limited + 27;
}
void CUI_Souvenir::SettingOwnTexNum()
{
	if (99 < CurrentOwn)
	{
		m_CommonList[8].bRender = m_DetailsList[6].bRender = true;
		m_CommonList[9].bRender = m_DetailsList[7].bRender = true;
		m_CommonList[10].bRender = m_DetailsList[8].bRender = true;
		m_CommonList[8].iTexNum = m_DetailsList[6].iTexNum = CurrentOwn / 100 + 27;
		m_CommonList[9].iTexNum = m_DetailsList[7].iTexNum = (CurrentOwn - (CurrentOwn / 100) * 100) / 10 + 27;
		m_CommonList[10].iTexNum = m_DetailsList[8].iTexNum = (CurrentOwn - (CurrentOwn / 100) * 100) % 10 + 27;
	}
	else if (9 < CurrentOwn)
	{
		m_CommonList[8].bRender = m_DetailsList[6].bRender = true;
		m_CommonList[9].bRender = m_DetailsList[7].bRender = true;
		m_CommonList[10].bRender = m_DetailsList[8].bRender = false;
		m_CommonList[8].iTexNum = m_DetailsList[6].iTexNum = CurrentOwn / 10 + 27;
		m_CommonList[9].iTexNum = m_DetailsList[7].iTexNum = (CurrentOwn - (CurrentOwn / 10) * 10) + 27;
	}
	else
	{
		m_CommonList[8].bRender = m_DetailsList[6].bRender = true;
		m_CommonList[9].bRender = m_DetailsList[7].bRender = false;
		m_CommonList[10].bRender = m_DetailsList[8].bRender = false;
		m_CommonList[8].iTexNum = m_DetailsList[6].iTexNum = CurrentOwn + 27;
	}
}
void CUI_Souvenir::SettingBuyTexNum()
{
	m_DetailsList[17].bRender = false;
	m_DetailsList[18].bRender = false;
	m_DetailsList[19].iTexNum = BuyNum + 27;
}
void CUI_Souvenir::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI_POST, this);
}

HRESULT CUI_Souvenir::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;
	switch (Situation)
	{
	case Client::CUI_Souvenir::MEET:
	{
		for (_uint i = 0; i < (_int)m_SouList.size(); ++i)
		{
			if (m_SouList[i].bRender)
			{
				if (FAILED(Setup_SouShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Souvenir::MENU:
	{
		for (_uint i = 0; i < (_int)m_MenuList.size(); ++i)
		{
			if (m_MenuList[i].bRender)
			{
				if (FAILED(Setup_MenuShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Souvenir::INMENU:
	{
		for (_uint i = 0; i < (_int)m_CommonList.size(); ++i)
		{
			if (m_CommonList[i].bRender)
			{
				if (FAILED(Setup_CommonShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
		{
			if (m_0Slot[i].bRender)
			{
				if (FAILED(Setup_0SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
		{
			if (m_1Slot[i].bRender)
			{
				if (FAILED(Setup_1SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_2Slot.size(); ++i)
		{
			if (m_2Slot[i].bRender)
			{
				if (FAILED(Setup_2SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_3Slot.size(); ++i)
		{
			if (m_3Slot[i].bRender)
			{
				if (FAILED(Setup_3SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_4Slot.size(); ++i)
		{
			if (m_4Slot[i].bRender)
			{
				if (FAILED(Setup_4SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_5Slot.size(); ++i)
		{
			if (m_5Slot[i].bRender)
			{
				if (FAILED(Setup_5SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_6Slot.size(); ++i)
		{
			if (m_6Slot[i].bRender)
			{
				if (FAILED(Setup_6SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_7Slot.size(); ++i)
		{
			if (m_7Slot[i].bRender)
			{
				if (FAILED(Setup_7SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}

	}
	break;
	case Client::CUI_Souvenir::DETAILS:
	{
		for (_uint i = 0; i < (_int)m_DetailsList.size(); ++i)
		{
			if (m_DetailsList[i].bRender)
			{
				if (FAILED(Setup_DetailsShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		break;
	case Client::CUI_Souvenir::CONFRIM:
	{
		for (_uint i = 0; i < (_int)m_FinalList.size(); ++i)
		{
			if (m_FinalList[i].bRender)
			{
				if (FAILED(Setup_FinalShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Souvenir::BYE:
	{
	}
	break;
	}
	}


	return S_OK;
}

void CUI_Souvenir::RenderGUI()
{
}

_bool CUI_Souvenir::SelectUI(SOUDESC* pDesc)
{
	_float3	fMousePos = m_pUIMouse->Get_MousePos();
	_vector vMouse = XMLoadFloat3(&fMousePos);
	_float Dist = 1.f;
	// 버퍼의 각 꼭지점
	_vector P0 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P1 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P2 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P3 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);

	// UI크기에 맞춰서 범위체크
	if ((XMVectorGetX(P0) < XMVectorGetX(vMouse)) && (XMVectorGetX(P1) > XMVectorGetX(vMouse)))
	{
		if ((XMVectorGetY(P0) > XMVectorGetY(vMouse)) && (XMVectorGetY(P2) < XMVectorGetY(vMouse)))
		{
			m_pUIMouse->Set_Texchange(true);
			return true;
		}
		else
		{
			return false;

		}
	}
	else
	{
		return false;
	}

	return false;
}

HRESULT CUI_Souvenir::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UISOUVI,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_Souvenir::Setup_SouShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_SouList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_SouList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_SouList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_SouList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_SouList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_SouList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_SouList[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_MenuShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_MenuList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_MenuList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_MenuList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_MenuList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_MenuList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_MenuList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_MenuList[index].iPass;

	return S_OK;
}

void CUI_Souvenir::IsMouseinRect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_0Slot[0]))
	{
		m_0Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_0Slot[8].bRender = false;

	if (SelectUI(&m_1Slot[0]))
	{
		m_1Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_1Slot[8].bRender = false;

	if (SelectUI(&m_2Slot[0]))
	{
		m_2Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_2Slot[8].bRender = false;

	if (SelectUI(&m_3Slot[0]))
	{
		m_3Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_3Slot[8].bRender = false;

	if (SelectUI(&m_4Slot[0]))
	{
		m_4Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_4Slot[8].bRender = false;

	if (SelectUI(&m_5Slot[0]))
	{
		m_5Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_5Slot[8].bRender = false;

	if (SelectUI(&m_6Slot[0]))
	{
		m_6Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_6Slot[8].bRender = false;

	if (SelectUI(&m_7Slot[0]))
	{
		m_7Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Souvenir::DETAILS;
		}
	}
	else
		m_7Slot[8].bRender = false;
}
_bool CUI_Souvenir::AddAlpha(SOUDESC* pDesc, _double TimeDelta)
{
	if (pDesc->Color.w > pDesc->fColorA)
		{
			pDesc->fColorA += (_float)TimeDelta * 100.f;
		}
		else
		{
			pDesc->fColorA = pDesc->Color.w;
			return true;
		}
	return false;
}

_bool CUI_Souvenir::MinusAlpha(SOUDESC* pDesc, _double TimeDelta)
{
		if (pDesc->Color.w < pDesc->fColorA)
		{
			pDesc->fColorA -= (_float)TimeDelta * 100.f;
		}
		else
		{
			pDesc->fColorA = pDesc->Color.w;
			return true;
		}
	return false;
}

_bool CUI_Souvenir::AddAlphaW(vector<SOUDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if ((*pDesc)[i].Color.w > (*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA += (_float)TimeDelta * 400.f;
		}
		else
		{
			(*pDesc)[i].fColorA = (*pDesc)[i].Color.w;
		}
	}
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (0.1f > abs((*pDesc)[i].Color.w - (*pDesc)[i].fColorA))
		{
			++m_Count;
		}
		if (m_Count == (_int)(*pDesc).size())
		{
			m_Count = 0;
			return true;
		}
	}
	m_Count = 0;
	return false;
}

_bool CUI_Souvenir::MinusAlphaW(vector<SOUDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (-255.f < (*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA -= (_float)TimeDelta * 400.f;
		}
		else
		{
			(*pDesc)[i].fColorA = -255.f;
		}
	}
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (0.01f > abs(-255.f - (*pDesc)[i].fColorA))
		{
			++m_Count;
		}
		if (m_Count == (_int)(*pDesc).size())
		{
			m_Count = 0;
			return true;
		}
	}
	m_Count = 0;
	return false;
}

void CUI_Souvenir::ColorP(SOUDESC* pDesc, _float4 fcolor, _double TimeDelta)
{
	if (0.1f < fcolor.x - pDesc->fColorR)
	{
		pDesc->fColorR += fcolor.x * (_float)TimeDelta;
	}
	else
		pDesc->fColorR = fcolor.x;
	if (0.1f < fcolor.y - pDesc->fColorG)
	{
		pDesc->fColorG += fcolor.y * (_float)TimeDelta;
	}
	else
		pDesc->fColorG = fcolor.y;
	if (0.1f < fcolor.z - pDesc->fColorB)
	{
		pDesc->fColorB += fcolor.z * (_float)TimeDelta;
	}
	else
		pDesc->fColorB = fcolor.z;
}

void CUI_Souvenir::ColorM(SOUDESC* pDesc, _float4 fcolor, _double TimeDelta)
{
	if (0.1f < abs(fcolor.x - pDesc->fColorR))
	{
		pDesc->fColorR -= pDesc->fColorR * (_float)TimeDelta;
	}
	else
		pDesc->fColorR = fcolor.x;
	if (0.1f < abs(fcolor.y - pDesc->fColorG))
	{
		pDesc->fColorG -= pDesc->fColorG * (_float)TimeDelta;
	}
	else
		pDesc->fColorG = fcolor.y;
	if (0.1f < abs(fcolor.z - pDesc->fColorB))
	{
		pDesc->fColorB -= pDesc->fColorB * (_float)TimeDelta;
	}
	else
		pDesc->fColorB = fcolor.z;
}

void CUI_Souvenir::MouseMove()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	mouse = pGameInstance->InputMouseMove(DIMM_Y);
	(134.f > m_CommonList[11].fY ? m_CommonList[11].fY += -mouse : m_CommonList[11].fY = 134.f);
	(-156.f <= m_CommonList[11].fY ? m_CommonList[11].fY += -mouse : m_CommonList[11].fY = -156.f);

	XMStoreFloat4x4(&(m_CommonList[11].WorldMatrix), XMMatrixScaling(m_CommonList[11].fWidth, m_CommonList[11].fHeight, 1.f)
		* XMMatrixTranslation(m_CommonList[11].fX, m_CommonList[11].fY, m_CommonList[11].fZ));

	for (auto& Slot : m_0Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_1Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_2Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_3Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_4Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_5Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_6Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_7Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
}

void CUI_Souvenir::Load()
{
	//
	_uint index = 3;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Souvenir%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.iPass = 21;
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_SouList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 6;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouMenu%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.iPass = 21;
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_MenuList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 12;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouCommon%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		//Desc.iPass = 21;
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_CommonList.push_back(Desc);
		CloseHandle(hFile);

	}

	index = 14;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouFinal%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_FinalList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 34;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouDetails%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_DetailsList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_0Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_1Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_2Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/3Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_3Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/4Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_4Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/5Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_5Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/6Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_6Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/7Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		SOUDESC Desc;
		ZeroMemory(&Desc, sizeof(SOUDESC));

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
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_7Slot.push_back(Desc);
		CloseHandle(hFile);

	}
}








void CUI_Souvenir::Save()
{
	//
	_uint index = 0;
	for (auto& Desc : m_SouList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Souvenir%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);

		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_MenuList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouMenu%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);

		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_CommonList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouCommon%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.bRender), sizeof(_bool), &dwByte, nullptr);

		CloseHandle(hFile);
		++index;
	}


	//
	index = 0;
	for (auto& Desc : m_FinalList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouFinal%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_DetailsList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/SouDetails%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_0Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_1Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_2Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_3Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/3Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_4Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/4Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_5Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/5Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_6Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/6Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_7Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/7Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		Desc.fColorA = Desc.Color.w;
		_ulong dwByte = 0;
		WriteFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);
		CloseHandle(hFile);
		++index;
	}

}

HRESULT CUI_Souvenir::Setup_CommonShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CommonList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CommonList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CommonList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CommonList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CommonList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CommonList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_CommonList[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_DetailsShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DetailsList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DetailsList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DetailsList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DetailsList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DetailsList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DetailsList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_DetailsList[index].iPass;

	return S_OK;
}
HRESULT CUI_Souvenir::Setup_0SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SWORD_0, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_0Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_0Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_0Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_0Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_0Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_0Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_0Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_1SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SWORD_1, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_1Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_1Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_1Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_1Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_1Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_1Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_1Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_2SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SWORD_2, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_2Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_2Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_2Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_2Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_2Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_2Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_2Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_3SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SWORD_3, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_3Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_3Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_3Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_3Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_3Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_3Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_3Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_4SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SWORD_4, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_4Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_4Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_4Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_4Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_4Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_4Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_4Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_5SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_GUN_0, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_5Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_5Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_5Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_5Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_5Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_5Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_5Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_6SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_GUN_1, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_6Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_6Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_6Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_6Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_6Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_6Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_6Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Souvenir::Setup_7SlotShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (2 == index)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_GUN_2, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_7Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_7Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_7Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_7Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_7Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_7Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_7Slot[index].iPass;

	return S_OK;
}


HRESULT CUI_Souvenir::Setup_FinalShader(_uint index)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_FinalList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_FinalList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_FinalList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_FinalList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_FinalList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_FinalList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_FinalList[index].iPass;

	return S_OK;
}


CUI_Souvenir* CUI_Souvenir::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Souvenir* pInstance = new CUI_Souvenir(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Souvenir";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Souvenir::Clone(void* pArg)
{
	CUI_Souvenir* pInstance = new CUI_Souvenir(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Souvenir";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Souvenir::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}