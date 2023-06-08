#include "stdafx.h"
#include "..\Public\UI_MerchantMen.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "UI_MainScreen.h"
#include "UI_Minimap.h"
#include "PlayerState.h"
#include "ItemDB.h"
#include "Item.h"
#include "Inventory.h"


CUI_MerchantMen::CUI_MerchantMen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_MerchantMen::CUI_MerchantMen(const CUI_MerchantMen& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_MerchantMen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MerchantMen::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	Load();

	return S_OK;
}

void CUI_MerchantMen::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	//m_PlayerCurrentLevel = m_pPlayerStateClass->Get_MainCharacterState()->iCurLevel; // 나중에 주석 풀기
	m_pDB = CItemDB::GetInstance();
	SetState(DISABLE);

	// 고정 박스, 보상메세지 박스 색상
	itemDesc[0] = m_pDB->GetItemData(ITEM::TACTITE_COIN);
	itemDesc[1] = m_pDB->GetItemData(ITEM::EXP0);
	itemDesc[2] = m_pDB->GetItemData(ITEM::EXP1);
	itemDesc[3] = m_pDB->GetItemData(ITEM::EXP2);
	itemDesc[4] = m_pDB->GetItemData(ITEM::COMMEMORATIVE_COIN);
	itemDesc[5] = m_pDB->GetItemData(ITEM::TACTREITE_VOUCHER);
	//CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 3); // 나중에 주석 풀기
	//ItemNum = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 4); // 나중에 주석 풀기


	_float3 color0 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[0].eItemGrade);
	m_FinalList[2].fColorR = m_0RewardList[0].fColorR = -(255.f - (color0.x * 255.f));
	m_FinalList[2].fColorG = m_0RewardList[0].fColorG = -(255.f - (color0.y * 255.f));
	m_FinalList[2].fColorB = m_0RewardList[0].fColorB = -(255.f - (color0.z * 255.f));

	_float3 color1 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[1].eItemGrade);
	m_FinalList[3].fColorR = m_1RewardList[0].fColorR = -(255.f - (color1.x * 255.f));
	m_FinalList[3].fColorG = m_1RewardList[0].fColorG = -(255.f - (color1.y * 255.f));
	m_FinalList[3].fColorB = m_1RewardList[0].fColorB = -(255.f - (color1.z * 255.f));

	_float3 color2 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[2].eItemGrade);
	m_FinalList[4].fColorR = m_2RewardList[0].fColorR = -(255.f - (color2.x * 255.f));
	m_FinalList[4].fColorG = m_2RewardList[0].fColorG = -(255.f - (color2.y * 255.f));
	m_FinalList[4].fColorB = m_2RewardList[0].fColorB = -(255.f - (color2.z * 255.f));

	_float3 color3 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[3].eItemGrade);
	m_FinalList[5].fColorR = m_3RewardList[0].fColorR = -(255.f - (color3.x * 255.f));
	m_FinalList[5].fColorG = m_3RewardList[0].fColorG = -(255.f - (color3.y * 255.f));
	m_FinalList[5].fColorB = m_3RewardList[0].fColorB = -(255.f - (color3.z * 255.f));

	_float3 color4 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[4].eItemGrade);
	m_FinalList[6].fColorR = m_4RewardList[0].fColorR = -(255.f - (color4.x * 255.f));
	m_FinalList[6].fColorG = m_4RewardList[0].fColorG = -(255.f - (color4.y * 255.f));
	m_FinalList[6].fColorB = m_4RewardList[0].fColorB = -(255.f - (color4.z * 255.f));

	_float3 color5 = m_pDB->GetItemColor((CItem::ITEM_GRADE)itemDesc[5].eItemGrade);
	m_FinalList[7].fColorR = m_5RewardList[0].fColorR = -(255.f - (color5.x * 255.f));
	m_FinalList[7].fColorG = m_5RewardList[0].fColorG = -(255.f - (color5.y * 255.f));
	m_FinalList[7].fColorB = m_5RewardList[0].fColorB = -(255.f - (color5.z * 255.f));

	// 들어오기, 나가기 
	/* NPC랑 플레이어랑 충돌하면 Setstate()로 활성화 , 비활성화*/
}

void CUI_MerchantMen::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	// 레벨에 따라 보상 아이템, 수치 설정
	switch (SettingLevel)
	{
	case 10:
	{
		m_0RewardList[5].iTexNum = 117; // 코인     26000
		m_1RewardList[5].iTexNum = 95;  // 경험치0  3
		m_2RewardList[5].iTexNum = 94;  // 경험치1  2 
		m_3RewardList[5].iTexNum = 93;  // 경험치2  1
		m_4RewardList[5].iTexNum = 93;  // 멋진코인 1
		m_5RewardList[5].iTexNum = 93;  // 레전드 1

		m_FinalList[21].iTexNum = 117;
		m_FinalList[22].iTexNum = 95;
		m_FinalList[23].iTexNum = 94;
		m_FinalList[24].iTexNum = 93;
		m_FinalList[25].iTexNum = 93;
		m_FinalList[26].iTexNum = 93;

		NeedNum = 2;
		m_CasketList[4].iTexNum = 69;
	}
	break;
	case 20:
	{
		m_0RewardList[5].iTexNum = 120; // 코인     42000
		m_1RewardList[5].iTexNum = 96;  // 경험치0  5
		m_2RewardList[5].iTexNum = 94;  // 경험치1  2 
		m_3RewardList[5].iTexNum = 94;  // 경험치2  2
		m_4RewardList[5].iTexNum = 94;  // 멋진코인 1
		m_5RewardList[5].iTexNum = 93;  // 레전드 1

		m_FinalList[21].iTexNum = 120;
		m_FinalList[22].iTexNum = 96;
		m_FinalList[23].iTexNum = 94;
		m_FinalList[24].iTexNum = 94;
		m_FinalList[25].iTexNum = 94;
		m_FinalList[26].iTexNum = 93;

		NeedNum = 3; 
		m_CasketList[4].iTexNum = 70;
	}
	break;
	case 30:
	{
		m_0RewardList[5].iTexNum = 123; // 코인     64000
		m_1RewardList[5].iTexNum = 98;  // 경험치0  8
		m_2RewardList[5].iTexNum = 95;  // 경험치1  3 
		m_3RewardList[5].iTexNum = 94;  // 경험치2  2
		m_4RewardList[5].iTexNum = 94;  // 멋진코인 2
		m_5RewardList[5].iTexNum = 94;  // 레전드 2

		m_FinalList[21].iTexNum = 123;
		m_FinalList[22].iTexNum = 98;
		m_FinalList[23].iTexNum = 95;
		m_FinalList[24].iTexNum = 94;
		m_FinalList[25].iTexNum = 94;
		m_FinalList[26].iTexNum = 94;

		NeedNum = 5; 
		m_CasketList[4].iTexNum = 72;
	}
	break;
	case 40:
	{
		m_0RewardList[5].iTexNum = 126; // 코인     106000
		m_1RewardList[5].iTexNum = 99;  // 경험치0  10
		m_2RewardList[5].iTexNum = 97;  // 경험치1  6 
		m_3RewardList[5].iTexNum = 95;  // 경험치2  3
		m_4RewardList[5].iTexNum = 95;  // 멋진코인 3
		m_5RewardList[5].iTexNum = 94;  // 레전드 2

		m_FinalList[21].iTexNum = 126;
		m_FinalList[22].iTexNum = 99;
		m_FinalList[23].iTexNum = 97;
		m_FinalList[24].iTexNum = 95;
		m_FinalList[25].iTexNum = 95;
		m_FinalList[26].iTexNum = 94;

		NeedNum = 7; 
		m_CasketList[4].iTexNum = 74;
	}
	break;
	case 50:
	{
		m_0RewardList[5].iTexNum = 129; // 코인     169000
		m_1RewardList[5].iTexNum = 99;  // 경험치0  10
		m_2RewardList[5].iTexNum = 98;  // 경험치1  8
		m_3RewardList[5].iTexNum = 95;  // 경험치2  5
		m_4RewardList[5].iTexNum = 95;  // 멋진코인 5
		m_5RewardList[5].iTexNum = 95;  // 레전드 3

		m_FinalList[21].iTexNum = 129;
		m_FinalList[22].iTexNum = 99;
		m_FinalList[23].iTexNum = 98;
		m_FinalList[24].iTexNum = 95;
		m_FinalList[25].iTexNum = 95;
		m_FinalList[26].iTexNum = 95;

		NeedNum = 9; 
		m_CasketList[4].iTexNum = 76;
	}
	break;
	}

	//m_PlayerCurrentLevel = m_pPlayerStateClass->Get_MainCharacterState()->iCurLevel; // 나중에 주석 풀기
	//CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 3); // 나중에 주석 풀기
	//ItemNum = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, 4); // 나중에 주석 풀기
	_int num = ItemNum / 1;
	m_CasketList[3].iTexNum = 67 + num;
	_int Ten = CurrentOwn / 10;
	m_OwnList[2].iTexNum = 67 + Ten;
	_int One = CurrentOwn - Ten * 10;
	m_OwnList[3].iTexNum = 67 + One;
	_int Dest = NeedNum * (CurrentOwn / NeedNum) / 10;
	_int Dest2 = NeedNum * (CurrentOwn / NeedNum) - (Dest * 10);
	m_MessageList[10].iTexNum = 67 + Dest;
	m_MessageList[11].iTexNum = 67 + Dest2;


	switch (Situation)
	{
	case Client::CUI_MerchantMen::MEET:
	{
		m_pUIMouse->Set_RenderMouse(true);
		m_bMouseActive = true;

		if (SelectUI(&m_MerchantList[0]))
		{

			if (false == m_MerchantList[0].OnRect)
			{
				for (auto& Mer : m_MerchantList)
				{
					Mer.fColorA = Mer.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_MerchantList[0].OnRect = true;
			}
		}

		if (true == m_MerchantList[0].OnRect)
		{
			if (MinusAlphaW(&m_MerchantList, TimeDelta))
			{
				m_MerchantList[0].OnRect = false;
				Situation = CUI_MerchantMen::MENU;
			}
		}
		else
		{
			AddAlphaW(&m_MerchantList, TimeDelta);
		}
	}
	break;
	case Client::CUI_MerchantMen::MENU:
	{
		if (m_MenuRnderStart)
		{
			if (AddAlphaW(&m_MenuList, TimeDelta))
				m_MenuRnderStart = false;
		}
		if (SelectUI(&m_MenuList[0])) // 랜더 도중에 메뉴에 마우스를 올리면
		{
			if ((false == m_MenuList[3].OnRect) && (false == m_MenuList[0].OnRect))
			{
				m_MenuList[0].iTexNum = 79;
				m_MenuList[3].iTexNum = 80;

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
				m_MenuList[0].iTexNum = 80;
				m_MenuList[3].iTexNum = 79;
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
				m_MenuRnderStart = true;
				m_MenuList[0].OnRect = false;
				Situation = CUI_MerchantMen::INMENU;
			}
		}
		if (true == m_MenuList[3].OnRect) // 떠나다
		{
			if (MinusAlphaW(&m_MenuList, TimeDelta))
			{
				m_MenuRnderStart = true;
				m_MenuList[3].OnRect = false;
				Situation = CUI_MerchantMen::BYE;
			}
		}
	}
	break;
	case Client::CUI_MerchantMen::INMENU:
	{
		// 창 열릴 때 전체 랜더on
		if (m_InMenuRnderStart)
			InMenuOpen(TimeDelta); // m_InMenuRnderStart 를 false로 
		else
		{
			if (0.f > m_CommonList[11].fColorA)
			{
				AddAlpha(&m_CommonList[11], TimeDelta*2.f);
			}
			else
			{
				if (m_CircleSTurntart)
				{
					Degree = 360.f;
					SubDegree += (_float)TimeDelta * 80.f;
					if (SubDegree < Degree)
					{
						XMStoreFloat4x4(&(m_CircleList[0].WorldMatrix), XMMatrixScaling(m_CircleList[0].fWidth, m_CircleList[0].fHeight, 1.f)
							* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(SubDegree))
							* XMMatrixTranslation(m_CircleList[0].fX, m_CircleList[0].fY, m_CircleList[0].fZ));
						XMStoreFloat4x4(&(m_CircleList[1].WorldMatrix), XMMatrixScaling(m_CircleList[1].fWidth, m_CircleList[1].fHeight, 1.f)
							* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-SubDegree))
							* XMMatrixTranslation(m_CircleList[1].fX, m_CircleList[1].fY, m_CircleList[1].fZ));
					}
					else
					{
						m_CircleSTurntart = false;
						SubDegree = 0.f;
					}
				}
			}
		}

		_int Ten = SettingLevel / 10;
		m_LevelList[0].iTexNum = 67 + Ten;

		// 원 돌리기
		if (m_CircleLevDown)
		{
			Degree = (360.f / 5.f) * (_float)Ten;

			if (SubDegree >= Degree)
			{

				SubDegree -= (_float)TimeDelta * 50.f;
				XMStoreFloat4x4(&(m_CircleList[0].WorldMatrix), XMMatrixScaling(m_CircleList[0].fWidth, m_CircleList[0].fHeight, 1.f)
					* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(SubDegree))
					* XMMatrixTranslation(m_CircleList[0].fX, m_CircleList[0].fY, m_CircleList[0].fZ));

				XMStoreFloat4x4(&(m_CircleList[1].WorldMatrix), XMMatrixScaling(m_CircleList[1].fWidth, m_CircleList[1].fHeight, 1.f)
					* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-SubDegree))
					* XMMatrixTranslation(m_CircleList[1].fX, m_CircleList[1].fY, m_CircleList[1].fZ));
			}

		}

		if (m_CircleSLevUp)
		{
			Degree = (360.f / 5.f) * (_float)Ten;

			if (SubDegree < Degree)
			{
				SubDegree += (_float)TimeDelta * 50.f;
				XMStoreFloat4x4(&(m_CircleList[0].WorldMatrix), XMMatrixScaling(m_CircleList[0].fWidth, m_CircleList[0].fHeight, 1.f)
					* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(SubDegree))
					* XMMatrixTranslation(m_CircleList[0].fX, m_CircleList[0].fY, m_CircleList[0].fZ));

				XMStoreFloat4x4(&(m_CircleList[1].WorldMatrix), XMMatrixScaling(m_CircleList[1].fWidth, m_CircleList[1].fHeight, 1.f)
					* XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-SubDegree))
					* XMMatrixTranslation(m_CircleList[1].fX, m_CircleList[1].fY, m_CircleList[1].fZ));
			}
		}

		// 레벨 변경 시	
		/*
		레벨 마이너스
		- 레벨 렌더 설정 	v
		- q 입력 v
		- 마우스 입력 v
		- 설정 레벨이 10보다 작을 때 랜더, 입력off  10, 20, 30, 40, 50 v

		- 이미 받은 보상 체크 , 이미 받은 보상 문구

		- 레벨에 따라 보상 설정
		- 보상 목록이 바뀜
		- 보상버튼 클릭
		- 소모 확인 창
		- 보상 확인 창
		*/

		switch (SettingLevel)
		{
		case 10:
		{
			if ((false == m_bRewardReceived[0]) && (m_PlayerCurrentLevel >= SettingLevel) && (CurrentOwn >= NeedNum))
			{
				if (SelectUI(&m_DeliverList[0])) // 버튼 누르면
				{
					m_DeliverList[0].iTexNum = 54;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_MsgboxRender = true;
					}
				}
				else
					m_DeliverList[0].iTexNum = 55;
			}
			else
				m_DeliverList[0].iTexNum = 56;

			if (m_MsgboxRender)
			{
				if (false == m_RewardboxRender) // 확인창이 켜져있으면 이전 창의 버튼 입력은 무시됨
				{
					if (SelectUI(&m_MessageList[1])) // 취소 누르면
					{
						m_MessageList[1].iTexNum = 12;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[1].iTexNum = 11;
					if (SelectUI(&m_MessageList[4])) // 취소 누르면
					{
						m_MessageList[4].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[4].iTexNum = 55;

					if (SelectUI(&m_MessageList[5])) // 확인 누르면
					{
						m_MessageList[5].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = true;
							ItemNum = CurrentOwn / NeedNum;
						}
					}
					else
						m_MessageList[5].iTexNum = 55;
				}

				if (false == m_CancelMsgbox)
					AddAlphaW(&m_MessageList, TimeDelta);

				if (m_CancelMsgbox)
				{
					if (MinusAlphaW(&m_MessageList, TimeDelta * 1.2f))
					{
						m_MsgboxRender = false;
						m_CancelMsgbox = false;
					}
				}



				if (m_RewardboxRender) // 확인버튼 누르면 보상 확인 창이 뜸
				{
					for (_int i = 1; i < 12; ++i)
					{
						m_MessageList[i].bRender = false;
					}

					AddAlphaW(&m_FinalList, TimeDelta);

					if (SelectUI(&m_FinalList[27])) // confrim 누르면
					{
						ColorP(&m_FinalList[27], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						ColorP(&m_FinalList[28], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = false;
							m_bRewardReceived[0] = true;
							m_MessageList[0].bRender = false;

							// 보상 인벤에 넣기
							m_pInven->AddItem(ITEM::TACTITE_COIN, 26000);
							m_pInven->AddItem(ITEM::EXP0, 3);
							m_pInven->AddItem(ITEM::EXP1, 2);
							m_pInven->AddItem(ITEM::EXP2, 1);
							m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, 1);
							m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, 1);
							m_pInven->AddItem(ITEM::CASKET, CurrentOwn / NeedNum);
							m_pInven->EraseItem(CInventory::INVEN_MATERIAL, 4, NeedNum * (CurrentOwn / NeedNum));
							CurrentOwn = CurrentOwn - (ItemNum * NeedNum);



						}
					}
					else
					{
						ColorM(&m_FinalList[27], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
						ColorM(&m_FinalList[28], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					}
				}

				if (m_bRewardReceived[0])
				{
					if (false == m_RewardboxRender)
					{
						if (MinusAlphaW(&m_FinalList, TimeDelta))
						{
							m_CancelMsgbox = true;
						}
					}

				}

			}
			else // 메세지 박스가 off라면 리셋
			{

				for (_int i = 0; i < 12; ++i)
				{
					m_MessageList[i].bRender = true;
				}

				MinusAlphaW(&m_MessageList, TimeDelta);
			}


			//클릭체크 m_bRewardReceived[0] 참거짓
			if (m_bRewardReceived[0])// 수령버튼 클릭
			{
				// 받은 상태
				m_DeliverList[0].iTexNum = 56;
				m_DeliverList[2].iTexNum = 57;
				m_DeliverList[2].bRender = true;

				m_0RewardList[3].bRender = true;
				m_0RewardList[4].bRender = true;
				m_0RewardList[4].iTexNum = 92;
				m_1RewardList[3].bRender = true;
				m_1RewardList[4].bRender = true;
				m_1RewardList[4].iTexNum = 92;
				m_2RewardList[3].bRender = true;
				m_2RewardList[4].bRender = true;
				m_2RewardList[4].iTexNum = 92;
				m_3RewardList[3].bRender = true;
				m_3RewardList[4].bRender = true;
				m_3RewardList[4].iTexNum = 92;
				m_4RewardList[3].bRender = true;
				m_4RewardList[4].bRender = true;
				m_4RewardList[4].iTexNum = 92;
				m_5RewardList[3].bRender = true;
				m_5RewardList[4].bRender = true;
				m_5RewardList[4].iTexNum = 92;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = false;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = false;
				}
			}
			else // 미클릭
			{
				// 받을 수 있지만 아직 안 받은 상태
				m_DeliverList[2].bRender = false;
				m_0RewardList[3].bRender = false;
				m_0RewardList[4].bRender = false;
				m_1RewardList[3].bRender = false;
				m_1RewardList[4].bRender = false;
				m_2RewardList[3].bRender = false;
				m_2RewardList[4].bRender = false;
				m_3RewardList[3].bRender = false;
				m_3RewardList[4].bRender = false;
				m_4RewardList[3].bRender = false;
				m_4RewardList[4].bRender = false;
				m_5RewardList[3].bRender = false;
				m_5RewardList[4].bRender = false;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = true;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = true;
				}
			}
		}
		break;
		case 20:
		{
			if ((false == m_bRewardReceived[1]) && (m_PlayerCurrentLevel >= SettingLevel) && (CurrentOwn >= NeedNum))
			{
				if (SelectUI(&m_DeliverList[0])) // 버튼 누르면
				{
					m_DeliverList[0].iTexNum = 54;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_MsgboxRender = true;
					}
				}
				else
					m_DeliverList[0].iTexNum = 55;
			}
			else
				m_DeliverList[0].iTexNum = 56;

			if (m_MsgboxRender)
			{
				if (false == m_RewardboxRender) // 확인창이 켜져있으면 이전 창의 버튼 입력은 무시됨
				{
					if (SelectUI(&m_MessageList[1])) // 취소 누르면
					{
						m_MessageList[1].iTexNum = 12;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[1].iTexNum = 11;
					if (SelectUI(&m_MessageList[4])) // 취소 누르면
					{
						m_MessageList[4].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[4].iTexNum = 55;

					if (SelectUI(&m_MessageList[5])) // 확인 누르면
					{
						m_MessageList[5].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = true;
							ItemNum = CurrentOwn / NeedNum; // 임시 설정
						}
					}
					else
						m_MessageList[5].iTexNum = 55;
				}

				if (false == m_CancelMsgbox)
					AddAlphaW(&m_MessageList, TimeDelta);

				if (m_CancelMsgbox)
				{
					if (MinusAlphaW(&m_MessageList, TimeDelta * 1.2f))
					{
						m_MsgboxRender = false;
						m_CancelMsgbox = false;
					}
				}



				if (m_RewardboxRender) // 확인버튼 누르면 보상 확인 창이 뜸
				{
					for (_int i = 1; i < 12; ++i)
					{
						m_MessageList[i].bRender = false; // 나갈때 켜줘야하며
					}

					AddAlphaW(&m_FinalList, TimeDelta);

					if (SelectUI(&m_FinalList[27])) // confrim 누르면
					{
						ColorP(&m_FinalList[27], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						ColorP(&m_FinalList[28], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = false;
							m_bRewardReceived[1] = true;
							m_MessageList[0].bRender = false;

							// 보상 인벤에 넣기
							m_pInven->AddItem(ITEM::TACTITE_COIN, 42000);
							m_pInven->AddItem(ITEM::EXP0, 5);
							m_pInven->AddItem(ITEM::EXP1, 2);
							m_pInven->AddItem(ITEM::EXP2, 2);
							m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, 1);
							m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, 1);
							m_pInven->AddItem(ITEM::CASKET, CurrentOwn / NeedNum);
							m_pInven->EraseItem(CInventory::INVEN_MATERIAL, 4, NeedNum * (CurrentOwn / NeedNum));
							CurrentOwn = CurrentOwn - (ItemNum * NeedNum);
						}
					}
					else
					{
						ColorM(&m_FinalList[27], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
						ColorM(&m_FinalList[28], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					}
				}
				if (m_bRewardReceived[1])
				{
					if (false == m_RewardboxRender)
					{
						if (MinusAlphaW(&m_FinalList, TimeDelta))
						{
							m_CancelMsgbox = true;
						}
					}

				}

			}
			else
			{

				for (_int i = 0; i < 12; ++i)
				{
					m_MessageList[i].bRender = true;
				}

				MinusAlphaW(&m_MessageList, TimeDelta);
			}

			//클릭체크 m_bRewardReceived[0] 참거짓
			if (m_bRewardReceived[1])// 수령버튼 클릭
			{
				// 받은 상태
				m_DeliverList[0].iTexNum = 56;
				m_DeliverList[2].iTexNum = 57;
				m_DeliverList[2].bRender = true;

				m_0RewardList[3].bRender = true;
				m_0RewardList[4].bRender = true;
				m_0RewardList[4].iTexNum = 92;
				m_1RewardList[3].bRender = true;
				m_1RewardList[4].bRender = true;
				m_1RewardList[4].iTexNum = 92;
				m_2RewardList[3].bRender = true;
				m_2RewardList[4].bRender = true;
				m_2RewardList[4].iTexNum = 92;
				m_3RewardList[3].bRender = true;
				m_3RewardList[4].bRender = true;
				m_3RewardList[4].iTexNum = 92;
				m_4RewardList[3].bRender = true;
				m_4RewardList[4].bRender = true;
				m_4RewardList[4].iTexNum = 92;
				m_5RewardList[3].bRender = true;
				m_5RewardList[4].bRender = true;
				m_5RewardList[4].iTexNum = 92;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = false;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = false;
				}
			}
			else // 미클릭
			{
				// 받을 수 있지만 아직 안 받은 상태
				m_DeliverList[2].bRender = false;

				m_0RewardList[3].bRender = false;
				m_0RewardList[4].bRender = false;
				m_1RewardList[3].bRender = false;
				m_1RewardList[4].bRender = false;
				m_2RewardList[3].bRender = false;
				m_2RewardList[4].bRender = false;
				m_3RewardList[3].bRender = false;
				m_3RewardList[4].bRender = false;
				m_4RewardList[3].bRender = false;
				m_4RewardList[4].bRender = false;
				m_5RewardList[3].bRender = false;
				m_5RewardList[4].bRender = false;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = true;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = true;
				}
			}
		}
		break;
		case 30:
		{
			if ((false == m_bRewardReceived[2]) && (m_PlayerCurrentLevel >= SettingLevel) && (CurrentOwn >= NeedNum))
			{
				if (SelectUI(&m_DeliverList[0])) // 버튼 누르면
				{
					m_DeliverList[0].iTexNum = 54;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_MsgboxRender = true;
					}
				}
				else
					m_DeliverList[0].iTexNum = 55;
			}
			else
				m_DeliverList[0].iTexNum = 56;

			if (m_MsgboxRender)
			{
				if (false == m_RewardboxRender) // 확인창이 켜져있으면 이전 창의 버튼 입력은 무시됨
				{
					if (SelectUI(&m_MessageList[1])) // 취소 누르면
					{
						m_MessageList[1].iTexNum = 12;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[1].iTexNum = 11;
					if (SelectUI(&m_MessageList[4])) // 취소 누르면
					{
						m_MessageList[4].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[4].iTexNum = 55;

					if (SelectUI(&m_MessageList[5])) // 확인 누르면
					{
						m_MessageList[5].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = true;
							ItemNum = CurrentOwn / NeedNum; // 임시 설정
						}
					}
					else
						m_MessageList[5].iTexNum = 55;
				}

				if (false == m_CancelMsgbox)
					AddAlphaW(&m_MessageList, TimeDelta);

				if (m_CancelMsgbox)
				{
					if (MinusAlphaW(&m_MessageList, TimeDelta * 1.2f))
					{
						m_MsgboxRender = false;
						m_CancelMsgbox = false;
					}
				}



				if (m_RewardboxRender) // 확인버튼 누르면 보상 확인 창이 뜸
				{
					for (_int i = 1; i < 12; ++i)
					{
						m_MessageList[i].bRender = false; // 나갈때 켜줘야하며
					}

					AddAlphaW(&m_FinalList, TimeDelta);

					if (SelectUI(&m_FinalList[27])) // confrim 누르면
					{
						ColorP(&m_FinalList[27], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						ColorP(&m_FinalList[28], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = false;
							m_bRewardReceived[2] = true;
							m_MessageList[0].bRender = false;

							// 보상 인벤에 넣기
							m_pInven->AddItem(ITEM::TACTITE_COIN, 64000);
							m_pInven->AddItem(ITEM::EXP0, 8);
							m_pInven->AddItem(ITEM::EXP1, 3);
							m_pInven->AddItem(ITEM::EXP2, 2);
							m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, 2);
							m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, 2);
							m_pInven->AddItem(ITEM::CASKET, CurrentOwn / NeedNum);
							m_pInven->EraseItem(CInventory::INVEN_MATERIAL, 4, NeedNum * (CurrentOwn / NeedNum));
							CurrentOwn = CurrentOwn - (ItemNum * NeedNum);
						}
					}
					else
					{
						ColorM(&m_FinalList[27], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
						ColorM(&m_FinalList[28], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					}
				}
				if (m_bRewardReceived[2])
				{
					if (false == m_RewardboxRender)
					{
						if (MinusAlphaW(&m_FinalList, TimeDelta))
						{
							m_CancelMsgbox = true;
						}
					}

				}

			}
			else
			{

				for (_int i = 0; i < 12; ++i)
				{
					m_MessageList[i].bRender = true;
				}

				MinusAlphaW(&m_MessageList, TimeDelta);
			}
			//클릭체크 m_bRewardReceived[0] 참거짓
			if (m_bRewardReceived[2])// 수령버튼 클릭
			{
				// 받은 상태
				m_DeliverList[0].iTexNum = 56;
				m_DeliverList[2].iTexNum = 57;
				m_DeliverList[2].bRender = true;

				m_0RewardList[3].bRender = true;
				m_0RewardList[4].bRender = true;
				m_0RewardList[4].iTexNum = 92;
				m_1RewardList[3].bRender = true;
				m_1RewardList[4].bRender = true;
				m_1RewardList[4].iTexNum = 92;
				m_2RewardList[3].bRender = true;
				m_2RewardList[4].bRender = true;
				m_2RewardList[4].iTexNum = 92;
				m_3RewardList[3].bRender = true;
				m_3RewardList[4].bRender = true;
				m_3RewardList[4].iTexNum = 92;
				m_4RewardList[3].bRender = true;
				m_4RewardList[4].bRender = true;
				m_4RewardList[4].iTexNum = 92;
				m_5RewardList[3].bRender = true;
				m_5RewardList[4].bRender = true;
				m_5RewardList[4].iTexNum = 92;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = false;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = false;
				}
			}
			else // 미클릭
			{
				// 받을 수 있지만 아직 안 받은 상태
				m_DeliverList[2].bRender = false;

				m_0RewardList[3].bRender = false;
				m_0RewardList[4].bRender = false;
				m_1RewardList[3].bRender = false;
				m_1RewardList[4].bRender = false;
				m_2RewardList[3].bRender = false;
				m_2RewardList[4].bRender = false;
				m_3RewardList[3].bRender = false;
				m_3RewardList[4].bRender = false;
				m_4RewardList[3].bRender = false;
				m_4RewardList[4].bRender = false;
				m_5RewardList[3].bRender = false;
				m_5RewardList[4].bRender = false;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = true;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = true;
				}
			}
		}
		break;
		case 40:
		{
			if ((false == m_bRewardReceived[3]) && (m_PlayerCurrentLevel >= SettingLevel) && (CurrentOwn >= NeedNum))
			{
				if (SelectUI(&m_DeliverList[0])) // 버튼 누르면
				{
					m_DeliverList[0].iTexNum = 54;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_MsgboxRender = true;
					}
				}
				else
					m_DeliverList[0].iTexNum = 55;
			}
			else
				m_DeliverList[0].iTexNum = 56;

			if (m_MsgboxRender)
			{
				if (false == m_RewardboxRender) // 확인창이 켜져있으면 이전 창의 버튼 입력은 무시됨
				{
					if (SelectUI(&m_MessageList[1])) // 취소 누르면
					{
						m_MessageList[1].iTexNum = 12;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[1].iTexNum = 11;
					if (SelectUI(&m_MessageList[4])) // 취소 누르면
					{
						m_MessageList[4].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[4].iTexNum = 55;

					if (SelectUI(&m_MessageList[5])) // 확인 누르면
					{
						m_MessageList[5].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = true;
							ItemNum = CurrentOwn / NeedNum; // 임시 설정
						}
					}
					else
						m_MessageList[5].iTexNum = 55;
				}

				if (false == m_CancelMsgbox)
					AddAlphaW(&m_MessageList, TimeDelta);

				if (m_CancelMsgbox)
				{
					if (MinusAlphaW(&m_MessageList, TimeDelta * 1.2f))
					{
						m_MsgboxRender = false;
						m_CancelMsgbox = false;
					}
				}



				if (m_RewardboxRender) // 확인버튼 누르면 보상 확인 창이 뜸
				{
					for (_int i = 1; i < 12; ++i)
					{
						m_MessageList[i].bRender = false; // 나갈때 켜줘야하며
					}

					AddAlphaW(&m_FinalList, TimeDelta);

					if (SelectUI(&m_FinalList[27])) // confrim 누르면
					{
						ColorP(&m_FinalList[27], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						ColorP(&m_FinalList[28], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = false;
							m_bRewardReceived[3] = true;
							m_MessageList[0].bRender = false;

							// 보상 인벤에 넣기
							m_pInven->AddItem(ITEM::TACTITE_COIN, 106000);
							m_pInven->AddItem(ITEM::EXP0, 10);
							m_pInven->AddItem(ITEM::EXP1, 6);
							m_pInven->AddItem(ITEM::EXP2, 3);
							m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, 3);
							m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, 2);
							m_pInven->AddItem(ITEM::CASKET, CurrentOwn / NeedNum);
							m_pInven->EraseItem(CInventory::INVEN_MATERIAL, 4, NeedNum * (CurrentOwn / NeedNum));
							CurrentOwn = CurrentOwn - (ItemNum * NeedNum); // 인벤토리 연결하고나면 지우기
						}
					}
					else
					{
						ColorM(&m_FinalList[27], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
						ColorM(&m_FinalList[28], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					}
				}
				if (m_bRewardReceived[3])
				{
					if (false == m_RewardboxRender)
					{
						if (MinusAlphaW(&m_FinalList, TimeDelta))
						{
							m_CancelMsgbox = true;
						}
					}

				}

			}
			else
			{

				for (_int i = 0; i < 12; ++i)
				{
					m_MessageList[i].bRender = true;
				}

				MinusAlphaW(&m_MessageList, TimeDelta);
			}
			//클릭체크 m_bRewardReceived[0] 참거짓
			if (m_bRewardReceived[3])// 수령버튼 클릭
			{
				// 받은 상태
				m_DeliverList[0].iTexNum = 56;
				m_DeliverList[2].iTexNum = 57;
				m_DeliverList[2].bRender = true;

				m_0RewardList[3].bRender = true;
				m_0RewardList[4].bRender = true;
				m_0RewardList[4].iTexNum = 92;
				m_1RewardList[3].bRender = true;
				m_1RewardList[4].bRender = true;
				m_1RewardList[4].iTexNum = 92;
				m_2RewardList[3].bRender = true;
				m_2RewardList[4].bRender = true;
				m_2RewardList[4].iTexNum = 92;
				m_3RewardList[3].bRender = true;
				m_3RewardList[4].bRender = true;
				m_3RewardList[4].iTexNum = 92;
				m_4RewardList[3].bRender = true;
				m_4RewardList[4].bRender = true;
				m_4RewardList[4].iTexNum = 92;
				m_5RewardList[3].bRender = true;
				m_5RewardList[4].bRender = true;
				m_5RewardList[4].iTexNum = 92;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = false;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = false;
				}
			}
			else // 미클릭
			{
				// 받을 수 있지만 아직 안 받은 상태
				m_DeliverList[2].bRender = false;

				m_0RewardList[3].bRender = false;
				m_0RewardList[4].bRender = false;
				m_1RewardList[3].bRender = false;
				m_1RewardList[4].bRender = false;
				m_2RewardList[3].bRender = false;
				m_2RewardList[4].bRender = false;
				m_3RewardList[3].bRender = false;
				m_3RewardList[4].bRender = false;
				m_4RewardList[3].bRender = false;
				m_4RewardList[4].bRender = false;
				m_5RewardList[3].bRender = false;
				m_5RewardList[4].bRender = false;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = true;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = true;
				}
			}
		}
		break;
		case 50:
		{
			if ((false == m_bRewardReceived[4]) && (m_PlayerCurrentLevel >= SettingLevel) && (CurrentOwn >= NeedNum))
			{
				if (SelectUI(&m_DeliverList[0])) // 버튼 누르면
				{
					m_DeliverList[0].iTexNum = 54;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_MsgboxRender = true;
					}
				}
				else
					m_DeliverList[0].iTexNum = 55;
			}
			else
				m_DeliverList[0].iTexNum = 56;


			if (m_MsgboxRender)
			{
				if (false == m_RewardboxRender) // 확인창이 켜져있으면 이전 창의 버튼 입력은 무시됨
				{
					if (SelectUI(&m_MessageList[1])) // 취소 누르면
					{
						m_MessageList[1].iTexNum = 12;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[1].iTexNum = 11;
					if (SelectUI(&m_MessageList[4])) // 취소 누르면
					{
						m_MessageList[4].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_CancelMsgbox = true;
						}
					}
					else
						m_MessageList[4].iTexNum = 55;

					if (SelectUI(&m_MessageList[5])) // 확인 누르면
					{
						m_MessageList[5].iTexNum = 54;
						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = true;
							ItemNum = CurrentOwn / NeedNum; // 임시 설정
						}
					}
					else
						m_MessageList[5].iTexNum = 55;
				}

				if (false == m_CancelMsgbox)
					AddAlphaW(&m_MessageList, TimeDelta);

				if (m_CancelMsgbox)
				{
					if (MinusAlphaW(&m_MessageList, TimeDelta * 1.2f))
					{
						m_MsgboxRender = false;
						m_CancelMsgbox = false;
					}
				}



				if (m_RewardboxRender) // 확인버튼 누르면 보상 확인 창이 뜸
				{
					for (_int i = 1; i < 12; ++i)
					{
						m_MessageList[i].bRender = false;
					}

					AddAlphaW(&m_FinalList, TimeDelta);

					if (SelectUI(&m_FinalList[27])) // confrim 누르면
					{
						ColorP(&m_FinalList[27], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
						ColorP(&m_FinalList[28], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

						if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						{
							m_RewardboxRender = false;
							m_bRewardReceived[4] = true;
							m_MessageList[0].bRender = false;

							// 보상 인벤에 넣기
							m_pInven->AddItem(ITEM::TACTITE_COIN, 169000);
							m_pInven->AddItem(ITEM::EXP0, 10);
							m_pInven->AddItem(ITEM::EXP1, 8);
							m_pInven->AddItem(ITEM::EXP2, 5);
							m_pInven->AddItem(ITEM::COMMEMORATIVE_COIN, 5);
							m_pInven->AddItem(ITEM::TACTREITE_VOUCHER, 3);
							m_pInven->AddItem(ITEM::CASKET, CurrentOwn / NeedNum);
							m_pInven->EraseItem(CInventory::INVEN_MATERIAL, 4, NeedNum * (CurrentOwn / NeedNum));
							CurrentOwn = CurrentOwn - (ItemNum * NeedNum);
						}
					}
					else
					{
						ColorM(&m_FinalList[27], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
						ColorM(&m_FinalList[28], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					}
				}
				if (m_bRewardReceived[4])
				{
					if (false == m_RewardboxRender)
					{
						if (MinusAlphaW(&m_FinalList, TimeDelta))
						{
							m_CancelMsgbox = true;
						}
					}
				}
			}
			else
			{
				for (_int i = 0; i < 12; ++i)
				{
					m_MessageList[i].bRender = true;
				}

				MinusAlphaW(&m_MessageList, TimeDelta);
			}

			// 레벨 옮길 때 변수들 다 초기화 보상수령 변수는 초기화x



			//클릭체크 m_bRewardReceived[0] 참거짓
			if (m_bRewardReceived[4])// 수령버튼 클릭
			{
				// 받은 상태
				m_DeliverList[0].iTexNum = 56;
				m_DeliverList[2].iTexNum = 57;
				m_DeliverList[2].bRender = true;

				m_0RewardList[3].bRender = true;
				m_0RewardList[4].bRender = true;
				m_0RewardList[4].iTexNum = 92;
				m_1RewardList[3].bRender = true;
				m_1RewardList[4].bRender = true;
				m_1RewardList[4].iTexNum = 92;
				m_2RewardList[3].bRender = true;
				m_2RewardList[4].bRender = true;
				m_2RewardList[4].iTexNum = 92;
				m_3RewardList[3].bRender = true;
				m_3RewardList[4].bRender = true;
				m_3RewardList[4].iTexNum = 92;
				m_4RewardList[3].bRender = true;
				m_4RewardList[4].bRender = true;
				m_4RewardList[4].iTexNum = 92;
				m_5RewardList[3].bRender = true;
				m_5RewardList[4].bRender = true;
				m_5RewardList[4].iTexNum = 92;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = false;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = false;
				}
			}
			else // 미클릭
			{
				// 받을 수 있지만 아직 안 받은 상태	
				m_DeliverList[2].bRender = false;

				m_0RewardList[3].bRender = false;
				m_0RewardList[4].bRender = false;
				m_1RewardList[3].bRender = false;
				m_1RewardList[4].bRender = false;
				m_2RewardList[3].bRender = false;
				m_2RewardList[4].bRender = false;
				m_3RewardList[3].bRender = false;
				m_3RewardList[4].bRender = false;
				m_4RewardList[3].bRender = false;
				m_4RewardList[4].bRender = false;
				m_5RewardList[3].bRender = false;
				m_5RewardList[4].bRender = false;
				for (auto& casket : m_CasketList)
				{
					casket.bRender = true;
				}
				for (auto& own : m_OwnList)
				{
					own.bRender = true;
				}
			}
		}
		break;
		}

		if (m_PlayerCurrentLevel < SettingLevel) // 셋팅레벨이 더 노을 때 무조건 잠김
		{
			m_DeliverList[0].iTexNum = 56;
			m_DeliverList[2].iTexNum = 49;
			m_DeliverList[2].bRender = true;

			m_0RewardList[3].bRender = true;
			m_0RewardList[4].iTexNum = 14;
			m_0RewardList[4].bRender = true;
			m_1RewardList[3].bRender = true;
			m_1RewardList[4].iTexNum = 14;
			m_1RewardList[4].bRender = true;
			m_2RewardList[3].bRender = true;
			m_2RewardList[4].iTexNum = 14;
			m_2RewardList[4].bRender = true;
			m_3RewardList[3].bRender = true;
			m_3RewardList[4].iTexNum = 14;
			m_3RewardList[4].bRender = true;
			m_4RewardList[3].bRender = true;
			m_4RewardList[4].iTexNum = 14;
			m_4RewardList[4].bRender = true;
			m_5RewardList[3].bRender = true;
			m_5RewardList[4].iTexNum = 14;
			m_5RewardList[4].bRender = true;
			for (auto& casket : m_CasketList)
			{
				casket.bRender = false;
			}
			for (auto& own : m_OwnList)
			{
				own.bRender = false;
			}
		}
		m_0RewardList[6].bRender = SelectUI(&m_0RewardList[0]) ? true : false;
		m_1RewardList[6].bRender = SelectUI(&m_1RewardList[0]) ? true : false;
		m_2RewardList[6].bRender = SelectUI(&m_2RewardList[0]) ? true : false;
		m_3RewardList[6].bRender = SelectUI(&m_3RewardList[0]) ? true : false;
		m_4RewardList[6].bRender = SelectUI(&m_4RewardList[0]) ? true : false;
		m_5RewardList[6].bRender = SelectUI(&m_5RewardList[0]) ? true : false;


		if (pGameInstance->InputKey(DIK_Q) == KEY_STATE::TAP)
		{
			if (50 == SettingLevel)
			{
				m_RButtonList[0].bRender = true;
				m_RButtonList[1].bRender = true;
			}

			SettingLevel -= 10;
			m_CircleLevDown = true;
			m_CircleSLevUp = false;

			// 메세지창들 변수 리셋

			m_MsgboxRender = false;
			m_RewardboxRender = false;
			m_CancelMsgbox = false;


		}
		if (SelectUI(&m_LButtonList[0]))
		{
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (50 == SettingLevel)
				{
					m_RButtonList[0].bRender = true;
					m_RButtonList[1].bRender = true;
				}

				SettingLevel -= 10;
				m_CircleLevDown = true;
				m_CircleSLevUp = false;


				// 메세지창들 변수 리셋
				m_MsgboxRender = false;
				m_RewardboxRender = false;
				m_CancelMsgbox = false;
			}
		}
		if (10.f >= SettingLevel)
		{
			SettingLevel = 10;
			m_LButtonList[0].bRender = false;
			m_LButtonList[1].bRender = false;
		}



		/*
		레벨 플러스
		- e 입력 v
		- 마우스 입력 v
		- 설정 레벨이 50보다 클 때 랜더, 입력off   10, 20, 30, 40, 50 v
		- 박스 잠김 v
		- 현재 레벨 보다 클 때 레벨 미달성 문구 v
		- 보상 목록이 바뀜
		*/


		if (pGameInstance->InputKey(DIK_E) == KEY_STATE::TAP)
		{
			if (10 == SettingLevel)
			{
				m_LButtonList[0].bRender = true;
				m_LButtonList[1].bRender = true;
			}

			SettingLevel += 10;
			m_CircleSLevUp = true;
			m_CircleLevDown = false;


			// 메세지창들 변수 리셋
			m_MsgboxRender = false;
			m_RewardboxRender = false;
			m_CancelMsgbox = false;
		}
		if (SelectUI(&m_RButtonList[0]))
		{
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (10 == SettingLevel)
				{
					m_LButtonList[0].bRender = true;
					m_LButtonList[1].bRender = true;
				}
				SettingLevel += 10;
				m_CircleSLevUp = true;
				m_CircleLevDown = false;

				// 메세지창들 변수 리셋
				m_MsgboxRender = false;
				m_RewardboxRender = false;
				m_CancelMsgbox = false;
			}
		}
		if (50.f <= SettingLevel)
		{
			SettingLevel = 50;
			m_RButtonList[0].bRender = false;
			m_RButtonList[1].bRender = false;
		}



		// 창 닫힐 때 전체 랜더off
		if (SelectUI(&m_CommonList[10])) // 별 닫기
		{
			m_CommonList[10].iTexNum = 12;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_CommonList[10].OnRect = true;
			}
		}
		else
			m_CommonList[10].iTexNum = 11;

		if (true == m_CommonList[10].OnRect)
		{
			if (InMenuEnd(TimeDelta))
			{
				Situation = CUI_MerchantMen::BYE;
				m_InMenuRnderStart = true; //리셋
				m_CommonList[10].OnRect = false;
				SubDegree = 0.f;
				Degree = 0.f;
			}
		}
	}
	break;
	case Client::CUI_MerchantMen::BYE:
	{
		m_pUIMouse->Set_RenderMouse(false);
		m_bMouseActive = false;
		SetState(DISABLE);
	}
	}


}


void CUI_MerchantMen::InMenuOpen(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (AddAlphaW(&m_CommonList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_LevelList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_CircleList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_LButtonList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_RButtonList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_CasketList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_OwnList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_DeliverList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_0RewardList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_1RewardList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_2RewardList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_3RewardList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_4RewardList, TimeDelta))
		++Count;

	if (AddAlphaW(&m_5RewardList, TimeDelta))
		++Count;

	if (14 == Count)
		m_InMenuRnderStart = false;
}

_bool CUI_MerchantMen::InMenuEnd(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (MinusAlphaW(&m_CommonList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_LevelList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_CircleList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_LButtonList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_RButtonList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_CasketList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_OwnList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_DeliverList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_0RewardList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_1RewardList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_2RewardList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_3RewardList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_4RewardList, TimeDelta))
		++Count;

	if (MinusAlphaW(&m_5RewardList, TimeDelta))
		++Count;

	if (14 == Count)
		return true;
	else
		return false;
}

void CUI_MerchantMen::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI_POST, this);
}

HRESULT CUI_MerchantMen::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;
	switch (Situation)
	{
	case Client::CUI_MerchantMen::MEET:
	{
		for (_uint i = 0; i < (_int)m_MerchantList.size(); ++i)
		{
			if (m_MerchantList[i].bRender)
			{
				if (FAILED(Setup_MerchantShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_MerchantMen::MENU:
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
	case Client::CUI_MerchantMen::INMENU:
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

		for (_uint i = 0; i < (_int)m_0RewardList.size(); ++i)
		{
			if (m_0RewardList[i].bRender)
			{
				if (FAILED(Setup_0RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_1RewardList.size(); ++i)
		{
			if (m_1RewardList[i].bRender)
			{
				if (FAILED(Setup_1RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_2RewardList.size(); ++i)
		{
			if (m_2RewardList[i].bRender)
			{
				if (FAILED(Setup_2RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_3RewardList.size(); ++i)
		{
			if (m_3RewardList[i].bRender)
			{
				if (FAILED(Setup_3RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_4RewardList.size(); ++i)
		{
			if (m_4RewardList[i].bRender)
			{
				if (FAILED(Setup_4RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_5RewardList.size(); ++i)
		{
			if (m_5RewardList[i].bRender)
			{
				if (FAILED(Setup_5RewardShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}


		for (_uint i = 0; i < (_int)m_LevelList.size(); ++i)
		{
			if (m_LevelList[i].bRender)
			{
				if (FAILED(Setup_LevelShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_CircleList.size(); ++i)
		{
			if (m_CircleList[i].bRender)
			{
				if (FAILED(Setup_CircleShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_LButtonList.size(); ++i)
		{
			if (m_LButtonList[i].bRender)
			{
				if (FAILED(Setup_LButtonShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_RButtonList.size(); ++i)
		{
			if (m_RButtonList[i].bRender)
			{
				if (FAILED(Setup_RButtonShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_CasketList.size(); ++i)
		{
			if (m_CasketList[i].bRender)
			{
				if (FAILED(Setup_CasketShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_OwnList.size(); ++i)
		{
			if (m_OwnList[i].bRender)
			{
				if (FAILED(Setup_OwnShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_DeliverList.size(); ++i)
		{
			if (m_DeliverList[i].bRender)
			{
				if (FAILED(Setup_DeliverShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_MessageList.size(); ++i)
		{
			if (m_MessageList[i].bRender)
			{
				if (FAILED(Setup_MessageShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}

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
	}



	return S_OK;
}

void CUI_MerchantMen::RenderGUI()
{
}

_bool CUI_MerchantMen::SelectUI(MERMENDESC* pDesc)
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

HRESULT CUI_MerchantMen::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIMERCHANT,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_MerchantMen::Setup_MerchantShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_MerchantList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_MerchantList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_MerchantList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_MerchantList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_MerchantList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_MerchantList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_MerchantList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_MenuShader(_uint index)
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
_bool CUI_MerchantMen::AddAlpha(MERMENDESC* pDesc, _double TimeDelta)
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

_bool CUI_MerchantMen::MinusAlpha(MERMENDESC* pDesc, _double TimeDelta)
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

_bool CUI_MerchantMen::AddAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta)
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

_bool CUI_MerchantMen::MinusAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta)
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

void CUI_MerchantMen::ColorP(MERMENDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_MerchantMen::ColorM(MERMENDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_MerchantMen::Load()
{
	//
	_uint index = 3;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/MerchantMen%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_MerchantList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 6;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Menu%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Common%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_CommonList.push_back(Desc);
		CloseHandle(hFile);

	}


	index = 2;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/level%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_LevelList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 2;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/circle%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_CircleList.push_back(Desc);
		CloseHandle(hFile);

	}


	//
	index = 2;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/LButton%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_LButtonList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 2;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/RButton%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_RButtonList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/casket%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_CasketList.push_back(Desc);
		CloseHandle(hFile);

	}


	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/own%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_OwnList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 3;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/deliver%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_DeliverList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_0RewardList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_1RewardList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_2RewardList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/3reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_3RewardList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/4reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_4RewardList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/5reward%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		m_5RewardList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 12;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Message%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		i == 0 ? Desc.iPass = 19 : i == 2 ? Desc.iPass = 19 : i == 3 ? Desc.iPass = 19 : Desc.iPass = 21;
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_MessageList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 30;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Final%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MERMENDESC Desc;
		ZeroMemory(&Desc, sizeof(MERMENDESC));

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
		i == 0 ? Desc.iPass = 19 : i == 1 ? Desc.iPass = 19 : Desc.iPass = 21;
		Desc.bRender = true;
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_FinalList.push_back(Desc);
		CloseHandle(hFile);

	}
}








void CUI_MerchantMen::Save()
{
	//
	_uint index = 0;
	for (auto& Desc : m_MerchantList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/MerchantMen%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Menu%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Common%d.dat", index);
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


	index = 0;
	for (auto& Desc : m_LevelList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/level%d.dat", index);
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
	for (auto& Desc : m_CircleList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/circle%d.dat", index);
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
	for (auto& Desc : m_LButtonList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/LButton%d.dat", index);
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
	for (auto& Desc : m_RButtonList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/RButton%d.dat", index);
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
	for (auto& Desc : m_CasketList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/casket%d.dat", index);
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
	for (auto& Desc : m_OwnList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/own%d.dat", index);
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
	for (auto& Desc : m_DeliverList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/deliver%d.dat", index);
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
		++index;
	}

	//
	index = 0;
	for (auto& Desc : m_0RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0reward%d.dat", index);
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
	for (auto& Desc : m_1RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1reward%d.dat", index);
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
	for (auto& Desc : m_2RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2reward%d.dat", index);
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
	for (auto& Desc : m_3RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/3reward%d.dat", index);
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
	for (auto& Desc : m_4RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/4reward%d.dat", index);
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
	for (auto& Desc : m_5RewardList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/5reward%d.dat", index);
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

	index = 0;
	for (auto& Desc : m_MessageList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Message%d.dat", index);
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

	index = 0;
	for (auto& Desc : m_FinalList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Final%d.dat", index);
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

HRESULT CUI_MerchantMen::Setup_CommonShader(_uint index)
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

HRESULT CUI_MerchantMen::Setup_LevelShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_LevelList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_LevelList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_LevelList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_LevelList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_LevelList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_LevelList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_LevelList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_CircleShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CircleList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CircleList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CircleList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CircleList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CircleList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CircleList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_CircleList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_LButtonShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_LButtonList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_LButtonList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_LButtonList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_LButtonList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_LButtonList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_LButtonList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_LButtonList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_RButtonShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_RButtonList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_RButtonList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_RButtonList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_RButtonList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_RButtonList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_RButtonList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_RButtonList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_CasketShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_CasketList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_CasketList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_CasketList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_CasketList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_CasketList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_CasketList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_CasketList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_OwnShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_OwnList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_OwnList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_OwnList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_OwnList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_OwnList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_OwnList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_OwnList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_DeliverShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DeliverList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DeliverList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DeliverList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DeliverList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DeliverList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DeliverList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_DeliverList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_0RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_0RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_0RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_0RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_0RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_0RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_0RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_0RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_1RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_1RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_1RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_1RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_1RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_1RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_1RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_1RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_2RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_2RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_2RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_2RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_2RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_2RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_2RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_2RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_3RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_3RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_3RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_3RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_3RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_3RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_3RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_3RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_4RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_4RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_4RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_4RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_4RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_4RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_4RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_4RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_5RewardShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_5RewardList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_5RewardList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_5RewardList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_5RewardList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_5RewardList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_5RewardList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_5RewardList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_MessageShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_MessageList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_MessageList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_MessageList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_MessageList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_MessageList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_MessageList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_MessageList[index].iPass;

	return S_OK;
}

HRESULT CUI_MerchantMen::Setup_FinalShader(_uint index)
{
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


CUI_MerchantMen* CUI_MerchantMen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MerchantMen* pInstance = new CUI_MerchantMen(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_MerchantMen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MerchantMen::Clone(void* pArg)
{
	CUI_MerchantMen* pInstance = new CUI_MerchantMen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_MerchantMen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MerchantMen::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}