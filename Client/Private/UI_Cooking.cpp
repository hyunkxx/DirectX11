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

	// ��ġ �̵� �ӽ��ڵ�
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

	return S_OK;
}

void CUI_Cooking::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));

	SetState(DISABLE);

	// ������, ������ 
	/* NPC�� �÷��̾�� �浹�ϸ� Setstate()�� Ȱ��ȭ , ��Ȱ��ȭ*/
}

void CUI_Cooking::Tick(_double TimeDelta)
{
	// ������ ���� �߰��ؾ���

	__super::Tick(TimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	switch (Situation)
	{
	case Client::CUI_Cooking::MEET:
	{
		if (SelectUI(&m_Cookist[0]))
		{
			if (false == m_Cookist[0].OnRect)
			{
				for (auto& Sou : m_Cookist)
				{
					Sou.fColorA = Sou.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
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
		//// �κ��丮�� �ִ� �丮���� �����۵� ���� ��������
		//InvenRice		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		//InvenTofu		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		//InvenFlour		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		//InvenMushroom	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MUSHROOM);
		//InvenChicken	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
		//InvenEgg		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		//InvenMeat		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		//InvenHerb		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::HERB);
		//InvenViolet		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::VIOLET);
		//InvenDandelion	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::DANDELION);
		//InvenPepper		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
		//InvenSugar		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		//InvenSalt		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		//InvenOil		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		//InvenSoysauce	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
		//InvenRecipe0	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE0);
		//InvenRecipe1	= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RECIPE1);
		//InvenFlatbread			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FLATBREAD);
		//InvenSalad				= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALAD);
		//InvenFriedTofu			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDTOFU);
		//InvenSaltedTea			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALTEDTEA);
		//InvenHerbTea			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::HERBTEA);
		//InvenDragonNoodle		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DRAGONNOODLE);
		//InvenOmurice			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::OMURICE);
		//InvenFriedrice			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDRICE);
		//InvenFriedchicken		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDCHICKEN);
		//InvenRabbitbread		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::RABBITBREAD);
		//InvenFriedMushroom		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDMUSHROOM);
		//InvenMapotofu			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::MAPOTOFU);
		//InvenPorkBelly			= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::PORKBELLY);
		//InvenDucknoodles		= m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DUCKNOODLES);
		// ó�� �޴� ������ ���: all , ������ : �븻�� ù��° �޴�
		// ��� all�� ���� �ؽ�ó ���ְ� �������� ���
		// ����ġ ��� -> ���Ը��� �丮 �ؽ�ó [2], �÷� ����[1], ��[3]�� ���� -> ������ ������������ Ŵ
		SettingSlot();
		Situation = CUI_Cooking::MENU;
	}
	break;
	case Client::CUI_Cooking::MENU:
	{
		if (SelectUI(&m_CommonList[9]))
		{
			m_CommonList[9].iTexNum = 8;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_MenuOutStart = true;;
			}
		}
		else
			m_CommonList[9].iTexNum = 7;

		if(m_MenuRenderStart)
		{
			InMenuOpen(TimeDelta);
		}
		if (m_MenuOutStart)
		{
			if (InMenuEnd(TimeDelta))
			{
				m_MenuOutStart = false;
				Situation = COOKEND;
				SetState(DISABLE);
			}
		}
		else // ������ ���� ���� �ƴ϶��
		{

			// �޴� ������ ������ ����
			SelectGreade();
			SettingSlot();
			SelectSlot();
			SettingNumbers();

			// ���� ���� ��ư-> ���� �������� ������ ������
			if (SelectUI(&m_LButtonList[1]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					--MakeNum;
				}
			}

			if (SelectUI(&m_LButtonList[2]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					++MakeNum;
				}
			}

			if (bMake) // ���� ��� �� ������
			{
				// Ŭ�� ���� 
				if (SelectUI(&m_RButtonList[0]))
				{
					// ���콺 �ø��� �ؽ�ó �����
					m_RButtonList[0].iTexNum = 20;
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						Situation = CUI_Cooking::READYCONFRIM;
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
	}
	break;
	case Client::CUI_Cooking::READYCONFRIM:
	{
		// ��Ʈ  ����
		SettingTotal(MakeNum);
		if (InMenuEnd(TimeDelta))
		{// ���⼭ ���� �ȵǸ� ���� ����ġ�� READYCONFRIM �߰����ְ� �����Լ��� �߰��ϱ�
			Situation = CUI_Cooking::CONFRIM;
		}
	}
	case Client::CUI_Cooking::CONFRIM:
	{
		if (m_ConfirmRenderStart)
			(AddAlphaW(&m_FinalList, TimeDelta) ? m_ConfirmRenderStart = false : m_ConfirmRenderStart = true);

		// Ȯ�� ������ ������� ���ư���, �� ����
		if (SelectUI(&m_FinalList[10]))
		{
			ColorP(&m_FinalList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
			ColorP(&m_FinalList[11], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
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
		m_pInven->AddItem((*pSelectSlot).SlotItemId, MakeNum); // ���� �丮

		// �κ��� ������ �ֱ�
		//��� ��� ���ҽ�Ű��
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
			m_pInven->AddItem(ITEM::OIL,  -1 * MakeNum);
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
			m_pInven->AddItem(ITEM::MEAT,  -2 * MakeNum);
			m_pInven->AddItem(ITEM::SALT,  -2 * MakeNum);
		}
		break;
		case ITEM::OMURICE:
		{
			m_pInven->AddItem(ITEM::RICE,  -2 * MakeNum);
			m_pInven->AddItem(ITEM::EGG,   -3 * MakeNum);
			m_pInven->AddItem(ITEM::SUGAR, -1 * MakeNum);
		}
		break;
		case ITEM::FRIEDRICE:
		{
			m_pInven->AddItem(ITEM::RICE,  -2 * MakeNum);
			m_pInven->AddItem(ITEM::EGG,   -2 * MakeNum);
			m_pInven->AddItem(ITEM::OIL,   -2 * MakeNum);
		}
		break;
		case ITEM::FRIEDCHICKEN:
		{
			m_pInven->AddItem(ITEM::CHICKEN, -2 * MakeNum);
			m_pInven->AddItem(ITEM::SALT,    -1 * MakeNum);
			m_pInven->AddItem(ITEM::OIL,     -3 * MakeNum);
		}
		break;
		case ITEM::RABBITBREAD:
		{
			m_pInven->AddItem(ITEM::FLOUR,  -5 * MakeNum);
			m_pInven->AddItem(ITEM::EGG,    -3 * MakeNum);
			m_pInven->AddItem(ITEM::SUGAR,  -3 * MakeNum);
		}
		break;
		case ITEM::FRIEDMUSHROOM:
		{
			m_pInven->AddItem(ITEM::MUSHROOM, -5 * MakeNum);
			m_pInven->AddItem(ITEM::OIL,      -4 * MakeNum);
			m_pInven->AddItem(ITEM::SALT,     -2 * MakeNum);
		}
		break;
		case ITEM::MAPOTOFU:
		{
			m_pInven->AddItem(ITEM::TOFU,   -5 * MakeNum);
			m_pInven->AddItem(ITEM::PEPPER, -3 * MakeNum);
			m_pInven->AddItem(ITEM::MEAT,   -3 * MakeNum);
		}
		break;
		case ITEM::PORKBELLY:
		{
			m_pInven->AddItem(ITEM::MEAT,     -5 * MakeNum);
			m_pInven->AddItem(ITEM::PEPPER,   -2 * MakeNum);
			m_pInven->AddItem(ITEM::SOYSAUCE, -4 * MakeNum);
		}
		break;
		case ITEM::DUCKNOODLES:
		{
			m_pInven->AddItem(ITEM::CHICKEN,  -5 * MakeNum);
			m_pInven->AddItem(ITEM::FLOUR,    -2 * MakeNum);
			m_pInven->AddItem(ITEM::SOYSAUCE, -3 * MakeNum);
		}
		break;
		}
		// ��� ���� �ʱ�ȭ
		Situation = CUI_Cooking::MENU;
		m_MenuRenderStart = true;
		m_ConfirmRenderStart = true;
		MakeNum = 0;
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

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����, ���̳� ������, 
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FLATBREAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FLATBREAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		//���̳� �ڽ� ���� ����
		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));

		// �ʿ� ��� ������ ����
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_FLOUR;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		// �ʿ��� ��� ����
		NeedMaterial0 = 2 * MakeNum;
		pNeedMaterial0 = &NeedMaterial0;

		// ���� �� ������ ���� ������� �ʴ� ������ ����
		NeedMaterial1 = 0 * MakeNum;
		pNeedMaterial1 = &NeedMaterial1;
		NeedMaterial2 = 0 * MakeNum;
		pNeedMaterial2 = &NeedMaterial2;

		// ������� �ʴ� ��� ���Կ� ���� ��������
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
		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);

		// ���� ��� ����
		pOwnMaterial0 = &InvenFlour;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);


		
	}
	break;
	case ITEM::SALAD:
	{
		InvenViolet = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::VIOLET);
		InvenSalad = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALAD);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenSalad;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_SALAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_SALAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));


		// �ʿ� ��� ������ ����
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_VIOLET;
		m_0MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);

		// ���� ��� ����
		pOwnMaterial0 = &InvenViolet;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
	}
	break;
	case ITEM::FRIEDTOFU:
	{
		InvenTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenFriedTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDTOFU);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFriedTofu;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDTOFU;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDTOFU;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODNORMAL;

		m_FinalList[3].fColorR = -(255.f - (color0.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color0.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color0.z * 255.f));


		// �ʿ� ��� ������ ����
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_TOFU;
		m_0MaterialSlot[0].fColorR = -(255.f - (color0.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color0.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color0.z * 255.f));

		m_1MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_OIL;
		m_1MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_1MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_1MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);

		// ���� ��� ����
		pOwnMaterial0 = &InvenTofu;
		pOwnMaterial1 = &InvenOil;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial0(pOwnMaterial1);
	}
	break;
	case ITEM::SALTEDTEA:
	{
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenSaltedTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::SALTEDTEA);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenSaltedTea;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_SALTEDTEA;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_SALTEDTEA;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODADVANCED;

		m_FinalList[3].fColorR = -(255.f - (color1.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color1.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color1.z * 255.f));

		// �ʿ� ��� ������ ����
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_SALT;
		m_0MaterialSlot[0].fColorR = -(255.f - (color2.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color2.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color2.z * 255.f));

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);

		// ���� ��� ����
		pOwnMaterial0 = &InvenSalt;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
	}
	break;
	case ITEM::HERBTEA:
	{
		InvenHerb = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::HERB);
		InvenHerbTea = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::HERBTEA);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenHerbTea;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_HERBTEA;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_HERBTEA;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODADVANCED;

		m_FinalList[3].fColorR = -(255.f - (color1.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color1.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color1.z * 255.f));

		// �ʿ� ��� ������ ����
		m_0MaterialSlot[1].iTexNum = STATIC_IMAGE::ICON_HERB;
		m_0MaterialSlot[0].fColorR = -(255.f - (color1.x * 255.f));
		m_0MaterialSlot[0].fColorG = -(255.f - (color1.y * 255.f));
		m_0MaterialSlot[0].fColorB = -(255.f - (color1.z * 255.f));

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);

		// ���� ��� ����
		pOwnMaterial0 = &InvenHerb;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
	}
	break;
	case ITEM::DRAGONNOODLE:
	{
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenDragonNoodle = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DRAGONNOODLE);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenDragonNoodle;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_DRAGONNOODLE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_DRAGONNOODLE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));

		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenFlour;
		pOwnMaterial1 = &InvenMeat;
		pOwnMaterial2 = &InvenSalt;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::OMURICE:
	{
		InvenRice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenSugar = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::OMURICE);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_OMURICE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_OMURICE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenRice;
		pOwnMaterial1 = &InvenEgg;
		pOwnMaterial2 = &InvenSugar;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::FRIEDRICE:
	{
		InvenRice = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::RICE);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDRICE);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDRICE;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDRICE;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenRice;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenEgg;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::FRIEDCHICKEN:
	{
		InvenChicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDCHICKEN);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDCHICKEN;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDCHICKEN;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODRARE;

		m_FinalList[3].fColorR = -(255.f - (color2.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color2.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color2.z * 255.f));


		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenChicken;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenSalt;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::RABBITBREAD:
	{
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenEgg = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::EGG);
		InvenSugar = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SUGAR);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::RABBITBREAD);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_RABBITBREAD;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_RABBITBREAD;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));


		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenFlour;
		pOwnMaterial1 = &InvenEgg;
		pOwnMaterial2 = &InvenSugar;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::FRIEDMUSHROOM:
	{
		InvenMushroom = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MUSHROOM);
		InvenOil = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::OIL);
		InvenSalt = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SALT);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::FRIEDMUSHROOM);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_FRIEDMUSHROOM;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_FRIEDMUSHROOM;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));


		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenMushroom;
		pOwnMaterial1 = &InvenOil;
		pOwnMaterial2 = &InvenSalt;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::MAPOTOFU:
	{
		InvenTofu = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::TOFU);
		InvenPepper = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::MAPOTOFU);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_MAPOTOFU;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_MAPOTOFU;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODUNIQUE;

		m_FinalList[3].fColorR = -(255.f - (color3.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color3.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color3.z * 255.f));

		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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

		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenTofu;
		pOwnMaterial1 = &InvenPepper;
		pOwnMaterial2 = &InvenMeat;


		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	case ITEM::PORKBELLY:
	{
		InvenMeat = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::MEAT);
		InvenPepper = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::PEPPER);
		InvenSoysauce = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::PORKBELLY);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_PORKBELLY;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_PORKBELLY;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

		m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

		// �ʿ� ��� ������ ����
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


		// �ʿ��� ��� ����
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


		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenMeat;
		pOwnMaterial1 = &InvenPepper;
		pOwnMaterial2 = &InvenSoysauce;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
	}
	break;
	case ITEM::DUCKNOODLES:
	{
		InvenChicken = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::CHICKEN);
		InvenSoysauce = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::SOYSAUCE);
		InvenFlour = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_MATERIAL, ITEM::FLOUR);
		InvenFlatbread = m_pInven->GetTotalAmount(CInventory::INVEN_TYPE::INVEN_COOK, ITEM::DUCKNOODLES);

		// �丮 ���纸�� ���� ���� Own
		pOwnFood = &InvenFlatbread;
		SettingOwnNum(pOwnFood);

		// ���� �丮 ������ ����
		m_FinalList[4].iTexNum = m_DetailsList[3].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES;
		m_DetailsList[4].iTexNum = STATIC_IMAGE::TEXT_DUCKNOODLES;;
		m_DetailsList[5].iTexNum = 44;
		m_DetailsList[6].iTexNum = STATIC_IMAGE::ITEM_DESC_FOODLEGEND;

		m_FinalList[3].fColorR = -(255.f - (color4.x * 255.f));
		m_FinalList[3].fColorG = -(255.f - (color4.y * 255.f));
		m_FinalList[3].fColorB = -(255.f - (color4.z * 255.f));

		// �ʿ� ��� ������ ����
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

		// �ʿ��� ��� ����
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
		// �ʿ� ��� ���� ����
		SettingNeedMaterial0(pNeedMaterial0);
		SettingNeedMaterial1(pNeedMaterial1);
		SettingNeedMaterial2(pNeedMaterial2);

		// ���� ��� ����
		pOwnMaterial0 = &InvenChicken;
		pOwnMaterial1 = &InvenSoysauce;
		pOwnMaterial2 = &InvenFlour;

		// ���� ��� ���� ����
		SettingOwnMaterial0(pOwnMaterial0);
		SettingOwnMaterial1(pOwnMaterial1);
		SettingOwnMaterial2(pOwnMaterial2);
	}
	break;
	}

	if (*pOwnMaterial0 < *pNeedMaterial0) 
	{
		bMake = false;
		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f * 0.8f;
		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f * 0.2f;
		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 255.f * 0.2f;
	}
	if (*pOwnMaterial1 < *pNeedMaterial1)
	{
		bMake = false;

		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f * 0.8f;
		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f * 0.2f;
		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 255.f * 0.2f;
	}
	if (*pOwnMaterial2 < *pNeedMaterial2)
	{
		bMake = false;

		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f * 0.8f;
		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f * 0.2f;
		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 255.f * 0.2f;

	}
	if ((*pOwnMaterial0 >= *pNeedMaterial0) || (*pOwnMaterial1 >= *pNeedMaterial1) || (*pOwnMaterial2 >= *pNeedMaterial2))
	{
		bMake = true;

		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;
		m_0MaterialSlot[4].fColorR = m_0MaterialSlot[5].fColorR = m_0MaterialSlot[6].fColorR = 0.f;

		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;
		m_1MaterialSlot[4].fColorR = m_1MaterialSlot[5].fColorR = m_1MaterialSlot[6].fColorR = 0.f;

		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;
		m_2MaterialSlot[4].fColorR = m_2MaterialSlot[5].fColorR = m_2MaterialSlot[6].fColorR = 0.f;

	}


}

void CUI_Cooking::SettingTotal(_int MakeNum) //�� ����
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string(MakeNum);
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_FinalList[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_FinalList[7].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}
	}


	if (100 > MakeNum)
	{
		m_FinalList[6].bRender = true;
		m_FinalList[7].bRender = true;
		m_FinalList[8].bRender = false;
		m_FinalList[9].bRender = false;
	}
	if (10 > MakeNum)
	{
		m_FinalList[6].bRender = true;
		m_FinalList[7].bRender = false;
		m_FinalList[8].bRender = false;
		m_FinalList[9].bRender = false;
	}

}

void CUI_Cooking::SettingOwnMaterial0(_int* pFood) //������� ����
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
	}

}

void CUI_Cooking::SettingOwnMaterial1(_int* pFood) //������� ����
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
	}

}

void CUI_Cooking::SettingOwnMaterial2(_int* pFood) //������� ����
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
	}
	
}


void CUI_Cooking::SettingNeedMaterial0(_int* pFood) //�ʿ���� ����
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
	}
}

void CUI_Cooking::SettingNeedMaterial1(_int* pFood) //�ʿ���� ����
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
	}
}

void CUI_Cooking::SettingNeedMaterial2(_int* pFood) //�ʿ���� ����
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
	}
}


void CUI_Cooking::SettingOwnNum(_int* pFood) // ���� �丮 ��
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
	}
}


void CUI_Cooking::SelectSlot()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_0Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			// ������ �������� ��,
			pSelectSlot = &m_0Slot[2];
		}
	}
	if (SelectUI(&m_1Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_1Slot[2];
		}
	}
	if (SelectUI(&m_2Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_2Slot[2];
		}
	}
	if (SelectUI(&m_3Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_3Slot[2];
		}
	}
	if (SelectUI(&m_4Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_4Slot[2];
		}
	}
	if (SelectUI(&m_5Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_5Slot[2];
		}
	}
	if (SelectUI(&m_6Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_6Slot[2];
		}
	}
	if (SelectUI(&m_7Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_7Slot[2];
		}
	}
	if (SelectUI(&m_8Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_8Slot[2];
		}
	}
	if (SelectUI(&m_9Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_9Slot[2];
		}
	}
	if (SelectUI(&m_10Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_10Slot[2];
		}
	}
	if (SelectUI(&m_11Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_11Slot[2];
		}
	}
	if (SelectUI(&m_12Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_12Slot[2];
		}
	}
	if (SelectUI(&m_13Slot[0]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			pSelectSlot = &m_13Slot[2];
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
			GradeIndex = 0;
		}
	}
	if (SelectUI(&m_GradeList[2]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			GradeIndex = 1;
		}
	}
	if (SelectUI(&m_GradeList[4]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			GradeIndex = 2;
		}
	}
	if (SelectUI(&m_GradeList[6]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			GradeIndex = 3;
		}
	}
	if (SelectUI(&m_GradeList[8]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			GradeIndex = 4;
		}
	}
	if (SelectUI(&m_GradeList[10]))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
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
		m_GradeList[10].iTexNum= 54;
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
		m_12Slot[2].iTexNum = STATIC_IMAGE::ICON_PORKBELLY;
		m_13Slot[2].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES;

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
		m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_PORKBELLY;
		m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_DUCKNOODLES;

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
{// â ���� �� ��ü ����
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
		m_MenuRenderStart = false;
}

_bool CUI_Cooking::InMenuEnd(_double TimeDelta)
{// â ���� �� ��ü ����
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
		// ��� �� �丮 ����
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
		break;
		}
	}
	case Client::CUI_Cooking::READYCONFRIM:
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

	}
	break;
	}


	return S_OK;
}

void CUI_Cooking::RenderGUI()
{
	ImGui::Begin("Souvenir ID");
	ImGui::InputInt("ID", &m_iObjectID);

	if (ImGui::Button("SouSave")) { Save(); }
	ImGui::SameLine();
	if (ImGui::Button("SouLoad")) { Load(); }
	ImGui::NewLine();

	if (ImGui::InputInt("SouSituation", &iSituation))
	{
		Situation = (COOKSITUINDEX)iSituation;
	}
	ImGui::InputInt("SouListType", &type);
	ImGui::InputInt("SouIn_List_Index", &m_Index);
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::InputInt("GradeIndex", &GradeIndex);
	ImGui::NewLine();


	switch (type)
	{

	case 0:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_Cookist.size()))
			CurrentDesc = &m_Cookist[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_Cookist.push_back(Desc);
		}
	}
	break;

	case 1:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_CommonList.size()))
			CurrentDesc = &m_CommonList[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_CommonList.push_back(Desc);
		}
	}
	break;
	case 2:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_GradeList.size()))
			CurrentDesc = &m_GradeList[m_Index];

		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_GradeList.push_back(Desc);
		}
	}
	break;
	case 3:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_DetailsList.size()))
			CurrentDesc = &m_DetailsList[m_Index];

		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_DetailsList.push_back(Desc);
		}
	}
	break;
	case 4:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_0MaterialSlot.size()))
			CurrentDesc = &m_0MaterialSlot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_0MaterialSlot.push_back(Desc);
		}
	}
	break;
	case 5:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_1MaterialSlot.size()))
			CurrentDesc = &m_1MaterialSlot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_1MaterialSlot.push_back(Desc);
		}
	}
	break;
	case 6:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_2MaterialSlot.size()))
			CurrentDesc = &m_2MaterialSlot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_2MaterialSlot.push_back(Desc);
		}
	}
	break;
	case 7:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_LButtonList.size()))
			CurrentDesc = &m_LButtonList[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_LButtonList.push_back(Desc);
		}
	}
	break;
	case 8:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_RButtonList.size()))
			CurrentDesc = &m_RButtonList[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_RButtonList.push_back(Desc);
		}
	}
	break;
	case 9:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_FinalList.size()))
			CurrentDesc = &m_FinalList[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_FinalList.push_back(Desc);
		}
	}
	break;

	case 10:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_0Slot.size()))
			CurrentDesc = &m_0Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_0Slot.push_back(Desc);
		}
	}
	break;
	case 11:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_1Slot.size()))
			CurrentDesc = &m_1Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_1Slot.push_back(Desc);
		}
	}
	break;
	case 12:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_2Slot.size()))
			CurrentDesc = &m_2Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_2Slot.push_back(Desc);
		}
	}
	break;
	case 13:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_3Slot.size()))
			CurrentDesc = &m_3Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_3Slot.push_back(Desc);
		}
	}
	break;
	case 14:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_4Slot.size()))
			CurrentDesc = &m_4Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_4Slot.push_back(Desc);
		}
	}
	break;
	case 15:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_5Slot.size()))
			CurrentDesc = &m_5Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_5Slot.push_back(Desc);
		}
	}
	break;
	case 16:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_6Slot.size()))
			CurrentDesc = &m_6Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_6Slot.push_back(Desc);
		}
	}
	break;
	case 17:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_7Slot.size()))
			CurrentDesc = &m_7Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_7Slot.push_back(Desc);
		}
	}
	break;
	case 18:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_8Slot.size()))
			CurrentDesc = &m_8Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_8Slot.push_back(Desc);
		}
	}
	break;
	case 19:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_9Slot.size()))
			CurrentDesc = &m_9Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_9Slot.push_back(Desc);
		}
	}
	break;
	case 20:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_10Slot.size()))
			CurrentDesc = &m_10Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_10Slot.push_back(Desc);
		}
	}
	break;
	case 21:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_11Slot.size()))
			CurrentDesc = &m_11Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_11Slot.push_back(Desc);
		}
	}
	break;
	case 22:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_12Slot.size()))
			CurrentDesc = &m_12Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_12Slot.push_back(Desc);
		}
	}
	break;
	case 23:
	{
		if ((-1 < m_Index) && (m_Index < (_int)m_13Slot.size()))
			CurrentDesc = &m_13Slot[m_Index];
		if (ImGui::Button("New"))
		{
			COOKDESC Desc;
			ZeroMemory(&Desc, sizeof(COOKDESC));
			m_13Slot.push_back(Desc);
		}
	}
	break;

	}




	if (nullptr != CurrentDesc)
	{
		ImGui::InputInt("pass", &CurrentDesc->iPass);
		ImGui::InputInt("texNum", &CurrentDesc->iTexNum);
		bRender = (_int)CurrentDesc->bRender;
		if (ImGui::InputInt("bRender", &bRender))
		{
			if (0 >= bRender)
			{
				CurrentDesc->bRender = false;
			}
			else
				CurrentDesc->bRender = true;
		}
		ImGui::NewLine();

		_float4 Pos = _float4{ CurrentDesc->fX, CurrentDesc->fY, CurrentDesc->fZ, 1.f };
		ImGui::InputFloat3("CookPos", &Pos.x, "%.2f");
		ImGui::SliderFloat("CookX", &Pos.x, -640.f, 640.f);
		ImGui::SliderFloat("CookY", &Pos.y, -360.f, 360.f);
		ImGui::SliderFloat("CookZ", &Pos.z, -1.f, 1.f);
		CurrentDesc->fX = CurrentDesc->WorldMatrix._41 = Pos.x;
		CurrentDesc->fY = CurrentDesc->WorldMatrix._42 = Pos.y;
		CurrentDesc->fZ = CurrentDesc->WorldMatrix._43 = Pos.z;
		ImGui::Separator();
		ImGui::Checkbox("bTextMod", &bTexMod);
		if (bTexMod)
		{
			ImGui::InputFloat("fmagnification", &magnification);
			ImGui::SliderFloat("magnification", &(magnification), 0.001f, 10.f);

			ImGui::InputFloat2("imageSize", &Size.x);
			if (ImGui::Button("TextMod", ImVec2(10.f, 10.f)))
			{
				CurrentDesc->fWidth = Size.x * magnification;
				CurrentDesc->fHeight = Size.y * magnification;
			}
		}
		else
		{
			ImGui::NewLine();

			ImGui::InputFloat("CookScaleX", &(CurrentDesc->fWidth));
			ImGui::NewLine();
			ImGui::InputFloat("CookScaleY", &(CurrentDesc->fHeight));
			ImGui::NewLine();
			ImGui::SliderFloat("sCookScaleX", &(CurrentDesc->fWidth), 0.1f, 1280.f);
			ImGui::NewLine();
			ImGui::SliderFloat("sCookScaleY", &(CurrentDesc->fHeight), 0.1f, 720.f);
			ImGui::NewLine();
		}
		XMStoreFloat4x4(&(CurrentDesc->WorldMatrix), XMMatrixScaling(CurrentDesc->fWidth, CurrentDesc->fHeight, 1.f)
			* XMMatrixTranslation(CurrentDesc->fX, CurrentDesc->fY, CurrentDesc->fZ));
		ImGui::Separator();
		//// ���İ�
		//CurrentDesc->Color.x = CurrentDesc->fColorR;
		//CurrentDesc->Color.y = CurrentDesc->fColorG;
		//CurrentDesc->Color.z = CurrentDesc->fColorB;
		//CurrentDesc->Color.w = CurrentDesc->fColorA;
		ImGui::InputFloat("CookR", &(CurrentDesc->fColorR));
		ImGui::InputFloat("CookG", &(CurrentDesc->fColorG));
		ImGui::InputFloat("CookB", &(CurrentDesc->fColorB));
		ImGui::InputFloat("CookA", &(CurrentDesc->fColorA));
		ImGui::SliderFloat("sCookR", &(CurrentDesc->fColorR), -255.f, 255.f);
		ImGui::SliderFloat("sCookG", &(CurrentDesc->fColorG), -255.f, 255.f);
		ImGui::SliderFloat("sCookB", &(CurrentDesc->fColorB), -255.f, 255.f);
		ImGui::SliderFloat("sCookA", &(CurrentDesc->fColorA), -255.f, 255.f);
	}

	ImGui::End();
}

_bool CUI_Cooking::SelectUI(COOKDESC* pDesc)
{
	_float3	fMousePos = m_pUIMouse->Get_MousePos();
	_vector vMouse = XMLoadFloat3(&fMousePos);
	_float Dist = 1.f;
	// ������ �� ������
	_vector P0 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P1 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P2 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P3 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);

	// UIũ�⿡ ���缭 ����üũ
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





void CUI_Cooking::IsMouseinRect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_0Slot[0]))
	{
		m_0Slot[8].bRender = true; //�÷�����
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_0Slot[8].bRender = false;

	if (SelectUI(&m_1Slot[0]))
	{
		m_1Slot[8].bRender = true; //�÷�����
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_1Slot[8].bRender = false;

	if (SelectUI(&m_2Slot[0]))
	{
		m_2Slot[8].bRender = true; //�÷�����
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{

		}
	}
	else
		m_2Slot[8].bRender = false;

	if (SelectUI(&m_3Slot[0]))
	{
		m_3Slot[8].bRender = true; //�÷�����
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
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
		//Desc.iPass = 21;
		Desc.bRender = true;
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
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
		Desc.bRender = true;
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
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
		Desc.bRender = true;
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_DetailsList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Cook0Slot%d.dat", i);
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_0Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_1Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_2Slot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_3Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_4Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_5Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_6Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_7Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_8Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_9Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_10Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_11Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_12Slot.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 5;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_13Slot.push_back(Desc);
		CloseHandle(hFile);

	}


	//
	index = 8;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_0MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 8;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_1MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 8;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_2MaterialSlot.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 6;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_LButtonList.push_back(Desc);
		CloseHandle(hFile);

	}

	//
	index = 3;
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
		//Desc.Color.w = Desc.fColorA;
		//Desc.fColorA = -255.f;
		//Desc.iPass = 22;
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
	if (3 == index)
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