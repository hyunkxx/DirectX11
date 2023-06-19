#include "stdafx.h"
#include "..\Public\UI_Cooking.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "Input_Device.h"
#include "UI_Mouse.h"
#include "PlayerState.h"
#include "ItemDB.h"
#include "Item.h"
#include "Inventory.h"

CUI_Cooking::CUI_Cooking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Cooking::CUI_Cooking(const CUI_Cooking& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Cooking::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cooking::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	color0 = SLOT_COLOR;
	color1 = SLOT_ADVANCED_COLOR;
	color2 = SLOT_RARE_COLOR;
	color3 = SLOT_UNIQUE_COLOR;
	color4 = SLOT_LEGEND_COLOR;


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	Load();

	// 위치 이동 임시코드
	for (_int i = 0; i < (_int)m_0Slot.size(); ++i)
	{
		m_3Slot[i] = m_0Slot[i];
		m_3Slot[i].fY = m_0Slot[i].fY - 135.f;
		XMStoreFloat4x4(&(m_3Slot[i].WorldMatrix), XMMatrixScaling(m_3Slot[i].fWidth, m_3Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_3Slot[i].fX, m_3Slot[i].fY, m_3Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_0Slot.size(); ++i)
	{
		m_6Slot[i] = m_0Slot[i];
		m_6Slot[i].fY = m_0Slot[i].fY - 135.f *2.f;

		XMStoreFloat4x4(&(m_6Slot[i].WorldMatrix), XMMatrixScaling(m_6Slot[i].fWidth, m_6Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_6Slot[i].fX, m_6Slot[i].fY, m_6Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_0Slot.size(); ++i)
	{
		m_9Slot[i] = m_0Slot[i];
		m_9Slot[i].fY = m_0Slot[i].fY - 135.f *3.f;

		XMStoreFloat4x4(&(m_9Slot[i].WorldMatrix), XMMatrixScaling(m_9Slot[i].fWidth, m_9Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_9Slot[i].fX, m_9Slot[i].fY, m_9Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_0Slot.size(); ++i)
	{
		m_12Slot[i] = m_0Slot[i];
		m_12Slot[i].fY = m_0Slot[i].fY - 135.f * 4.f;

		XMStoreFloat4x4(&(m_12Slot[i].WorldMatrix), XMMatrixScaling(m_12Slot[i].fWidth, m_12Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_12Slot[i].fX, m_12Slot[i].fY, m_12Slot[i].fZ));
	}
	//
	for (_int i = 0; i < (_int)m_1Slot.size(); ++i)
	{
		m_4Slot[i] = m_1Slot[i];
		m_4Slot[i].fY = m_1Slot[i].fY - 135.f;

		XMStoreFloat4x4(&(m_4Slot[i].WorldMatrix), XMMatrixScaling(m_4Slot[i].fWidth, m_4Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_4Slot[i].fX, m_4Slot[i].fY, m_4Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_1Slot.size(); ++i)
	{
		m_7Slot[i] = m_1Slot[i];
		m_7Slot[i].fY = m_1Slot[i].fY - 135.f *2.f;

		XMStoreFloat4x4(&(m_7Slot[i].WorldMatrix), XMMatrixScaling(m_7Slot[i].fWidth, m_7Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_7Slot[i].fX, m_7Slot[i].fY, m_7Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_1Slot.size(); ++i)
	{
		m_10Slot[i] = m_1Slot[i];
		m_10Slot[i].fY = m_1Slot[i].fY - 135.f *3.f;

		XMStoreFloat4x4(&(m_10Slot[i].WorldMatrix), XMMatrixScaling(m_10Slot[i].fWidth, m_10Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_10Slot[i].fX, m_10Slot[i].fY, m_10Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_1Slot.size(); ++i)
	{
		m_13Slot[i] = m_1Slot[i];
		m_13Slot[i].fY = m_1Slot[i].fY - 135.f * 4.f;

		XMStoreFloat4x4(&(m_13Slot[i].WorldMatrix), XMMatrixScaling(m_13Slot[i].fWidth, m_13Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_13Slot[i].fX, m_13Slot[i].fY, m_13Slot[i].fZ));
	}

	//
	for (_int i = 0; i < (_int)m_2Slot.size(); ++i)
	{
		m_5Slot[i] = m_2Slot[i];
		m_5Slot[i].fY = m_2Slot[i].fY - 135.f;

		XMStoreFloat4x4(&(m_5Slot[i].WorldMatrix), XMMatrixScaling(m_5Slot[i].fWidth, m_5Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_5Slot[i].fX, m_5Slot[i].fY, m_5Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_2Slot.size(); ++i)
	{
		m_8Slot[i] = m_2Slot[i];
		m_8Slot[i].fY = m_2Slot[i].fY - 135.f *2.f;

		XMStoreFloat4x4(&(m_8Slot[i].WorldMatrix), XMMatrixScaling(m_8Slot[i].fWidth, m_8Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_8Slot[i].fX, m_8Slot[i].fY, m_8Slot[i].fZ));
	}
	for (_int i = 0; i < (_int)m_2Slot.size(); ++i)
	{
		m_11Slot[i] = m_2Slot[i];
		m_11Slot[i].fY = m_2Slot[i].fY - 135.f *3.f;

		XMStoreFloat4x4(&(m_11Slot[i].WorldMatrix), XMMatrixScaling(m_11Slot[i].fWidth, m_11Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_11Slot[i].fX, m_11Slot[i].fY, m_11Slot[i].fZ));
	}
	pSelectSlot = &m_0Slot[2];
	pOwnFood = &InvenFlatbread;
	pNeedMaterial0 = &NeedMaterial0;
	pNeedMaterial1 = &NeedMaterial1;
	pNeedMaterial2 = &NeedMaterial2;
	pOwnMaterial0 = &InvenFlour;
	pOwnMaterial1 = &InvenRice;
	pOwnMaterial2 = &InvenTofu;

	m_RButtonList[0].fX += 10.f;
	// 배우기 텍스트 위치 ->456.f 원래 위치로 돌아올때 사용
	m_RButtonList[1].OriPos.x = m_RButtonList[1].fX = 438.f;
	m_FinalList[13].OriPos.x = m_FinalList[13].fX;

	m_0Slot[0].bRender = false;
	m_1Slot[0].bRender = false;
	m_2Slot[0].bRender = false;
	m_3Slot[0].bRender = false;
	m_4Slot[0].bRender = false;
	m_5Slot[0].bRender = false;
	m_6Slot[0].bRender = false;
	m_7Slot[0].bRender = false;
	m_8Slot[0].bRender = false;
	m_9Slot[0].bRender = false;
	m_10Slot[0].bRender = false;
	m_11Slot[0].bRender = false;
	m_12Slot[0].bRender = false;
	m_13Slot[0].bRender = false;

	return S_OK;
}

void CUI_Cooking::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));

	SetState(DISABLE);

	// 들어오기, 나가기 
	/* NPC랑 플레이어랑 충돌하면 Setstate()로 활성화 , 비활성화*/
}

void CUI_Cooking::Tick(_double TimeDelta)
{
	// 레시피 조건 추가해야함

	__super::Tick(TimeDelta);

	if (m_NPCbye)
		Set_END();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameMode* pGM = CGameMode::GetInstance();
	CCharacter* pActiveCharacter = m_pPlayerStateClass->Get_ActiveCharacter();

	switch (Situation)
	{
	case Client::CUI_Cooking::MEET:
	{
		m_pUIMouse->Set_RenderMouse(true);
		pGM->SetMouseActive(true);

		if (SelectUI(&m_Cookist[0]))
		{
			pActiveCharacter->Set_OnControl(false);
			if (false == m_Cookist[0].OnRect)
			{
				for (auto& Sou : m_Cookist)
				{
					Sou.fColorA = Sou.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				pGameInstance->PlaySoundEx(L"Play_AE_UI_But_Click.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 확인 클릭하는 소리
				m_Cookist[0].OnRect = true;
			}
		}

		if (true == m_Cookist[0].OnRect)
		{
			if (MinusAlphaW(&m_Cookist, TimeDelta))
			{
				m_Cookist[0].OnRect = false;
				Situation = CUI_Cooking::SETTINGNUM;
			}
		}
		else
		{
			AddAlphaW(&m_Cookist, TimeDelta);
		}
	}
	break;
	case Client::CUI_Cooking::SETTINGNUM:
	{
		//// 인벤토리에 있는 요리관련 아이템들 수량 가져오기
		//m_pInven->AddItem(ITEM::RECIPE0, 1);
		//m_pInven->AddItem(ITEM::RECIPE1, 1);
		//m_pInven->AddItem(ITEM::RICE, 5);
		//m_pInven->AddItem(ITEM::TOFU, 5);
		//m_pInven->AddItem(ITEM::FLOUR, 5);
		//m_pInven->AddItem(ITEM::MUSHROOM, 50);
		//m_pInven->AddItem(ITEM::CHICKEN, 5);
		//m_pInven->AddItem(ITEM::EGG, 5);
		//m_pInven->AddItem(ITEM::MEAT, 5);
		//m_pInven->AddItem(ITEM::HERB, 5);
		//m_pInven->AddItem(ITEM::VIOLET, 5);
		//m_pInven->AddItem(ITEM::DANDELION, 5);
		//m_pInven->AddItem(ITEM::PEPPER, 5);
		//m_pInven->AddItem(ITEM::SUGAR, 5);
		//m_pInven->AddItem(ITEM::SALT, 5);
		//m_pInven->AddItem(ITEM::OIL, 5);
		//m_pInven->AddItem(ITEM::SOYSAUCE, 5);
		InvenRice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		InvenTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenMushroom = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MUSHROOM);
		InvenChicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenHerb = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::HERB);
		InvenViolet = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::VIOLET);
		InvenDandelion = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::DANDELION);
		InvenPepper = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
		InvenSugar = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenSoysauce = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
		InvenRecipe0 = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE0);
		InvenRecipe1 = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE1);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FLATBREAD);
		InvenSalad = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALAD);
		InvenFriedTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDTOFU);
		InvenSaltedTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALTEDTEA);
		InvenHerbTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::HERBTEA);
		InvenDragonNoodle = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DRAGONNOODLE);
		InvenOmurice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::OMURICE);
		InvenFriedrice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDRICE);
		InvenFriedchicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDCHICKEN);
		InvenRabbitbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::RABBITBREAD);
		InvenFriedMushroom = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDMUSHROOM);
		InvenMapotofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::MAPOTOFU);
		InvenPorkBelly = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::PORKBELLY);
		InvenDucknoodles = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DUCKNOODLES);



		// 처음 메뉴 랜더는 등급: all , 디테일 : 노말의 첫번째 메뉴
		// 등급 all에 선택 텍스처 떠있고 나머지는 흰색
		// 스위치 등급 -> 슬롯마다 요리 텍스처 [2], 컬러 설정[1], 뉴[3]은 끄기 -> 레시피 해제됐을때만 킴
		SettingSlot();
		Situation = CUI_Cooking::MENU;
	}
	break;
	case Client::CUI_Cooking::MENU:
	{
		if (m_MenuRenderStart)
		{
			InMenuOpen(TimeDelta);
		}
		if (SelectUI(&m_CommonList[9]))
		{
			m_CommonList[9].iTexNum = 8;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				pGameInstance->PlaySoundEx(L"Play_AE_UI_But_Cancel.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 취소 클릭하는 소리

				pActiveCharacter->Set_OnControl(true);
				m_MenuOutStart = true;
			}
		}
		else
			m_CommonList[9].iTexNum = 7;

		if (m_MenuOutStart)
		{
			if (InMenuEnd(TimeDelta))
			{
				MakeNum = 1;
				m_MenuRenderStart = true;
				m_MenuOutStart = false;
				Situation = COOKEND;
				SetState(DISABLE);
				pGM->SetMouseActive(false);
				m_NPCbye = false;
			}
		}
		else // 나가고 있을 때가 아니라면
		{
			// 메뉴 누르면 디테일 변경
			SelectGreade();
			SettingSlot();
			SelectSlot();
			SettingNumbers();
			SettingTotal(MakeNum);
			// 레시피 사용 유무에 따라 우버튼 텍스트 사진이 바뀜 마스터 16번, 배우기 17번, 확인 26번
			if (ITEM::PORKBELLY == pSelectSlot->SlotItemId)
			{
				if (false == bUsedRecipe0)
				{
					m_RButtonList[1].iTexNum = 17;
					m_RButtonList[1].fX = 458.f;
					m_RButtonList[1].fY = -296.f;
					m_RButtonList[1].fWidth = 204.8f; m_RButtonList[1].fHeight = 25.6f;
				}
				else
				{
					m_RButtonList[1].iTexNum = 26;
					m_RButtonList[1].fX = 432.f;
					m_RButtonList[1].fY = -292.f;
					m_RButtonList[1].fWidth = 184.32f; m_RButtonList[1].fHeight = 23.04f;
				}
			}
			else if (ITEM::DUCKNOODLES == pSelectSlot->SlotItemId)
			{
				if (false == bUsedRecipe1)
				{
					m_RButtonList[1].iTexNum = 17;
					m_RButtonList[1].fX = 458.f;
					m_RButtonList[1].fY = -296.f;
					m_RButtonList[1].fWidth = 204.8f; m_RButtonList[1].fHeight = 25.6f;
				}
				else
				{
					m_RButtonList[1].iTexNum = 26;
					m_RButtonList[1].fX = 432.f;
					m_RButtonList[1].fY = -292.f;
					m_RButtonList[1].fWidth = 184.32f; m_RButtonList[1].fHeight = 23.04f;
				}
			}
			else
			{
				m_RButtonList[1].iTexNum = 26;
				m_RButtonList[1].fX = 432.f;
				m_RButtonList[1].fY = -292.f;
				m_RButtonList[1].fWidth = 184.32f; m_RButtonList[1].fHeight = 23.04f;
			}

			// 수량 조절 버튼-> 보유 수량봅다 많으면 빨간색
			if (SelectUI(&m_LButtonList[1]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					pGameInstance->PlaySoundEx(L"Play_ae_ui_but_count.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 수량 선택하는 소리

					if (1<MakeNum)
						--MakeNum;

					// 라스트  수량
					//SettingTotal(MakeNum);
				}
			}

			if (SelectUI(&m_LButtonList[2]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					pGameInstance->PlaySoundEx(L"Play_ae_ui_but_count.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 수량 선택하는 소리

					++MakeNum;
					// 라스트  수량
					//SettingTotal(MakeNum);
				}
			}

			if (bMake) // 보유 재고가 더 많으면
			{
				// 클릭 가능 
				if (SelectUI(&m_RButtonList[0]))
				{
					// 마우스 올리면 텍스처 밝아짐
					m_RButtonList[0].iTexNum = 20;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						pGameInstance->PlaySoundEx(L"Play_AE_UI_But_Click.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 확인 클릭하는 소리

						m_ReadyConfirmRender = true;
					}
				}
				else
					m_RButtonList[0].iTexNum = 21;
			}
			else
			{
				m_RButtonList[0].iTexNum = 22;
			}
		}
		if (m_ReadyConfirmRender)
		{
			if (InMenuEnd(TimeDelta))
			{
				m_ReadyConfirmRender = false;
				m_ConfirmRenderStart = true;
				Situation = CUI_Cooking::READYCONFRIM;
				pGameInstance->PlaySoundEx(L"Play_AE_UI_AreaReward_Get.wem.wav", SOUND_UI_FEEDBACK, VOLUME_VFX); // 물건 구매나 보상획득 소리
			}
		}
	}
	break;
	case Client::CUI_Cooking::READYCONFRIM:
	{
		// 라스트  수량
		SettingTotal(MakeNum);
		Situation = CUI_Cooking::CONFRIM;
	}
	case Client::CUI_Cooking::CONFRIM:
	{
		//레시피를 배웠다면
		if (ITEM::PORKBELLY == pSelectSlot->SlotItemId)
		{
			if (false == bUsedRecipe0)
			{
				m_FinalList[13].iTexNum = 16; //텍스트 마스터로 변경
				m_FinalList[13].fX = 14.f;
			}
			else
			{
				m_FinalList[13].iTexNum = 19;
				m_FinalList[13].fX = m_FinalList[13].OriPos.x;
			}

		}
		if (ITEM::DUCKNOODLES == pSelectSlot->SlotItemId)
		{
			if (false == bUsedRecipe1)
			{
				m_FinalList[13].iTexNum = 16;
				m_FinalList[13].fX = 14.f;
			}
			else
			{
				m_FinalList[13].iTexNum = 19;
				m_FinalList[13].fX = m_FinalList[13].OriPos.x;
			}
		}
		if (ITEM::DUCKNOODLES != pSelectSlot->SlotItemId && ITEM::PORKBELLY != pSelectSlot->SlotItemId)
		{
			//다른 메뉴들일때
			m_FinalList[13].iTexNum = 19;
			m_FinalList[13].fX = m_FinalList[13].OriPos.x;
		}

		if (m_ConfirmRenderStart)
			(AddAlphaW(&m_FinalList, TimeDelta) ? m_ConfirmRenderStart = false : m_ConfirmRenderStart = true);

		// 확인 누르면 목록으로 돌아가기, 색 변경
		if (SelectUI(&m_FinalList[10]))
		{
			ColorP(&m_FinalList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
			ColorP(&m_FinalList[11], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_Close_2.wem.wav", SOUND_UI_FEEDBACK, VOLUME_VFX); // 목록 닫는 소리, NPC나가는 소리

				Situation = CUI_Cooking::BYE;
				m_ConfirmRenderStart = true;
			}
		}
		else
		{
			ColorM(&m_FinalList[10], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
			ColorM(&m_FinalList[11], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
		}

	}
	break;

	case Client::CUI_Cooking::BYE:
	{
		MinusAlphaW(&m_FinalList, TimeDelta);
		m_pInven->AddItem((*pSelectSlot).SlotItemId, MakeNum); // 만든 요리

															   // 인벤에 아이템 넣기
															   //사용 재료 감소시키기
		switch ((*pSelectSlot).SlotItemId)
		{
		case ITEM::FLATBREAD:
		{
			m_pInven->AddItem(ITEM::FLOUR, -2 * MakeNum);
		}
		break;
		case ITEM::SALAD:
		{
			m_pInven->AddItem(ITEM::VIOLET, -2 * MakeNum);
		}
		break;
		case ITEM::FRIEDTOFU:
		{
			m_pInven->AddItem(ITEM::TOFU, -1 * MakeNum);
			m_pInven->AddItem(ITEM::OIL, -1 * MakeNum);
		}
		break;
		case ITEM::SALTEDTEA:
		{
			m_pInven->AddItem(ITEM::SALT, -3 * MakeNum);
		}
		break;
		case ITEM::HERBTEA:
		{
			m_pInven->AddItem(ITEM::HERB, -3 * MakeNum);
		}
		break;
		case ITEM::DRAGONNOODLE:
		{
			m_pInven->AddItem(ITEM::FLOUR, -2 * MakeNum);
			m_pInven->AddItem(ITEM::MEAT, -2 * MakeNum);
			m_pInven->AddItem(ITEM::SALT, -2 * MakeNum);
		}
		break;
		case ITEM::OMURICE:
		{
			m_pInven->AddItem(ITEM::RICE, -2 * MakeNum);
			m_pInven->AddItem(ITEM::EGG, -3 * MakeNum);
			m_pInven->AddItem(ITEM::SUGAR, -1 * MakeNum);
		}
		break;
		case ITEM::FRIEDRICE:
		{
			m_pInven->AddItem(ITEM::RICE, -2 * MakeNum);
			m_pInven->AddItem(ITEM::EGG, -2 * MakeNum);
			m_pInven->AddItem(ITEM::OIL, -2 * MakeNum);
		}
		break;
		case ITEM::FRIEDCHICKEN:
		{
			m_pInven->AddItem(ITEM::CHICKEN, -2 * MakeNum);
			m_pInven->AddItem(ITEM::SALT, -1 * MakeNum);
			m_pInven->AddItem(ITEM::OIL, -3 * MakeNum);
		}
		break;
		case ITEM::RABBITBREAD:
		{
			m_pInven->AddItem(ITEM::FLOUR, -5 * MakeNum);
			m_pInven->AddItem(ITEM::EGG, -3 * MakeNum);
			m_pInven->AddItem(ITEM::SUGAR, -3 * MakeNum);
		}
		break;
		case ITEM::FRIEDMUSHROOM:
		{
			m_pInven->AddItem(ITEM::MUSHROOM, -5 * MakeNum);
			m_pInven->AddItem(ITEM::OIL, -4 * MakeNum);
			m_pInven->AddItem(ITEM::SALT, -2 * MakeNum);
		}
		break;
		case ITEM::MAPOTOFU:
		{
			m_pInven->AddItem(ITEM::TOFU, -5 * MakeNum);
			m_pInven->AddItem(ITEM::PEPPER, -3 * MakeNum);
			m_pInven->AddItem(ITEM::MEAT, -3 * MakeNum);
		}
		break;
		case ITEM::PORKBELLY:
		{
			if (false == bUsedRecipe0)
			{
				bUsedRecipe0 = true;
				m_pInven->AddItem(ITEM::RECIPE0, -1);
				m_pInven->AddItem((*pSelectSlot).SlotItemId, -1);
			}
			else
			{
				m_pInven->AddItem(ITEM::MEAT, -5 * MakeNum);
				m_pInven->AddItem(ITEM::PEPPER, -2 * MakeNum);
				m_pInven->AddItem(ITEM::SOYSAUCE, -4 * MakeNum);
			}
		}
		break;
		case ITEM::DUCKNOODLES:
		{
			if (false == bUsedRecipe1)
			{
				bUsedRecipe1 = true;
				m_pInven->AddItem(ITEM::RECIPE1, -1);
				m_pInven->AddItem((*pSelectSlot).SlotItemId, -1);
			}
			else
			{
				m_pInven->AddItem(ITEM::CHICKEN, -5 * MakeNum);
				m_pInven->AddItem(ITEM::FLOUR, -2 * MakeNum);
				m_pInven->AddItem(ITEM::SOYSAUCE, -3 * MakeNum);
			}
		}
		break;
		}
		// 모든 변수 초기화
		Situation = CUI_Cooking::MENU;
		m_MenuRenderStart = true;
		m_ConfirmRenderStart = true;
		MakeNum = 1;
		bMake = false;
		GradeIndex = 0;
		pSelectSlot = &m_0Slot[2];
	}
	break;
	}
}


void CUI_Cooking::SettingNumbers()
{
	switch (pSelectSlot->SlotItemId)
	{
	case ITEM::FLATBREAD:
	{
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FLATBREAD);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅, 파이널 아이콘, 
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FLATBREAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FLATBREAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		//파이널 박스 색상 셋팅
		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));

		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_FLOUR;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		// 수량 비교 설정을 위한 사용하지 않는 변수들 설정
		NeedMaterial1 = 0 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;
		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		// 사용하지 않는 재료 슬롯에 대한 랜더설정
		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = false;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = false;
		}
		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenFlour;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		bMake = bEnoughMaterial0;
	}
	break;
	case ITEM::SALAD:
	{
		InvenViolet = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::VIOLET);
		InvenSalad = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALAD);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenSalad;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_SALAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_SALAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_VIOLET;
		m_0MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 0 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;
		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = false;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = false;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenViolet;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		bMake = bEnoughMaterial0;
	}
	break;
	case ITEM::FRIEDTOFU:
	{
		InvenTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenFriedTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDTOFU);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenFriedTofu;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDTOFU;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDTOFU;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_TOFU;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_OIL;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 1 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 1 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = false;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenTofu;
		pOwnMaterial1 = &InvenOil;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial1 < *pNeedMaterial1) // 살 수 없다
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1)
			bMake = true;
		else
			bMake = false;

	}
	break;
	case ITEM::SALTEDTEA:
	{
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenSaltedTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALTEDTEA);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenSaltedTea;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_SALTEDTEA;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_SALTEDTEA;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODADVANCED;

		m_FinalList[3].fColorR = -(255.f - (color1.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color1.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color1.z * 255.f));

		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SALT;
		m_0MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 3 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 0 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;
		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = false;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = false;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenSalt;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		bMake = bEnoughMaterial0;
	}
	break;
	case ITEM::HERBTEA:
	{
		InvenHerb = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::HERB);
		InvenHerbTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::HERBTEA);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenHerbTea;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_HERBTEA;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_HERBTEA;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODADVANCED;

		m_FinalList[3].fColorR = -(255.f - (color1.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color1.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color1.z * 255.f));

		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_HERB;
		m_0MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 3 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 0 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;
		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = false;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = false;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenHerb;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		bMake = bEnoughMaterial0;
	}
	break;
	case ITEM::DRAGONNOODLE:
	{
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenDragonNoodle = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DRAGONNOODLE);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenDragonNoodle;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_DRAGONNOODLE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_DRAGONNOODLE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));

		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_FLOUR;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_MEAT;
		m_1MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SALT;
		m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 2 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 2 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenFlour;
		pOwnMaterial1 = &InvenMeat;
		pOwnMaterial2 = &InvenSalt;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;

	}
	break;
	case ITEM::OMURICE:
	{
		InvenRice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenSugar = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		InvenOmurice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::OMURICE);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenOmurice;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_OMURICE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_OMURICE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_RICE;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_EGG;
		m_1MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SUGAR;
		m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 3 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 1 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenRice;
		pOwnMaterial1 = &InvenEgg;
		pOwnMaterial2 = &InvenSugar;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;
	}
	break;
	case ITEM::FRIEDRICE:
	{
		InvenRice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenFriedrice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDRICE);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenFriedrice;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDRICE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDRICE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_RICE;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_OIL;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_EGG;
		m_2MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 2 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 2 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenRice;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenEgg;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;

	}
	break;
	case ITEM::FRIEDCHICKEN:
	{
		InvenChicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenFriedchicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDCHICKEN);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenFriedchicken;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDCHICKEN;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDCHICKEN;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_CHICKEN;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_OIL;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SALT;
		m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 3 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 1 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenChicken;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenSalt;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;
	}
	break;
	case ITEM::RABBITBREAD:
	{
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenSugar = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		InvenRabbitbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::RABBITBREAD);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenRabbitbread;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_RABBITBREAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_RABBITBREAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_FLOUR;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_EGG;
		m_1MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SUGAR;
		m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 5 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 3 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 3 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenFlour;
		pOwnMaterial1 = &InvenEgg;
		pOwnMaterial2 = &InvenSugar;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;
	}
	break;
	case ITEM::FRIEDMUSHROOM:
	{
		InvenMushroom = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MUSHROOM);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenFriedMushroom = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDMUSHROOM);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenFriedMushroom;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDMUSHROOM;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDMUSHROOM;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));


		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_MUSHROOM;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_OIL;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SALT;
		m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 5 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 4 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 2 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenMushroom;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenSalt;

		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;
	}
	break;
	case ITEM::MAPOTOFU:
	{
		InvenTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		InvenPepper = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenMapotofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::MAPOTOFU);

		// 요리 현재보유 숫자 셋팅 Own
		pOwnFood = &InvenMapotofu;
		SettingOwnNum(pOwnFood);

		// 만들 요리 아이콘 셋팅
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_MAPOTOFU;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_MAPOTOFU;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));

		// 필요 재료 아이콘 셋팅
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_TOFU;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_PEPPER;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_MEAT;
		m_2MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_2MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_2MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		// 필요한 재료 셋팅
		NeedMaterial0 = 5 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		NeedMaterial1 = 3 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;

		NeedMaterial2 = 3 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		for (auto& Desc0 : m_1MaterialSlot)
		{
			Desc0.bRender = true;
		}
		for (auto& Desc1 : m_1MaterialSlot)
		{
			Desc1.bRender = true;
		}
		for (auto& Desc2 : m_2MaterialSlot)
		{
			Desc2.bRender = true;
		}

		// 필요 재료 숫자 셋팅
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// 보유 재료 셋팅
		pOwnMaterial0 = &InvenTofu;
		pOwnMaterial1 = &InvenPepper;
		pOwnMaterial2 = &InvenMeat;


		// 보유 재료 숫자 셋팅
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);

		// 구매할 수 있는지 bMake 체크
		if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
		{
			bEnoughMaterial0 = false;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial0 = true;
			m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
			m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
			m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

		}

		if (*pOwnMaterial1 < *pNeedMaterial1)
		{
			bEnoughMaterial1 = false;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
		}
		else
		{
			bEnoughMaterial1 = true;
			m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
			m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
			m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
		}

		if (*pOwnMaterial2 < *pNeedMaterial2)
		{
			bEnoughMaterial1 = false;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
		}
		else // 살 수 있다
		{
			bEnoughMaterial1 = true;
			m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
			m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
			m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
		}
		if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
			bMake = true;
		else
			bMake = false;
	}
	break;
	case ITEM::PORKBELLY:
	{

		if (bUsedRecipe0)
		{
			InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
			InvenPepper = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
			InvenSoysauce = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
			InvenPorkBelly = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::PORKBELLY);

			// 요리 현재보유 숫자 셋팅 Own
			pOwnFood = &InvenPorkBelly;
			SettingOwnNum(pOwnFood);

			// 만들 요리 아이콘 셋팅
			m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_PORKBELLY;
			m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_PORKBELLY;
			m_DetailsList[5].iTexNum = 44;
			m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

			m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
			m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
			m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

			// 필요 재료 아이콘 셋팅
			m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_MEAT;
			m_0MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
			m_0MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
			m_0MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

			m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_PEPPER;
			m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
			m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
			m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

			m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SOYSAUCE;
			m_2MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
			m_2MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
			m_2MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));


			// 필요한 재료 셋팅
			NeedMaterial0 = 5 * MakeNum;
			pNeedMaterial0 = &NeedMaterial0;

			NeedMaterial1 = 2 * MakeNum;
			pNeedMaterial1 = &NeedMaterial1;

			NeedMaterial2 = 4 * MakeNum;
			pNeedMaterial2 = &NeedMaterial2;

			for (auto& Desc0 : m_1MaterialSlot)
			{
				Desc0.bRender = true;
			}
			for (auto& Desc1 : m_1MaterialSlot)
			{
				Desc1.bRender = true;
			}
			for (auto& Desc2 : m_2MaterialSlot)
			{
				Desc2.bRender = true;
			}


			// 필요 재료 숫자 셋팅
			SettingNeedMaterial0(pNeedMaterial0);
			SettingNeedMaterial1(pNeedMaterial1);
			SettingNeedMaterial2(pNeedMaterial2);

			// 보유 재료 셋팅
			pOwnMaterial0 = &InvenMeat;
			pOwnMaterial1 = &InvenPepper;
			pOwnMaterial2 = &InvenSoysauce;

			// 보유 재료 숫자 셋팅
			SettingOwnMaterial0(pOwnMaterial0);
			SettingOwnMaterial1(pOwnMaterial1);
			SettingOwnMaterial2(pOwnMaterial2);

			// 구매할 수 있는지 bMake 체크
			if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
			{
				bEnoughMaterial0 = false;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial0 = true;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

			}

			if (*pOwnMaterial1 < *pNeedMaterial1)
			{
				bEnoughMaterial1 = false;
				m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
				m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
				m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
			}
			else
			{
				bEnoughMaterial1 = true;
				m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
				m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
				m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
			}

			if (*pOwnMaterial2 < *pNeedMaterial2)
			{
				bEnoughMaterial1 = false;
				m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
				m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
				m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial1 = true;
				m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
				m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
				m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
			}
			if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
				bMake = true;
			else
				bMake = false;
		}
		else
		{ //레시피 사용전
			InvenRecipe0 = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE0);
			InvenPorkBelly = 0;

			// 요리 현재보유 숫자 셋팅 Own
			pOwnFood = &InvenPorkBelly;
			SettingOwnNum(pOwnFood);

			// 만들 요리 아이콘 셋팅
			m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_FinalList[4].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_PORKBELLY;
			m_DetailsList[5].iTexNum = 44;
			m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

			m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
			m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
			m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

			// 필요 재료 아이콘 셋팅
			m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_0MaterialSlot[0].fColorR = -(255.f - (color4.x * 255.f));
			m_0MaterialSlot[0].fColorG = -(255.f - (color4.y * 255.f));
			m_0MaterialSlot[0].fColorB = -(255.f - (color4.z * 255.f));

			// 필요한 재료 셋팅
			NeedMaterial0 = 1 * MakeNum;
			pNeedMaterial0 = &NeedMaterial0;

			NeedMaterial1 = 0 * MakeNum;
			pNeedMaterial1 = &NeedMaterial1;

			NeedMaterial2 = 0 * MakeNum;
			pNeedMaterial2 = &NeedMaterial2;

			for (auto& Desc0 : m_1MaterialSlot)
			{
				Desc0.bRender = true;
			}
			for (auto& Desc1 : m_1MaterialSlot)
			{
				Desc1.bRender = false;
			}
			for (auto& Desc2 : m_2MaterialSlot)
			{
				Desc2.bRender = false;
			}

			// 필요 재료 숫자 셋팅
			SettingNeedMaterial0(pNeedMaterial0);

			// 보유 재료 셋팅
			pOwnMaterial0 = &InvenRecipe0;

			// 보유 재료 숫자 셋팅
			SettingOwnMaterial0(pOwnMaterial0);

			// 만들 수 있는지 bMake 체크
			if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
			{
				bEnoughMaterial0 = false;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial0 = true;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
			}
			bMake = bEnoughMaterial0;

		}
	}
	break;
	case ITEM::DUCKNOODLES:
	{
		if (bUsedRecipe1)
		{
			InvenChicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
			InvenSoysauce = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
			InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
			InvenDucknoodles = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DUCKNOODLES);

			// 요리 현재보유 숫자 셋팅 Own
			pOwnFood = &InvenDucknoodles;
			SettingOwnNum(pOwnFood);

			// 만들 요리 아이콘 셋팅
			m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES;
			m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_DUCKNOODLES;;
			m_DetailsList[5].iTexNum = 44;
			m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

			m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
			m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
			m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

			// 필요 재료 아이콘 셋팅
			m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_CHICKEN;
			m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
			m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
			m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

			m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SOYSAUCE;
			m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
			m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
			m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

			m_2MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_FLOUR;
			m_2MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
			m_2MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
			m_2MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

			// 필요한 재료 셋팅
			NeedMaterial0 = 5 * MakeNum;
			pNeedMaterial0 = &NeedMaterial0;

			NeedMaterial1 = 3 * MakeNum;
			pNeedMaterial1 = &NeedMaterial1;

			NeedMaterial1 = 3 * MakeNum;
			pNeedMaterial1 = &NeedMaterial1;

			for (auto& Desc0 : m_1MaterialSlot)
			{
				Desc0.bRender = true;
			}
			for (auto& Desc1 : m_1MaterialSlot)
			{
				Desc1.bRender = true;
			}
			for (auto& Desc2 : m_2MaterialSlot)
			{
				Desc2.bRender = true;

			}
			// 필요 재료 숫자 셋팅
			SettingNeedMaterial0(pNeedMaterial0);
			SettingNeedMaterial1(pNeedMaterial1);
			SettingNeedMaterial2(pNeedMaterial2);

			// 보유 재료 셋팅
			pOwnMaterial0 = &InvenChicken;
			pOwnMaterial1 = &InvenSoysauce;
			pOwnMaterial2 = &InvenFlour;

			// 보유 재료 숫자 셋팅
			SettingOwnMaterial0(pOwnMaterial0);
			SettingOwnMaterial1(pOwnMaterial1);
			SettingOwnMaterial2(pOwnMaterial2);

			// 구매할 수 있는지 bMake 체크
			if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
			{
				bEnoughMaterial0 = false;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial0 = true;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;

			}

			if (*pOwnMaterial1 < *pNeedMaterial1)
			{
				bEnoughMaterial1 = false;
				m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
				m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 0.f;
				m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 0.f;
			}
			else
			{
				bEnoughMaterial1 = true;
				m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f - 189.f;
				m_1MaterialSlot[4].fColorG = m_1MaterialSlot[5].fColorG = m_1MaterialSlot[6].fColorG = 255.f - 172.f;
				m_1MaterialSlot[4].fColorB = m_1MaterialSlot[5].fColorB = m_1MaterialSlot[6].fColorB = 255.f - 100.f;
			}

			if (*pOwnMaterial2 < *pNeedMaterial2)
			{
				bEnoughMaterial1 = false;
				m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
				m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 0.f;
				m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial1 = true;
				m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f - 189.f;
				m_2MaterialSlot[4].fColorG = m_2MaterialSlot[5].fColorG = m_2MaterialSlot[6].fColorG = 255.f - 172.f;
				m_2MaterialSlot[4].fColorB = m_2MaterialSlot[5].fColorB = m_2MaterialSlot[6].fColorB = 255.f - 100.f;
			}
			if (true == bEnoughMaterial0 && true == bEnoughMaterial1 && true == bEnoughMaterial2)
				bMake = true;
			else
				bMake = false;
		}
		else // 레시피 사용전
		{
			//레시피 사용전
			InvenRecipe1 = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE1);
			InvenDucknoodles = 0;

			// 요리 현재보유 숫자 셋팅 Own
			pOwnFood = &InvenDucknoodles;
			SettingOwnNum(pOwnFood);

			// 만들 요리 아이콘 셋팅
			m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_FinalList[4].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_DUCKNOODLES;
			m_DetailsList[5].iTexNum = 44;
			m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

			m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
			m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
			m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

			// 필요 재료 아이콘 셋팅
			m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_RECIPE;
			m_0MaterialSlot[0].fColorR = -(255.f - (color4.x * 255.f));
			m_0MaterialSlot[0].fColorG = -(255.f - (color4.y * 255.f));
			m_0MaterialSlot[0].fColorB = -(255.f - (color4.z * 255.f));

			// 필요한 재료 셋팅
			NeedMaterial0 = 1 * MakeNum;
			pNeedMaterial0 = &NeedMaterial0;

			NeedMaterial1 = 0 * MakeNum;
			pNeedMaterial1 = &NeedMaterial1;

			NeedMaterial2 = 0 * MakeNum;
			pNeedMaterial2 = &NeedMaterial2;

			for (auto& Desc0 : m_1MaterialSlot)
			{
				Desc0.bRender = true;
			}
			for (auto& Desc1 : m_1MaterialSlot)
			{
				Desc1.bRender = false;
			}
			for (auto& Desc2 : m_2MaterialSlot)
			{
				Desc2.bRender = false;
			}

			// 필요 재료 숫자 셋팅
			SettingNeedMaterial0(pNeedMaterial0);

			// 보유 재료 셋팅
			pOwnMaterial0 = &InvenRecipe0;

			// 보유 재료 숫자 셋팅
			SettingOwnMaterial0(pOwnMaterial0);

			// 만들 수 있는지 bMake 체크
			if (*pOwnMaterial0 < *pNeedMaterial0) // 살 수 없다
			{
				bEnoughMaterial0 = false;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 0.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 0.f;
			}
			else // 살 수 있다
			{
				bEnoughMaterial0 = true;
				m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f - 189.f;
				m_0MaterialSlot[4].fColorG = m_0MaterialSlot[5].fColorG = m_0MaterialSlot[6].fColorG = 255.f - 172.f;
				m_0MaterialSlot[4].fColorB = m_0MaterialSlot[5].fColorB = m_0MaterialSlot[6].fColorB = 255.f - 100.f;
			}
			bMake = bEnoughMaterial0;
		}
	}
	break;
	}

}

void CUI_Cooking::SettingTotal(_int MakeNum) //총 수량
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string(MakeNum);
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: { m_LButtonList[3].iTexNum = m_FinalList[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: { m_LButtonList[4].iTexNum = m_FinalList[7].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}


	if (100 > MakeNum)
	{
		m_LButtonList[3].bRender = true;
		m_LButtonList[4].bRender = true;

		m_FinalList[6].bRender = true;
		m_FinalList[7].bRender = true;
		m_FinalList[8].bRender = false;
		m_FinalList[9].bRender = false;
	}
	if (10 > MakeNum)
	{
		m_LButtonList[3].bRender = true;
		m_LButtonList[4].bRender = false;

		m_FinalList[6].bRender = true;
		m_FinalList[7].bRender = false;
		m_FinalList[8].bRender = false;
		m_FinalList[9].bRender = false;
		if (1 < MakeNum)
		{
			MakeNum = 1;
		}
	}

}

void CUI_Cooking::SettingOwnMaterial0(_int* pFood) //보유재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_0MaterialSlot[4].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_0MaterialSlot[5].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 2: {m_0MaterialSlot[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}

	if (1000 > (*pFood))
	{
		m_0MaterialSlot[4].bRender = true;
		m_0MaterialSlot[5].bRender = true;
		m_0MaterialSlot[6].bRender = true;
	}
	if (100 > (*pFood))
	{
		m_0MaterialSlot[4].bRender = true;
		m_0MaterialSlot[5].bRender = true;
		m_0MaterialSlot[6].bRender = false;
	}
	if (10 > (*pFood))
	{
		m_0MaterialSlot[4].bRender = true;
		m_0MaterialSlot[5].bRender = false;
		m_0MaterialSlot[6].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_0MaterialSlot[4].iTexNum = 27;
		}
	}

}

void CUI_Cooking::SettingOwnMaterial1(_int* pFood) //보유재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_1MaterialSlot[4].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_1MaterialSlot[5].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 2: {m_1MaterialSlot[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}
	if (1000 > (*pFood))
	{
		m_1MaterialSlot[4].bRender = true;
		m_1MaterialSlot[5].bRender = true;
		m_1MaterialSlot[6].bRender = true;
	}
	if (100 > (*pFood))
	{
		m_1MaterialSlot[4].bRender = true;
		m_1MaterialSlot[5].bRender = true;
		m_1MaterialSlot[6].bRender = false;
	}
	if (10 > (*pFood))
	{
		m_1MaterialSlot[4].bRender = true;
		m_1MaterialSlot[5].bRender = false;
		m_1MaterialSlot[6].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_1MaterialSlot[4].iTexNum = 27;
		}
	}

}

void CUI_Cooking::SettingOwnMaterial2(_int* pFood) //보유재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_2MaterialSlot[4].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_2MaterialSlot[5].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 2: {m_2MaterialSlot[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}

	if (1000 > (*pFood))
	{
		m_2MaterialSlot[4].bRender = true;
		m_2MaterialSlot[5].bRender = true;
		m_2MaterialSlot[6].bRender = true;
	}
	if (100 > (*pFood))
	{
		m_2MaterialSlot[4].bRender = true;
		m_2MaterialSlot[5].bRender = true;
		m_2MaterialSlot[6].bRender = false;
	}
	if (10 > (*pFood))
	{
		m_2MaterialSlot[4].bRender = true;
		m_2MaterialSlot[5].bRender = false;
		m_2MaterialSlot[6].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_2MaterialSlot[4].iTexNum = 27;
		}
	}

}


void CUI_Cooking::SettingNeedMaterial0(_int* pFood) //필요재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_0MaterialSlot[2].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_0MaterialSlot[3].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}
	if (100 > (*pFood))
	{
		m_0MaterialSlot[2].bRender = true;
		m_0MaterialSlot[3].bRender = true;
	}
	if (10 > (*pFood))
	{
		m_0MaterialSlot[2].bRender = true;
		m_0MaterialSlot[3].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_0MaterialSlot[2].iTexNum = 27;
		}
	}
}

void CUI_Cooking::SettingNeedMaterial1(_int* pFood) //필요재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_1MaterialSlot[2].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_1MaterialSlot[3].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}
	if (100 > (*pFood))
	{
		m_1MaterialSlot[2].bRender = true;
		m_1MaterialSlot[3].bRender = true;
	}
	if (10 > (*pFood))
	{
		m_1MaterialSlot[2].bRender = true;
		m_1MaterialSlot[3].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_1MaterialSlot[2].iTexNum = 27;
		}
	}
}

void CUI_Cooking::SettingNeedMaterial2(_int* pFood) //필요재료 숫자
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_2MaterialSlot[2].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_2MaterialSlot[3].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}

	if (100 > (*pFood))
	{
		m_2MaterialSlot[2].bRender = true;
		m_2MaterialSlot[3].bRender = true;
	}
	if (10 > (*pFood))
	{
		m_2MaterialSlot[2].bRender = true;
		m_2MaterialSlot[3].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_2MaterialSlot[2].iTexNum = 27;
		}
	}
}


void CUI_Cooking::SettingOwnNum(_int* pFood) // 보유 요리 수
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string((*pFood));
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_DetailsList[1].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_DetailsList[2].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}

	if (100 > (*pFood))
	{
		m_DetailsList[1].bRender = true;
		m_DetailsList[2].bRender = true;
	}
	if (10 > (*pFood))
	{
		m_DetailsList[1].bRender = true;
		m_DetailsList[2].bRender = false;
		if (0 >= (*pFood))
		{
			(*pFood) = 0;
			m_DetailsList[1].iTexNum = 27;
		}
	}
}


void CUI_Cooking::SelectSlot()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (bUsedRecipe0)
		m_12Slot[3].bRender = false;
	else
		m_12Slot[3].bRender = true;

	if (bUsedRecipe1)
		m_13Slot[3].bRender = false;
	else
		m_13Slot[3].bRender = true;


	if (SelectUI(&m_0Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			// 슬롯을 선택했을 때,
			pSelectSlot = &m_0Slot[2];
			if (m_0Slot[3].bRender)
				m_0Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_1Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_1Slot[2];
			if (m_1Slot[3].bRender)
				m_1Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_2Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_2Slot[2];
			if (m_2Slot[3].bRender)
				m_2Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_3Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_3Slot[2];
			if (m_3Slot[3].bRender)
				m_3Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_4Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_4Slot[2];
			if (m_4Slot[3].bRender)
				m_4Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_5Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_5Slot[2];
			if (m_5Slot[3].bRender)
				m_5Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_6Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_6Slot[2];
			if (m_6Slot[3].bRender)
				m_6Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_7Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_7Slot[2];
			if (m_7Slot[3].bRender)
				m_7Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_8Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_8Slot[2];
			if (m_8Slot[3].bRender)
				m_8Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_9Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_9Slot[2];
			if (m_9Slot[3].bRender)
				m_9Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_10Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_10Slot[2];
			if (m_10Slot[3].bRender)
				m_10Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_11Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_11Slot[2];
			if (m_11Slot[3].bRender)
				m_11Slot[3].bRender = false;
		}
	}
	if (SelectUI(&m_12Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_12Slot[2];

			if (m_12Slot[3].bRender)
				m_12Slot[3].bRender = false;

		}
	}
	if (SelectUI(&m_13Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_ItemPick.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 슬롯 선택하는 소리

			pSelectSlot = &m_13Slot[2];

			if (m_13Slot[3].bRender)
				m_13Slot[3].bRender = false;
		}
	}
}

void CUI_Cooking::SelectGreade()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_GradeList[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 0;
		}
	}
	if (SelectUI(&m_GradeList[2]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 1;
		}
	}
	if (SelectUI(&m_GradeList[4]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 2;
		}
	}
	if (SelectUI(&m_GradeList[6]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 3;
		}
	}
	if (SelectUI(&m_GradeList[8]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 4;
		}
	}
	if (SelectUI(&m_GradeList[10]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_FilterOpen.wem.wav", SOUND_UI_BUTTON, VOLUME_VFX); // 등급 선택하는 소리

			GradeIndex = 5;
		}
	}

	switch (GradeIndex)
	{
	case 0:
	{
		m_GradeList[0].iTexNum = 55;
		m_GradeList[2].iTexNum = 54;
		m_GradeList[4].iTexNum = 54;
		m_GradeList[6].iTexNum = 54;
		m_GradeList[8].iTexNum = 54;
		m_GradeList[10].iTexNum = 54;
	}
	break;
	case 1:
	{
		m_GradeList[0].iTexNum = 54;
		m_GradeList[2].iTexNum = 55;
		m_GradeList[4].iTexNum = 54;
		m_GradeList[6].iTexNum = 54;
		m_GradeList[8].iTexNum = 54;
		m_GradeList[10].iTexNum = 54;
	}
	break;
	case 2:
	{
		m_GradeList[0].iTexNum = 54;
		m_GradeList[2].iTexNum = 54;
		m_GradeList[4].iTexNum = 55;
		m_GradeList[6].iTexNum = 54;
		m_GradeList[8].iTexNum = 54;
		m_GradeList[10].iTexNum = 54;
	}
	break;
	case 3:
	{
		m_GradeList[0].iTexNum = 54;
		m_GradeList[2].iTexNum = 54;
		m_GradeList[4].iTexNum = 54;
		m_GradeList[6].iTexNum = 55;
		m_GradeList[8].iTexNum = 54;
		m_GradeList[10].iTexNum = 54;
	}
	break;
	case 4:
	{
		m_GradeList[0].iTexNum = 54;
		m_GradeList[2].iTexNum = 54;
		m_GradeList[4].iTexNum = 54;
		m_GradeList[6].iTexNum = 54;
		m_GradeList[8].iTexNum = 55;
		m_GradeList[10].iTexNum = 54;
	}
	break;
	case 5:
	{
		m_GradeList[0].iTexNum = 54;
		m_GradeList[2].iTexNum = 54;
		m_GradeList[4].iTexNum = 54;
		m_GradeList[6].iTexNum = 54;
		m_GradeList[8].iTexNum = 54;
		m_GradeList[10].iTexNum = 55;
	}
	break;
	}


}
void CUI_Cooking::SettingSlot()
{
	switch (GradeIndex)
	{
	case 0:
	{
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_FLATBREAD;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_SALAD;
		m_2Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDTOFU;
		m_3Slot[2].iTexNum = STATIC_IMAGE::ICON_SALTEDTEA;
		m_4Slot[2].iTexNum = STATIC_IMAGE::ICON_HERBTEA;
		m_5Slot[2].iTexNum = STATIC_IMAGE::ICON_DRAGONNOODLE;
		m_6Slot[2].iTexNum = STATIC_IMAGE::ICON_OMURICE;
		m_7Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDRICE;
		m_8Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDCHICKEN;
		m_9Slot[2].iTexNum = STATIC_IMAGE::ICON_RABBITBREAD;
		m_10Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDMUSHROOM;
		m_11Slot[2].iTexNum = STATIC_IMAGE::ICON_MAPOTOFU;
		//레시피0를 사용했다면
		(bUsedRecipe0 ? m_12Slot[2].iTexNum = STATIC_IMAGE::ICON_PORKBELLY : m_12Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE);
		//레시피1를 사용했다면
		(bUsedRecipe1 ? m_13Slot[2].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES : m_13Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE);

		m_0Slot[2].SlotItemId = ITEM::FLATBREAD;
		m_1Slot[2].SlotItemId = ITEM::SALAD;
		m_2Slot[2].SlotItemId = ITEM::FRIEDTOFU;
		m_3Slot[2].SlotItemId = ITEM::SALTEDTEA;
		m_4Slot[2].SlotItemId = ITEM::HERBTEA;
		m_5Slot[2].SlotItemId = ITEM::DRAGONNOODLE;
		m_6Slot[2].SlotItemId = ITEM::OMURICE;
		m_7Slot[2].SlotItemId = ITEM::FRIEDRICE;
		m_8Slot[2].SlotItemId = ITEM::FRIEDCHICKEN;
		m_9Slot[2].SlotItemId = ITEM::RABBITBREAD;
		m_10Slot[2].SlotItemId = ITEM::FRIEDMUSHROOM;
		m_11Slot[2].SlotItemId = ITEM::MAPOTOFU;
		m_12Slot[2].SlotItemId = ITEM::PORKBELLY;
		m_13Slot[2].SlotItemId = ITEM::DUCKNOODLES;

		m_0Slot[1].fColorR = m_1Slot[1].fColorR = m_2Slot[1].fColorR = (color0.x * 255.f); //- (255.f - (color0.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = m_2Slot[1].fColorG = (color0.y * 255.f); //- (255.f - (color0.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = m_2Slot[1].fColorB = (color0.z * 255.f); //-(255.f - (color0.z * 255.f));

		m_3Slot[1].fColorR = m_4Slot[1].fColorR = (color1.x * 255.f); // - (255.f - (color1.x * 255.f));
		m_3Slot[1].fColorG = m_4Slot[1].fColorG = (color1.y * 255.f); // - (255.f - (color1.y * 255.f));
		m_3Slot[1].fColorB = m_4Slot[1].fColorB = (color1.z * 255.f); // -(255.f - (color1.z * 255.f));

		m_5Slot[1].fColorR = m_6Slot[1].fColorR = m_7Slot[1].fColorR = m_8Slot[1].fColorR = (color2.x * 255.f); // - (255.f - (color2.x * 255.f));
		m_5Slot[1].fColorG = m_6Slot[1].fColorG = m_7Slot[1].fColorG = m_8Slot[1].fColorG = (color2.y * 255.f); // - (255.f - (color2.y * 255.f));
		m_5Slot[1].fColorB = m_6Slot[1].fColorB = m_7Slot[1].fColorB = m_8Slot[1].fColorB = (color2.z * 255.f); // -(255.f - (color2.z * 255.f));

		m_9Slot[1].fColorR = m_10Slot[1].fColorR = m_11Slot[1].fColorR = (color3.x * 255.f); // - (255.f - (color3.x * 255.f));
		m_9Slot[1].fColorG = m_10Slot[1].fColorG = m_11Slot[1].fColorG = (color3.y * 255.f); // - (255.f - (color3.y * 255.f));
		m_9Slot[1].fColorB = m_10Slot[1].fColorB = m_11Slot[1].fColorB = (color3.z * 255.f); // -(255.f - (color3.z * 255.f));

		m_12Slot[1].fColorR = m_13Slot[1].fColorR = (color4.x * 255.f); //-(255.f - (color4.x * 255.f));
		m_12Slot[1].fColorG = m_13Slot[1].fColorG = (color4.y * 255.f); //-(255.f - (color4.y * 255.f));
		m_12Slot[1].fColorB = m_13Slot[1].fColorB = (color4.z * 255.f); //-(255.f - (color4.z * 255.f));

	}
	break;
	case 1:
	{
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_FLATBREAD;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_SALAD;
		m_2Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDTOFU;

		m_0Slot[2].SlotItemId = ITEM::FLATBREAD;
		m_1Slot[2].SlotItemId = ITEM::SALAD;
		m_2Slot[2].SlotItemId = ITEM::FRIEDTOFU;

		m_13Slot[2].SlotItemId = ITEM::DUCKNOODLES;
		m_0Slot[1].fColorR = m_1Slot[1].fColorR = m_2Slot[1].fColorR = (color0.x * 255.f); //-(255.f - (color0.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = m_2Slot[1].fColorG = (color0.y * 255.f); //-(255.f - (color0.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = m_2Slot[1].fColorB = (color0.z * 255.f); //-(255.f - (color0.z * 255.f));


	}
	break;
	case 2:
	{
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_SALTEDTEA;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_HERBTEA;

		m_0Slot[2].SlotItemId = ITEM::SALTEDTEA;
		m_1Slot[2].SlotItemId = ITEM::HERBTEA;

		m_0Slot[1].fColorR = m_1Slot[1].fColorR = (color1.x * 255.f); //-(255.f - (color1.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = (color1.y * 255.f); //-(255.f - (color1.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = (color1.z * 255.f); //-(255.f - (color1.z * 255.f));
	}
	break;
	case 3:
	{
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_DRAGONNOODLE;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_OMURICE;
		m_2Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDRICE;
		m_3Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDCHICKEN;

		m_0Slot[2].SlotItemId = ITEM::DRAGONNOODLE;
		m_1Slot[2].SlotItemId = ITEM::OMURICE;
		m_2Slot[2].SlotItemId = ITEM::FRIEDRICE;
		m_3Slot[2].SlotItemId = ITEM::FRIEDCHICKEN;

		m_0Slot[1].fColorR = m_1Slot[1].fColorR = m_2Slot[1].fColorR = m_3Slot[1].fColorR = (color2.x * 255.f); //-(255.f - (color2.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = m_2Slot[1].fColorG = m_3Slot[1].fColorG = (color2.y * 255.f); //-(255.f - (color2.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = m_2Slot[1].fColorB = m_3Slot[1].fColorB = (color2.z * 255.f); //-(255.f - (color2.z * 255.f));


	}
	break;
	case 4:
	{
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_RABBITBREAD;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_FRIEDMUSHROOM;
		m_2Slot[2].iTexNum = STATIC_IMAGE::ICON_MAPOTOFU;

		m_0Slot[2].SlotItemId = ITEM::RABBITBREAD;
		m_1Slot[2].SlotItemId = ITEM::FRIEDMUSHROOM;
		m_2Slot[2].SlotItemId = ITEM::MAPOTOFU;

		m_0Slot[1].fColorR = m_1Slot[1].fColorR = m_2Slot[1].fColorR = (color3.x * 255.f); //- (255.f - (color3.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = m_2Slot[1].fColorG = (color3.y * 255.f); //- (255.f - (color3.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = m_2Slot[1].fColorB = (color3.z * 255.f); //-(255.f - (color3.z * 255.f));


	}
	break;
	case 5:
	{
		//레시피0를 사용했다면
		(bUsedRecipe0 ? m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_PORKBELLY : m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE);
		//레시피1를 사용했다면
		(bUsedRecipe1 ? m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES : m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE);


		m_0Slot[2].SlotItemId = ITEM::PORKBELLY;
		m_1Slot[2].SlotItemId = ITEM::DUCKNOODLES;

		m_0Slot[1].fColorR = m_1Slot[1].fColorR = (color4.x * 255.f); // - (255.f - (color4.x * 255.f));
		m_0Slot[1].fColorG = m_1Slot[1].fColorG = (color4.y * 255.f); // - (255.f - (color4.y * 255.f));
		m_0Slot[1].fColorB = m_1Slot[1].fColorB = (color4.z * 255.f); // -(255.f - (color4.z * 255.f));
	}
	break;
	}
}
void CUI_Cooking::InMenuOpen(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (AddAlphaW(&m_CommonList, TimeDelta))
		++Count;
	if (AddAlphaW(&m_GradeList, TimeDelta))
		++Count;
	if (AddAlphaW(&m_DetailsList, TimeDelta))
		++Count;
	if (AddAlphaW(&m_0MaterialSlot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_1MaterialSlot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_2MaterialSlot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_LButtonList, TimeDelta))
		++Count;
	if (AddAlphaW(&m_RButtonList, TimeDelta))
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
	if (AddAlphaW(&m_7Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_8Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_9Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_10Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_11Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_12Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_13Slot, TimeDelta))
		++Count;

	if (22 == Count)
	{
		m_MenuRenderStart = false;
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		pGameInstance->PlaySoundEx(L"Play_AE_UI_FB_Open_2.wav ", SOUND_UI_FEEDBACK, VOLUME_VFX); // 목록 열리는 소리,NPC들어가는 소리

	}
}

_bool CUI_Cooking::InMenuEnd(_double TimeDelta)
{// 창 열릴 때 전체 랜더
	_int Count = 0;

	if (MinusAlphaW(&m_CommonList, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_GradeList, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_DetailsList, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_0MaterialSlot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_1MaterialSlot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_2MaterialSlot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_LButtonList, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_RButtonList, TimeDelta))
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
	if (MinusAlphaW(&m_7Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_8Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_9Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_10Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_11Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_12Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_13Slot, TimeDelta))
		++Count;

	if (22 == Count)
	{
		return true;
	}
	else
		return false;
}


void CUI_Cooking::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Cooking::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;
	switch (Situation)
	{
	case Client::CUI_Cooking::MEET:
	{
		for (_uint i = 0; i < (_int)m_Cookist.size(); ++i)
		{
			if (m_Cookist[i].bRender)
			{
				if (FAILED(Setup_CookShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Cooking::SETTINGNUM:
	{
		for (_uint i = 0; i < (_int)m_Cookist.size(); ++i)
		{
			if (m_Cookist[i].bRender)
			{
				if (FAILED(Setup_CookShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Cooking::MENU:
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
		for (_uint i = 0; i < (_int)m_GradeList.size(); ++i)
		{
			if (m_GradeList[i].bRender)
			{
				if (FAILED(Setup_GradeShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}

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

		for (_uint i = 0; i < (_int)m_0MaterialSlot.size(); ++i)
		{
			if (m_0MaterialSlot[i].bRender)
			{
				if (FAILED(Setup_0MaterialShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}

		for (_uint i = 0; i < (_int)m_1MaterialSlot.size(); ++i)
		{
			if (m_1MaterialSlot[i].bRender)
			{
				if (FAILED(Setup_1MaterialShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_2MaterialSlot.size(); ++i)
		{
			if (m_2MaterialSlot[i].bRender)
			{
				if (FAILED(Setup_2MaterialShader(i)))
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
		// 등급 별 요리 랜더
		switch (GradeIndex)
		{
		case 0: // all
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_2Slot.size(); ++i)
			{
				if (m_2Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_2Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_3Slot.size(); ++i)
			{
				if (m_3Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_3Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_4Slot.size(); ++i)
			{
				if (m_4Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_4Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_5Slot.size(); ++i)
			{
				if (m_5Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_5Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_6Slot.size(); ++i)
			{
				if (m_6Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_6Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_7Slot.size(); ++i)
			{
				if (m_7Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_7Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_8Slot.size(); ++i)
			{
				if (m_8Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_8Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_9Slot.size(); ++i)
			{
				if (m_9Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_9Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_10Slot.size(); ++i)
			{
				if (m_10Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_10Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_11Slot.size(); ++i)
			{
				if (m_11Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_11Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_12Slot.size(); ++i)
			{
				if (m_12Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_12Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_13Slot.size(); ++i)
			{
				if (m_13Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_13Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		break;
		case 1:
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_2Slot.size(); ++i)
			{
				if (m_2Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_2Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		break;
		case 2:
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		break;
		case 3:
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_2Slot.size(); ++i)
			{
				if (m_2Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_2Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_3Slot.size(); ++i)
			{
				if (m_3Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_3Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		break;
		case 4:
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
			for (_uint i = 0; i < (_int)m_2Slot.size(); ++i)
			{
				if (m_2Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_2Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		break;
		case 5:
		{
			for (_uint i = 0; i < (_int)m_0Slot.size(); ++i)
			{
				if (m_0Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_0Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}

			for (_uint i = 0; i < (_int)m_1Slot.size(); ++i)
			{
				if (m_1Slot[i].bRender)
				{
					if (FAILED(Setup_SlotShader(&m_1Slot, i)))
						return E_FAIL;
					m_pShader->Begin(m_iPass);
					m_pVIBuffer->Render();
				}
			}
		}
		}
	}
	case Client::CUI_Cooking::READYCONFRIM:
	{
	}
	break;
	case Client::CUI_Cooking::CONFRIM:
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
	case Client::CUI_Cooking::BYE:
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
	}


	return S_OK;
}

void CUI_Cooking::RenderGUI()
{
}

_bool CUI_Cooking::SelectUI(COOKDESC* pDesc)
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

HRESULT CUI_Cooking::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UICOOKING,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}


void CUI_Cooking::Set_SituMeet()
{
	SetState(ACTIVE);
	Situation = COOKSITUINDEX::MEET;
}

void CUI_Cooking::Set_END()
{
	Situation = COOKSITUINDEX::COOKEND;
	m_Count = 0;
	m_MenuRenderStart = true;
	m_MenuOutStart = false;
	m_ReadyConfirmRender = false;
	m_ConfirmRenderStart = true;
	pSelectSlot = &m_0Slot[2];
	pOwnFood = &InvenFlatbread;
	pNeedMaterial0 = &NeedMaterial0;
	pNeedMaterial1 = &NeedMaterial1;
	pNeedMaterial2 = &NeedMaterial2;
	pOwnMaterial0 = &InvenFlour;
	pOwnMaterial1 = &InvenRice;
	pOwnMaterial2 = &InvenTofu;
	MakeNum = 1;
	bEnoughMaterial0 = false;
	bEnoughMaterial1 = false;
	bEnoughMaterial2 = false;
	bMake = false;
	GradeIndex = 0;
	m_NPCbye = false;
	for (auto& Desc : m_Cookist)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_CommonList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_GradeList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_DetailsList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_0MaterialSlot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_1MaterialSlot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_2MaterialSlot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_LButtonList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_RButtonList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_FinalList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_0Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_1Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_2Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_3Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_4Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_5Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_6Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_7Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_8Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_9Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_10Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_11Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_12Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_13Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	SetState(DISABLE);
}

void CUI_Cooking::IsMouseinRect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_0Slot[0]))
	{
		m_0Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_0Slot[8].bRender = false;

	if (SelectUI(&m_1Slot[0]))
	{
		m_1Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_1Slot[8].bRender = false;

	if (SelectUI(&m_2Slot[0]))
	{
		m_2Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_2Slot[8].bRender = false;

	if (SelectUI(&m_3Slot[0]))
	{
		m_3Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_3Slot[8].bRender = false;

}
_bool CUI_Cooking::AddAlpha(COOKDESC* pDesc, _double TimeDelta)
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

_bool CUI_Cooking::MinusAlpha(COOKDESC* pDesc, _double TimeDelta)
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

_bool CUI_Cooking::AddAlphaW(vector<COOKDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if ((*pDesc)[i].Color.w >(*pDesc)[i].fColorA)
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

_bool CUI_Cooking::MinusAlphaW(vector<COOKDESC>* pDesc, _double TimeDelta)
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

void CUI_Cooking::ColorP(COOKDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_Cooking::ColorM(COOKDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_Cooking::Load()
{
	//
	_uint index = 3;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.Color.w = Desc.fColorA = 0.f;
		Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_Cookist.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 10;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookCommon%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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

	//
	index = 12;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookGrade%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		m_GradeList.push_back(Desc);
		CloseHandle(hFile);

	}


	index = 14;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookFinal%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		m_FinalList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookDetail%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		m_DetailsList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0CookSlot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_0Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook1Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_1Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook2Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_2Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook3Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_3Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook4Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_4Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook5Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_5Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook6Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_6Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook7Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_7Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook8Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_8Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook9Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_9Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook10Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_10Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook11Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_11Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook12Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_12Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 4;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook13Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_13Slot.push_back(Desc);
		CloseHandle(hFile);

	}


	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0Material%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		//Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_0MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1Material%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		//Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_1MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 7;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2Material%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		//Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_2MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookLButton%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		//Desc.iPass = 21;
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
		wsprintf(szFileName, L"../../Data/UI/CookRButton%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		COOKDESC Desc;
		ZeroMemory(&Desc, sizeof(COOKDESC));

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
		//Desc.iPass = 21;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_RButtonList.push_back(Desc);
		CloseHandle(hFile);

	}
}


void CUI_Cooking::Save()
{
	//
	_uint index = 0;
	for (auto& Desc : m_Cookist)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/CookCommon%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
	for (auto& Desc : m_GradeList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CookGrade%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/CookFinal%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/CookDetail%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/Cook0Slot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/1CookSlot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/2CookSlot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/3CookSlot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/4CookSlot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/5CookSlot%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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

		//
		index = 0;
		for (auto& Desc : m_6Slot)
		{
			TCHAR	szFileName[128] = L"";
			wsprintf(szFileName, L"../../Data/UI/6CookSlot%d.dat", index);
			HANDLE hFile = CreateFile(
				szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			//Desc.fColorA = Desc.Color.w;
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

			//
			index = 0;
			for (auto& Desc : m_7Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/7CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_8Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/8CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_9Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/9CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_10Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/10CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_11Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/11CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_12Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/12CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
			for (auto& Desc : m_13Slot)
			{
				TCHAR	szFileName[128] = L"";
				wsprintf(szFileName, L"../../Data/UI/13CookSlot%d.dat", index);
				HANDLE hFile = CreateFile(
					szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				//Desc.fColorA = Desc.Color.w;
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
	}

	//
	index = 0;
	for (auto& Desc : m_0MaterialSlot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/0Material%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
	for (auto& Desc : m_1MaterialSlot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/1Material%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
	for (auto& Desc : m_2MaterialSlot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/2Material%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/CookLButton%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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
		wsprintf(szFileName, L"../../Data/UI/CookRButton%d.dat", index);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		//Desc.fColorA = Desc.Color.w;
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

HRESULT CUI_Cooking::Setup_CookShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_Cookist[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_Cookist[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_Cookist[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_Cookist[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_Cookist[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_Cookist[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_Cookist[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_CommonShader(_uint index)
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

HRESULT CUI_Cooking::Setup_GradeShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_GradeList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_GradeList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_GradeList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_GradeList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_GradeList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_GradeList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_GradeList[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_DetailsShader(_uint index)
{
	if ((3 == index) || (4 == index) || (6 == index))
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_DetailsList[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DetailsList[index].iTexNum)))
			return E_FAIL;
	}
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
HRESULT CUI_Cooking::Setup_0MaterialShader(_uint index)
{
	if (1 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_0MaterialSlot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_0MaterialSlot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_0MaterialSlot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_0MaterialSlot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_0MaterialSlot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_0MaterialSlot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_0MaterialSlot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_0MaterialSlot[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_1MaterialShader(_uint index)
{
	if (1 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_1MaterialSlot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_1MaterialSlot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_1MaterialSlot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_1MaterialSlot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_1MaterialSlot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_1MaterialSlot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_1MaterialSlot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_1MaterialSlot[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_2MaterialShader(_uint index)
{
	if (1 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_2MaterialSlot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_2MaterialSlot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_2MaterialSlot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_2MaterialSlot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_2MaterialSlot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_2MaterialSlot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_2MaterialSlot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_2MaterialSlot[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_SlotShader(vector<COOKDESC>* Desc, _uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV((*Desc)[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", (*Desc)[index].iTexNum)))
			return E_FAIL;
	}


	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &((*Desc)[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &((*Desc)[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &((*Desc)[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &((*Desc)[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &((*Desc)[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_0Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Cooking::Setup_LButtonShader(_uint index)
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

HRESULT CUI_Cooking::Setup_RButtonShader(_uint index)
{
	XMStoreFloat4x4(&(m_RButtonList[index].WorldMatrix), XMMatrixScaling(m_RButtonList[index].fWidth, m_RButtonList[index].fHeight, 1.f)
		* XMMatrixTranslation(m_RButtonList[index].fX, m_RButtonList[index].fY, m_RButtonList[index].fZ));

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


HRESULT CUI_Cooking::Setup_FinalShader(_uint index)
{
	XMStoreFloat4x4(&(m_FinalList[index].WorldMatrix), XMMatrixScaling(m_FinalList[index].fWidth, m_FinalList[index].fHeight, 1.f)
		* XMMatrixTranslation(m_FinalList[index].fX, m_FinalList[index].fY, m_FinalList[index].fZ));
	if (4 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_FinalList[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_FinalList[index].iTexNum)))
			return E_FAIL;
	}
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


CUI_Cooking* CUI_Cooking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Cooking* pInstance = new CUI_Cooking(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Cooking";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Cooking::Clone(void* pArg)
{
	CUI_Cooking* pInstance = new CUI_Cooking(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Cooking";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Cooking::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}