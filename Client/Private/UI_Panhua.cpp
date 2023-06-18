#include "stdafx.h"
#include "..\Public\UI_Panhua.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "Input_Device.h"
#include "UI_Mouse.h"
#include "PlayerState.h"
#include "ItemDB.h"
#include "Item.h"
#include "Inventory.h"


CUI_Panhua::CUI_Panhua(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Panhua::CUI_Panhua(const CUI_Panhua& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Panhua::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Panhua::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	Load();
	// 위치이동 임시코드
	//
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

	for (_int i = 0; i< 9; ++i)
	{
		m_8Slot[i].fX		= m_7Slot[i].fX;
		m_8Slot[i].fY		= m_7Slot[i].fY - 82.f;
		m_8Slot[i].fZ		= m_7Slot[i].fZ;
		m_8Slot[i].iPass	= m_7Slot[i].iPass;
		m_8Slot[i].iTexNum	= m_7Slot[i].iTexNum;
		m_8Slot[i].fHeight	= m_7Slot[i].fHeight;
		m_8Slot[i].fWidth	= m_7Slot[i].fWidth;
		m_8Slot[i].fColorR	= m_7Slot[i].fColorR;
		m_8Slot[i].fColorG	= m_7Slot[i].fColorG;
		m_8Slot[i].fColorB	= m_7Slot[i].fColorB;
		m_8Slot[i].fColorA	= m_7Slot[i].fColorA;

		XMStoreFloat4x4(&(m_8Slot[i].WorldMatrix), XMMatrixScaling(m_8Slot[i].fWidth, m_8Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_8Slot[i].fX, m_8Slot[i].fY, m_8Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_9Slot[i].fX		= m_8Slot[i].fX;
		m_9Slot[i].fY		= m_8Slot[i].fY - 82.f;
		m_9Slot[i].fZ		= m_8Slot[i].fZ;
		m_9Slot[i].iPass	= m_8Slot[i].iPass;
		m_9Slot[i].iTexNum	= m_8Slot[i].iTexNum;
		m_9Slot[i].fHeight	= m_8Slot[i].fHeight;
		m_9Slot[i].fWidth	= m_8Slot[i].fWidth;
		m_9Slot[i].fColorR	= m_8Slot[i].fColorR;
		m_9Slot[i].fColorG	= m_8Slot[i].fColorG;
		m_9Slot[i].fColorB	= m_8Slot[i].fColorB;
		m_9Slot[i].fColorA	= m_8Slot[i].fColorA;

		XMStoreFloat4x4(&(m_9Slot[i].WorldMatrix), XMMatrixScaling(m_9Slot[i].fWidth, m_9Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_9Slot[i].fX, m_9Slot[i].fY, m_9Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_10Slot[i].fX		= m_9Slot[i].fX;
		m_10Slot[i].fY		= m_9Slot[i].fY - 82.f;
		m_10Slot[i].fZ		= m_9Slot[i].fZ;
		m_10Slot[i].iPass	= m_9Slot[i].iPass;
		m_10Slot[i].iTexNum	= m_9Slot[i].iTexNum;
		m_10Slot[i].fHeight	= m_9Slot[i].fHeight;
		m_10Slot[i].fWidth	= m_9Slot[i].fWidth;
		m_10Slot[i].fColorR	= m_9Slot[i].fColorR;
		m_10Slot[i].fColorG	= m_9Slot[i].fColorG;
		m_10Slot[i].fColorB	= m_9Slot[i].fColorB;
		m_10Slot[i].fColorA	= m_9Slot[i].fColorA;

		XMStoreFloat4x4(&(m_10Slot[i].WorldMatrix), XMMatrixScaling(m_10Slot[i].fWidth, m_10Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_10Slot[i].fX, m_10Slot[i].fY, m_10Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_11Slot[i].fX		= m_10Slot[i].fX;
		m_11Slot[i].fY		= m_10Slot[i].fY - 82.f;
		m_11Slot[i].fZ		= m_10Slot[i].fZ;
		m_11Slot[i].iPass	= m_10Slot[i].iPass;
		m_11Slot[i].iTexNum	= m_10Slot[i].iTexNum;
		m_11Slot[i].fHeight	= m_10Slot[i].fHeight;
		m_11Slot[i].fWidth	= m_10Slot[i].fWidth;
		m_11Slot[i].fColorR	= m_10Slot[i].fColorR;
		m_11Slot[i].fColorG	= m_10Slot[i].fColorG;
		m_11Slot[i].fColorB	= m_10Slot[i].fColorB;
		m_11Slot[i].fColorA	= m_10Slot[i].fColorA;

		XMStoreFloat4x4(&(m_11Slot[i].WorldMatrix), XMMatrixScaling(m_11Slot[i].fWidth, m_11Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_11Slot[i].fX, m_11Slot[i].fY, m_11Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_12Slot[i].fX		= m_11Slot[i].fX;
		m_12Slot[i].fY		= m_11Slot[i].fY - 82.f;
		m_12Slot[i].fZ		= m_11Slot[i].fZ;
		m_12Slot[i].iPass	= m_11Slot[i].iPass;
		m_12Slot[i].iTexNum = m_11Slot[i].iTexNum;
		m_12Slot[i].fHeight = m_11Slot[i].fHeight;
		m_12Slot[i].fWidth	= m_11Slot[i].fWidth;
		m_12Slot[i].fColorR = m_11Slot[i].fColorR;
		m_12Slot[i].fColorG = m_11Slot[i].fColorG;
		m_12Slot[i].fColorB = m_11Slot[i].fColorB;
		m_12Slot[i].fColorA = m_11Slot[i].fColorA;

		XMStoreFloat4x4(&(m_12Slot[i].WorldMatrix), XMMatrixScaling(m_12Slot[i].fWidth, m_12Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_12Slot[i].fX, m_12Slot[i].fY, m_12Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_13Slot[i].fX		= m_12Slot[i].fX;
		m_13Slot[i].fY		= m_12Slot[i].fY - 82.f;
		m_13Slot[i].fZ		= m_12Slot[i].fZ;
		m_13Slot[i].iPass	= m_12Slot[i].iPass;
		m_13Slot[i].iTexNum = m_12Slot[i].iTexNum;
		m_13Slot[i].fHeight = m_12Slot[i].fHeight;
		m_13Slot[i].fWidth  = m_12Slot[i].fWidth;
		m_13Slot[i].fColorR = m_12Slot[i].fColorR;
		m_13Slot[i].fColorG = m_12Slot[i].fColorG;
		m_13Slot[i].fColorB = m_12Slot[i].fColorB;
		m_13Slot[i].fColorA = m_12Slot[i].fColorA;

		XMStoreFloat4x4(&(m_13Slot[i].WorldMatrix), XMMatrixScaling(m_13Slot[i].fWidth, m_13Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_13Slot[i].fX, m_13Slot[i].fY, m_13Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_14Slot[i].fX		= m_13Slot[i].fX;
		m_14Slot[i].fY		= m_13Slot[i].fY - 82.f;
		m_14Slot[i].fZ		= m_13Slot[i].fZ;
		m_14Slot[i].iPass   = m_13Slot[i].iPass;
		m_14Slot[i].iTexNum = m_13Slot[i].iTexNum;
		m_14Slot[i].fHeight = m_13Slot[i].fHeight;
		m_14Slot[i].fWidth  = m_13Slot[i].fWidth;
		m_14Slot[i].fColorR = m_13Slot[i].fColorR;
		m_14Slot[i].fColorG = m_13Slot[i].fColorG;
		m_14Slot[i].fColorB = m_13Slot[i].fColorB;
		m_14Slot[i].fColorA = m_13Slot[i].fColorA;

		XMStoreFloat4x4(&(m_14Slot[i].WorldMatrix), XMMatrixScaling(m_14Slot[i].fWidth, m_14Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_14Slot[i].fX, m_14Slot[i].fY, m_14Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_15Slot[i].fX		= m_14Slot[i].fX;
		m_15Slot[i].fY		= m_14Slot[i].fY - 82.f;
		m_15Slot[i].fZ		= m_14Slot[i].fZ;
		m_15Slot[i].iPass   = m_14Slot[i].iPass;
		m_15Slot[i].iTexNum = m_14Slot[i].iTexNum;
		m_15Slot[i].fHeight = m_14Slot[i].fHeight;
		m_15Slot[i].fWidth  = m_14Slot[i].fWidth;
		m_15Slot[i].fColorR = m_14Slot[i].fColorR;
		m_15Slot[i].fColorG = m_14Slot[i].fColorG;
		m_15Slot[i].fColorB = m_14Slot[i].fColorB;
		m_15Slot[i].fColorA = m_14Slot[i].fColorA;

		XMStoreFloat4x4(&(m_15Slot[i].WorldMatrix), XMMatrixScaling(m_15Slot[i].fWidth, m_15Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_15Slot[i].fX, m_15Slot[i].fY, m_15Slot[i].fZ));
	}
	for (_int i = 0; i< 9; ++i)
	{
		m_16Slot[i].fX		= m_15Slot[i].fX;
		m_16Slot[i].fY		= m_15Slot[i].fY - 82.f;
		m_16Slot[i].fZ		= m_15Slot[i].fZ;
		m_16Slot[i].iPass   = m_15Slot[i].iPass;
		m_16Slot[i].iTexNum = m_15Slot[i].iTexNum;
		m_16Slot[i].fHeight = m_15Slot[i].fHeight;
		m_16Slot[i].fWidth  = m_15Slot[i].fWidth;
		m_16Slot[i].fColorR = m_15Slot[i].fColorR;
		m_16Slot[i].fColorG = m_15Slot[i].fColorG;
		m_16Slot[i].fColorB = m_15Slot[i].fColorB;
		m_16Slot[i].fColorA = m_15Slot[i].fColorA;

		XMStoreFloat4x4(&(m_16Slot[i].WorldMatrix), XMMatrixScaling(m_16Slot[i].fWidth, m_16Slot[i].fHeight, 1.f)
			* XMMatrixTranslation(m_16Slot[i].fX, m_16Slot[i].fY, m_16Slot[i].fZ));
	}

 // 설명0 ,타입
m_DetailsList[31].fX = -338.f;
m_DetailsList[31].fY = 108.87f;
m_DetailsList[31].fWidth = 460.8f;
m_DetailsList[31].fHeight = 129.6f;
m_DetailsList[33].fX = -488.f;

	m_pDB = CItemDB::GetInstance();
	// 고정 박스, 보상메세지 박스 색상
	itemDesc[0] = m_pDB->GetItemData(ITEM::RICE);		//cost 100
	itemDesc[1] = m_pDB->GetItemData(ITEM::TOFU);		//cost 100
	itemDesc[2] = m_pDB->GetItemData(ITEM::FLOUR);		//cost 100
	itemDesc[3] = m_pDB->GetItemData(ITEM::MUSHROOM);	//cost 100
	itemDesc[4] = m_pDB->GetItemData(ITEM::CHICKEN);	//cost 100
	itemDesc[5] = m_pDB->GetItemData(ITEM::EGG);		//cost 100
	itemDesc[6] = m_pDB->GetItemData(ITEM::MEAT);		//cost 100
	itemDesc[7] = m_pDB->GetItemData(ITEM::HERB);		//cost 100
	itemDesc[8] = m_pDB->GetItemData(ITEM::VIOLET);		//cost 100
	itemDesc[9] = m_pDB->GetItemData(ITEM::DANDELION);  //cost 100
	itemDesc[10] = m_pDB->GetItemData(ITEM::PEPPER);	//cost 100
	itemDesc[11] = m_pDB->GetItemData(ITEM::SUGAR);		//cost 100
	itemDesc[12] = m_pDB->GetItemData(ITEM::SALT);		//cost 100
	itemDesc[13] = m_pDB->GetItemData(ITEM::OIL);		//cost 100
	itemDesc[14] = m_pDB->GetItemData(ITEM::SOYSAUCE);		//cost 100
	itemDesc[15] = m_pDB->GetItemData(ITEM::RECIPE0);		//cost 100
	itemDesc[16] = m_pDB->GetItemData(ITEM::RECIPE1);		//cost 100

	m_MenuList[0].fX = m_MenuList[3].fX;
	m_MenuList[1].fX = m_MenuList[4].fX;

	m_MenuList[0].OriPos = _float2(m_MenuList[0].fX, m_MenuList[0].fY);
	m_MenuList[0].AimPos = _float2(m_MenuList[0].fX - 10.f, m_MenuList[0].fY);

	m_MenuList[1].OriPos = _float2(m_MenuList[1].fX, m_MenuList[1].fY);
	m_MenuList[1].AimPos = _float2(m_MenuList[1].fX - 10.f, m_MenuList[1].fY);
	m_MenuList[2].iTexNum = 50;

	m_DetailsList[22].iTexNum = m_CommonList[7].iTexNum = 16;

	_float3 color0 = SLOT_COLOR;
	m_0Slot[1].fColorR = -(255.f - (color0.x * 255.f));
	m_0Slot[1].fColorG = -(255.f - (color0.y * 255.f));
	m_0Slot[1].fColorB = -(255.f - (color0.z * 255.f));
	m_0Slot[2].iTexNum = STATIC_IMAGE::ICON_RICE; // 아이콘
	m_0Slot[5].iTexNum = 80; // 이름
	m_0Slot[4].iTexNum = 67;					  // 수량
	m_0Slot[6].iTexNum = 16;					  // 재화 아이콘
	m_0Slot[7].iTexNum = 68;					  // 가격 이미지
	_float3 color1 = SLOT_COLOR;
	m_1Slot[1].fColorR = -(255.f - (color1.x * 255.f));
	m_1Slot[1].fColorG = -(255.f - (color1.y * 255.f));
	m_1Slot[1].fColorB = -(255.f - (color1.z * 255.f));
	m_1Slot[2].iTexNum = STATIC_IMAGE::ICON_TOFU;
	m_1Slot[5].iTexNum = 83;
	m_1Slot[4].iTexNum = 67;
	m_1Slot[6].iTexNum = 16;
	m_1Slot[7].iTexNum = 68;
	_float3 color2 = SLOT_COLOR;
	m_2Slot[1].fColorR = -(255.f - (color2.x * 255.f));
	m_2Slot[1].fColorG = -(255.f - (color2.y * 255.f));
	m_2Slot[1].fColorB = -(255.f - (color2.z * 255.f));
	m_2Slot[2].iTexNum = STATIC_IMAGE::ICON_FLOUR;
	m_2Slot[5].iTexNum = 86;
	m_2Slot[4].iTexNum = 67;
	m_2Slot[6].iTexNum = 16;
	m_2Slot[7].iTexNum = 68;
	_float3 color3 = SLOT_COLOR;
	m_3Slot[1].fColorR = -(255.f - (color3.x * 255.f));
	m_3Slot[1].fColorG = -(255.f - (color3.y * 255.f));
	m_3Slot[1].fColorB = -(255.f - (color3.z * 255.f));
	m_3Slot[2].iTexNum = STATIC_IMAGE::ICON_MUSHROOM;
	m_3Slot[5].iTexNum = 89;
	m_3Slot[4].iTexNum = 67;
	m_3Slot[6].iTexNum = 16;
	m_3Slot[7].iTexNum = 68;
	_float3 color4 = SLOT_COLOR;
	m_4Slot[1].fColorR = -(255.f - (color4.x * 255.f));
	m_4Slot[1].fColorG = -(255.f - (color4.y * 255.f));
	m_4Slot[1].fColorB = -(255.f - (color4.z * 255.f));
	m_4Slot[2].iTexNum = STATIC_IMAGE::ICON_CHICKEN;
	m_4Slot[5].iTexNum = 92;
	m_4Slot[4].iTexNum = 67; 
	m_4Slot[6].iTexNum = 16;
	m_4Slot[7].iTexNum = 68;
	_float3 color5 = SLOT_COLOR;
	m_5Slot[1].fColorR = -(255.f - (color5.x * 255.f));
	m_5Slot[1].fColorG = -(255.f - (color5.y * 255.f));
	m_5Slot[1].fColorB = -(255.f - (color5.z * 255.f));
	m_5Slot[2].iTexNum = STATIC_IMAGE::ICON_EGG;
	m_5Slot[5].iTexNum = 95;
	m_5Slot[4].iTexNum = 67;
	m_5Slot[6].iTexNum = 16;
	m_5Slot[7].iTexNum = 68;
	_float3 color6 = SLOT_ADVANCED_COLOR;
	m_6Slot[1].fColorR = -(255.f - (color6.x * 255.f));
	m_6Slot[1].fColorG = -(255.f - (color6.y * 255.f));
	m_6Slot[1].fColorB = -(255.f - (color6.z * 255.f));
	m_6Slot[2].iTexNum = STATIC_IMAGE::ICON_MEAT;
	m_6Slot[5].iTexNum = 98;
	m_6Slot[4].iTexNum = 67;
	m_6Slot[6].iTexNum = 16;
	m_6Slot[7].iTexNum = 69;
	_float3 color7 = SLOT_ADVANCED_COLOR;
	m_7Slot[1].fColorR = -(255.f - (color7.x * 255.f));
	m_7Slot[1].fColorG = -(255.f - (color7.y * 255.f));
	m_7Slot[1].fColorB = -(255.f - (color7.z * 255.f));
	m_7Slot[2].iTexNum = STATIC_IMAGE::ICON_HERB;
	m_7Slot[5].iTexNum = 101;
	m_7Slot[4].iTexNum = 67;
	m_7Slot[6].iTexNum = 16;
	m_7Slot[7].iTexNum = 69;
	_float3 color8 = SLOT_ADVANCED_COLOR;
	m_8Slot[1].fColorR = -(255.f - (color8.x * 255.f));
	m_8Slot[1].fColorG = -(255.f - (color8.y * 255.f));
	m_8Slot[1].fColorB = -(255.f - (color8.z * 255.f));
	m_8Slot[2].iTexNum = STATIC_IMAGE::ICON_VIOLET;
	m_8Slot[5].iTexNum = 104;
	m_8Slot[4].iTexNum = 67;
	m_8Slot[6].iTexNum = 16;
	m_8Slot[7].iTexNum = 69;
	_float3 color9 = SLOT_ADVANCED_COLOR;
	m_9Slot[1].fColorR = -(255.f - (color9.x * 255.f));
	m_9Slot[1].fColorG = -(255.f - (color9.y * 255.f));
	m_9Slot[1].fColorB = -(255.f - (color9.z * 255.f));
	m_9Slot[2].iTexNum = STATIC_IMAGE::ICON_DANDELION;
	m_9Slot[5].iTexNum = 107;
	m_9Slot[4].iTexNum = 67;
	m_9Slot[6].iTexNum = 16;
	m_9Slot[7].iTexNum = 69;
	_float3 color10 = SLOT_RARE_COLOR;
	m_10Slot[1].fColorR = -(255.f - (color10.x * 255.f));
	m_10Slot[1].fColorG = -(255.f - (color10.y * 255.f));
	m_10Slot[1].fColorB = -(255.f - (color10.z * 255.f));
	m_10Slot[2].iTexNum = STATIC_IMAGE::ICON_PEPPER;
	m_10Slot[5].iTexNum = 110;
	m_10Slot[4].iTexNum = 67;
	m_10Slot[6].iTexNum = 16;
	m_10Slot[7].iTexNum = 70;
	_float3 color11 = SLOT_RARE_COLOR;
	m_11Slot[1].fColorR = -(255.f - (color11.x * 255.f));
	m_11Slot[1].fColorG = -(255.f - (color11.y * 255.f));
	m_11Slot[1].fColorB = -(255.f - (color11.z * 255.f));
	m_11Slot[2].iTexNum = STATIC_IMAGE::ICON_SUGAR;
	m_11Slot[5].iTexNum = 113;
	m_11Slot[4].iTexNum = 67;
	m_11Slot[6].iTexNum = 16;
	m_11Slot[7].iTexNum = 70;
	_float3 color12 = SLOT_RARE_COLOR;
	m_12Slot[1].fColorR = -(255.f - (color12.x * 255.f));
	m_12Slot[1].fColorG = -(255.f - (color12.y * 255.f));
	m_12Slot[1].fColorB = -(255.f - (color12.z * 255.f));
	m_12Slot[2].iTexNum = STATIC_IMAGE::ICON_SALT;
	m_12Slot[5].iTexNum = 116;
	m_12Slot[4].iTexNum = 67;
	m_12Slot[6].iTexNum = 16;
	m_12Slot[7].iTexNum = 70;
	_float3 color13 = SLOT_RARE_COLOR;
	m_13Slot[1].fColorR = -(255.f - (color13.x * 255.f));
	m_13Slot[1].fColorG = -(255.f - (color13.y * 255.f));
	m_13Slot[1].fColorB = -(255.f - (color13.z * 255.f));
	m_13Slot[2].iTexNum = STATIC_IMAGE::ICON_OIL;
	m_13Slot[5].iTexNum = 119;
	m_13Slot[4].iTexNum = 67;
	m_13Slot[6].iTexNum = 16;
	m_13Slot[7].iTexNum = 70;
	_float3 color14 = SLOT_RARE_COLOR;
	m_14Slot[1].fColorR = -(255.f - (color14.x * 255.f));
	m_14Slot[1].fColorG = -(255.f - (color14.y * 255.f));
	m_14Slot[1].fColorB = -(255.f - (color14.z * 255.f));
	m_14Slot[2].iTexNum = STATIC_IMAGE::ICON_SOYSAUCE;
	m_14Slot[5].iTexNum = 122;
	m_14Slot[4].iTexNum = 67;
	m_14Slot[6].iTexNum = 16;
	m_14Slot[7].iTexNum = 70;
	_float3 color15 = SLOT_LEGEND_COLOR;
	m_15Slot[1].fColorR = -(255.f - (color15.x * 255.f));
	m_15Slot[1].fColorG = -(255.f - (color15.y * 255.f));
	m_15Slot[1].fColorB = -(255.f - (color15.z * 255.f));
	m_15Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE;
	m_15Slot[5].iTexNum = 125;
	m_15Slot[4].iTexNum = 66;
	m_15Slot[6].iTexNum = 16;
	m_15Slot[7].iTexNum = 72;
	_float3 color16 = SLOT_LEGEND_COLOR;
	m_16Slot[1].fColorR = -(255.f - (color16.x * 255.f));
	m_16Slot[1].fColorG = -(255.f - (color16.y * 255.f));
	m_16Slot[1].fColorB = -(255.f - (color16.z * 255.f));
	m_16Slot[2].iTexNum = STATIC_IMAGE::ICON_RECIPE;
	m_16Slot[5].iTexNum = 128;
	m_16Slot[4].iTexNum = 66;
	m_16Slot[6].iTexNum = 16;
	m_16Slot[7].iTexNum = 72;

	return S_OK;
}

void CUI_Panhua::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	
	SetState(DISABLE);
}

void CUI_Panhua::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_NPCbye)
		Set_END();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameMode* pGM = CGameMode::GetInstance();
	CCharacter* pActiveCharacter = m_pPlayerStateClass->Get_ActiveCharacter();

	switch (Situation)
	{
	case Client::CUI_Panhua::MEET:
	{
		m_pUIMouse->Set_RenderMouse(true);
		pGM->SetMouseActive(true);

		if (SelectUI(&m_PanList[0]))
		{
			if (false == m_PanList[0].OnRect)
			{
				for (auto& Sou : m_PanList)
				{
					Sou.fColorA = Sou.Color.w;
				}
			}
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_PanList[0].OnRect = true;
			}
		}

		if (true == m_PanList[0].OnRect)
		{
			if (MinusAlphaW(&m_PanList, TimeDelta))
			{
				pActiveCharacter->Set_OnControl(false);
				m_PanList[0].OnRect = false;
				Situation = CUI_Panhua::MENU;
			}
		}
		else
		{
			AddAlphaW(&m_PanList, TimeDelta);
		}
	}
		break;
	case Client::CUI_Panhua::MENU:
	{
		if (m_MenuRenderStart)
		{
			if (AddAlphaW(&m_MenuList, TimeDelta))
				m_MenuRenderStart = false;
		}
		if (SelectUI(&m_MenuList[0])) // 랜더 도중에 메뉴에 마우스를 올리면
		{
			Start_Move(0, 3,  TimeDelta);
			End_Move(3, 6, TimeDelta);
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
			Start_Move(3, 6, TimeDelta);
			End_Move(0, 3, TimeDelta);
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
				pActiveCharacter->Set_OnControl(true);
			}
		}
		else
		{
			End_Move(0, 6, TimeDelta);
		}

		if (true == m_MenuList[0].OnRect) // 인메뉴로 이동
		{
			if (MinusAlphaW(&m_MenuList, TimeDelta))
			{
				m_MenuRenderStart = true;
				m_MenuList[0].OnRect = false;
				Situation = CUI_Panhua::INMENU;
			}
		}
		if (true == m_MenuList[3].OnRect) // 떠나다
		{
			if (MinusAlphaW(&m_MenuList, TimeDelta))
			{
				m_MenuRenderStart = true;
				m_MenuList[3].OnRect = false;
				SetState(DISABLE);
				pGM->SetMouseActive(false);
			}
		}
	}
		break;
	case Client::CUI_Panhua::INMENU: ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		if (m_bOverPurchase)
			SettingOverPurchase();
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

		// 슬롯 위에 올리면 테두리, 리미트 셋팅, 디테일 설명이미지 셋팅
		IsMouseinRect();

		// 코인 현재 보유 수량
		SettingCurMoneyTexNum();

		// 캔슬 누르면 목록으로 돌아가기, 텍스처 바꾸기
		if (SelectUI(&m_CommonList[4]))
		{
			m_CommonList[4].iTexNum = 8;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_InMenuRenderStart = false;
				Situation = CUI_Panhua::MENU;
			}
		}
		else
			m_CommonList[4].iTexNum = 7;
		if (CUI_Panhua::MENU == Situation)
		{
			MinusAlphaW(&m_CommonList, TimeDelta);
		}
	}
		break;
	case Client::CUI_Panhua::DETAILS: //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		if (m_DetailRenderStart)
		{
			(AddAlphaW(&m_DetailsList, TimeDelta) ? m_DetailRenderStart = false : m_DetailRenderStart = true);
		}
		// 선택아이템 현재 수량
		SettingCurOwnTexNum();
		// 구매제한 수량
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

			(Limited - (*pLimibuycount) > BuyNum ? ++BuyNum : BuyNum = Limited - (*pLimibuycount));

			if (Limited - (*pLimibuycount) == BuyNum)
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

				(Limited - (*pLimibuycount) > BuyNum ? ++BuyNum : BuyNum = Limited - (*pLimibuycount));

				if (Limited - (*pLimibuycount) == BuyNum)
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
				Situation = CUI_Panhua::INMENU;
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
				Situation = CUI_Panhua::INMENU;
				(m_DetailsList[13].bRender == false ? m_DetailsList[13].bRender = true : m_DetailsList[13].bRender = true);
				(m_DetailsList[14].bRender == false ? m_DetailsList[14].bRender = true : m_DetailsList[14].bRender = true);
				(m_DetailsList[15].bRender == false ? m_DetailsList[15].bRender = true : m_DetailsList[15].bRender = true);
				(m_DetailsList[16].bRender == false ? m_DetailsList[16].bRender = true : m_DetailsList[16].bRender = true);
			}
		}
		else
			m_DetailsList[26].iTexNum = 21;


		if (CUI_Panhua::INMENU == Situation)
		{
			MinusAlphaW(&m_DetailsList, TimeDelta);
		}

		// 구매 누르면 확인창 뜨기,테스처 바꾸기
		if ((SelectUI(&m_DetailsList[27])) && (0<BuyNum) &&(Limited - (*pLimibuycount)>= BuyNum) && (CurrentMoney >= iTotal) &&(Limited != (*pLimibuycount)))
		{
			m_DetailsList[27].iTexNum = 20;
			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				(*pLimibuycount) += BuyNum;
				//제한개수와 누적구매가 같다면
				(Limited == (*pLimibuycount) ? m_bOverPurchase = true : false);
				Situation = CUI_Panhua::CONFRIM;
				(m_DetailsList[13].bRender == false ? m_DetailsList[13].bRender = true : m_DetailsList[13].bRender = true);
				(m_DetailsList[14].bRender == false ? m_DetailsList[14].bRender = true : m_DetailsList[14].bRender = true);
				(m_DetailsList[15].bRender == false ? m_DetailsList[15].bRender = true : m_DetailsList[15].bRender = true);
				(m_DetailsList[16].bRender == false ? m_DetailsList[16].bRender = true : m_DetailsList[16].bRender = true);
			}
		}
		else
			m_DetailsList[27].iTexNum = 21;



		if (Limited == (*pLimibuycount))
		{
			for (_int i = 12; i < 26; ++i)
			{
				m_DetailsList[i].bRender = false;
			}
			m_DetailsList[27].iTexNum = 22;
		}
		else
		{
			for (_int i = 12; i < 17; ++i)
			{
				m_DetailsList[i].bRender = true;
			}
			for (_int i = 19; i < 23; ++i)
			{
				m_DetailsList[i].bRender = true;
			}
		}
	

	}
		break;
	case Client::CUI_Panhua::CONFRIM:
	{
		// 아이템 아이콘, 박스 색상
		m_FinalList[4].iTexNum = (*pSelectSlot)[2].iTexNum;
		m_FinalList[3].fColorR = (*pSelectSlot)[1].fColorR;
		m_FinalList[3].fColorG = (*pSelectSlot)[1].fColorG;
		m_FinalList[3].fColorB = (*pSelectSlot)[1].fColorB;

		// 구매수량
		if ((pSelectSlot != &m_15Slot) && (pSelectSlot != &m_16Slot))
		{
			for (_int i = 8; i < 10; ++i)
			{
				m_FinalList[i].bRender = false;
			}
			m_FinalList[6].iTexNum = (BuyNum*10)/10 + 27;
			m_FinalList[7].iTexNum = 27;
		}
		else
		{
			for (_int i = 7; i < 10; ++i)
			{
				m_FinalList[i].bRender = false;
			}
			m_FinalList[6].iTexNum = BuyNum + 27;
		}
		if (m_ConfirmRenderStart)
			(AddAlphaW(&m_FinalList, TimeDelta) ? m_ConfirmRenderStart = false : m_ConfirmRenderStart = true);

		if (SelectUI(&m_FinalList[10]))
		{
			ColorP(&m_FinalList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
			ColorP(&m_FinalList[11], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);

			if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_ConfirmRenderStart = true;
				Situation = CUI_Panhua::BYE;
			}
		}
		else
		{
			ColorM(&m_FinalList[10], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
			ColorM(&m_FinalList[11], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
		}

	}
		break;
	case Client::CUI_Panhua::BYE:
	{
		// 인벤토리 처리 후 목록으로
		// 보상 인벤에 넣기
		if(pSelectSlot == &m_0Slot)
			m_pInven->AddItem(itemDesc[0].iItemID, BuyNum*10);
		if (pSelectSlot == &m_1Slot)
			m_pInven->AddItem(itemDesc[1].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_2Slot)
			m_pInven->AddItem(itemDesc[2].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_3Slot)
			m_pInven->AddItem(itemDesc[3].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_4Slot)
			m_pInven->AddItem(itemDesc[4].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_5Slot)
			m_pInven->AddItem(itemDesc[5].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_6Slot)
			m_pInven->AddItem(itemDesc[6].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_7Slot)
			m_pInven->AddItem(itemDesc[7].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_8Slot)
			m_pInven->AddItem(itemDesc[8].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_9Slot)
			m_pInven->AddItem(itemDesc[9].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_10Slot)
			m_pInven->AddItem(itemDesc[10].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_11Slot)
			m_pInven->AddItem(itemDesc[11].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_12Slot)
			m_pInven->AddItem(itemDesc[12].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_13Slot)
			m_pInven->AddItem(itemDesc[13].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_14Slot)
			m_pInven->AddItem(itemDesc[14].iItemID, BuyNum * 10);
		if (pSelectSlot == &m_15Slot)
			m_pInven->AddItem(itemDesc[15].iItemID, BuyNum);
		if (pSelectSlot == &m_16Slot)
			m_pInven->AddItem(itemDesc[16].iItemID, BuyNum);

		m_pInven->DeleteCoin(iTotal);
		SettingCurMoneyTexNum();
		BuyNum = 0;
		Situation = CUI_Panhua::INMENU;
	}
		break;
	}
}


void CUI_Panhua::InMenuOpen(_double TimeDelta)
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
	if (AddAlphaW(&m_14Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_15Slot, TimeDelta))
		++Count;
	if (AddAlphaW(&m_16Slot, TimeDelta))
		++Count;
	if (18 == Count)
		m_InMenuRenderStart = false;
}

_bool CUI_Panhua::InMenuEnd(_double TimeDelta)
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
	if (MinusAlphaW(&m_14Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_15Slot, TimeDelta))
		++Count;
	if (MinusAlphaW(&m_16Slot, TimeDelta))
		++Count;
	if (18 == Count)
		return true;
	else
		return false;
}
void CUI_Panhua::Start_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_MenuList[i].OriPos.x, m_MenuList[i].OriPos.y, m_MenuList[i].fZ, 1.f);
		_vector AimPos = XMVectorSet(m_MenuList[i].AimPos.x, m_MenuList[i].AimPos.y, m_MenuList[i].fZ, 1.f);
		_vector vDir = AimPos - OriPos;
		_vector CurrentPos = XMVectorSet(m_MenuList[i].fX, m_MenuList[i].fY, m_MenuList[i].fZ, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta * 30.f;
		
		_float Dist = XMVectorGetX(CurrentPos) - m_MenuList[i].AimPos.x;
		if (0.05f < Dist)
		{
			m_MenuList[i].fX = XMVectorGetX(CurrentPos);
		}
		else
		{
			m_MenuList[i].fX = m_MenuList[i].AimPos.x;
		}
		XMStoreFloat4x4(&(m_MenuList[i].WorldMatrix), XMMatrixScaling(m_MenuList[i].fWidth, m_MenuList[i].fHeight, 1.f)
			* XMMatrixTranslation(m_MenuList[i].fX, m_MenuList[i].fY, m_MenuList[i].fZ));
	}
}
void CUI_Panhua::End_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_MenuList[i].OriPos.x, m_MenuList[i].OriPos.y, m_MenuList[i].fZ, 1.f);
		_vector AimPos = XMVectorSet(m_MenuList[i].AimPos.x, m_MenuList[i].AimPos.y, m_MenuList[i].fZ, 1.f);
		_vector vDir = OriPos  - AimPos;
		_vector CurrentPos = XMVectorSet(m_MenuList[i].fX, m_MenuList[i].fY, m_MenuList[i].fZ, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta * 30.f;

		_float Dist = m_MenuList[i].OriPos.x - XMVectorGetX(CurrentPos);
		if (0.05f < Dist)
		{
			m_MenuList[i].fX = XMVectorGetX(CurrentPos);
		}
		else
		{
			m_MenuList[i].fX = m_MenuList[i].OriPos.x;
		}
		XMStoreFloat4x4(&(m_MenuList[i].WorldMatrix), XMMatrixScaling(m_MenuList[i].fWidth, m_MenuList[i].fHeight, 1.f)
			* XMMatrixTranslation(m_MenuList[i].fX, m_MenuList[i].fY, m_MenuList[i].fZ));
	}
}

void CUI_Panhua::Total()
{
	iTotal = BuyNum * Cost;
	CGameInstance * pGame = CGameInstance::GetInstance();
	string strCoint = to_string(iTotal);
	_uint iDigit = (_uint)strCoint.size();
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_DetailsList[23].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_DetailsList[24].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 2: {m_DetailsList[25].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 3: {m_DetailsList[34].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 4: {m_DetailsList[35].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 5: {m_DetailsList[36].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 6: {m_DetailsList[37].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}


	}

	if (10000000 > iTotal) // 백만
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[34].bRender = true;
		m_DetailsList[35].bRender = true;
		m_DetailsList[36].bRender = true;
		m_DetailsList[37].bRender = true;

	}
	if (1000000 > iTotal) // 십만
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[34].bRender = true;
		m_DetailsList[35].bRender = true;
		m_DetailsList[36].bRender = true;
		m_DetailsList[37].bRender = false;

	}
	if (100000 > iTotal) //만
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[34].bRender = true;
		m_DetailsList[35].bRender = true;
		m_DetailsList[36].bRender = false;
		m_DetailsList[37].bRender = false;

	}
	if (10000 > iTotal) // 천
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[34].bRender = true;
		m_DetailsList[35].bRender = false;
		m_DetailsList[36].bRender = false;
		m_DetailsList[37].bRender = false;
	}
	if (1000 > iTotal) //백
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = true;
		m_DetailsList[34].bRender = false;
		m_DetailsList[35].bRender = false;
		m_DetailsList[36].bRender = false;
		m_DetailsList[37].bRender = false;
	}
	if (100 > iTotal) //십
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = true;
		m_DetailsList[25].bRender = false;
		m_DetailsList[34].bRender = false;
		m_DetailsList[35].bRender = false;
		m_DetailsList[36].bRender = false;
		m_DetailsList[37].bRender = false;

	}
	if (10 > iTotal) //일
	{
		m_DetailsList[23].bRender = true;
		m_DetailsList[24].bRender = false;
		m_DetailsList[25].bRender = false;
		m_DetailsList[34].bRender = false;
		m_DetailsList[35].bRender = false;
		m_DetailsList[36].bRender = false;
		m_DetailsList[37].bRender = false;
	}
}

void CUI_Panhua::SettingLimitTexNum()
{
	m_DetailsList[9].iTexNum = (*pLimibuycount) + 27;
	m_DetailsList[11].iTexNum = Limited + 27;
}
void CUI_Panhua::SettingOverPurchase()
{
	if (nullptr == pSelectSlot)
		return;
	(*pSelectSlot)[0].fColorR = 255.f;
	(*pSelectSlot)[0].fColorG = -255.f;
	(*pSelectSlot)[0].fColorB = -255.f;
	//for (auto& Slot : (*pSelectSlot))
	//{
	//	Slot.fColorR = 255.f;
	//	Slot.fColorG = -255.f;
	//	Slot.fColorB = -255.f;
	//}
	m_bOverPurchase = false;
}

void CUI_Panhua::SettingCurMoneyTexNum()
{
	CGameInstance * pGame = CGameInstance::GetInstance();
	CurrentMoney = m_pInven->GetCoin();
	string strCoint = to_string(CurrentMoney);
	_uint iDigit = (_uint)strCoint.size();
	//for (_uint i = 0; i < iDigit; ++i)
	//{
	//	pGame->SetupSRV(strCoint[i] - '0', m_pShader, "g_DiffuseTextures");
	//}
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_CommonList[8].iTexNum = (strCoint[i] - '0' )+ 27; }
			break;
		case 1: {m_CommonList[9].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		case 2: {m_CommonList[10].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		case 3: {m_CommonList[12].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		case 4: {m_CommonList[13].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		case 5: {m_CommonList[14].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		case 6: {m_CommonList[15].iTexNum = (strCoint[i] - '0') + 27; }
			break;
		}
		

	}

	if (10000000 > CurrentMoney) // 백만
	{
		m_CommonList[8].bRender =  true;
		m_CommonList[9].bRender =  true;
		m_CommonList[10].bRender = true;
		m_CommonList[12].bRender = true;
		m_CommonList[13].bRender = true;
		m_CommonList[14].bRender = true;
		m_CommonList[15].bRender = true;

	}
	if (1000000 > CurrentMoney) // 십만
	{
		m_CommonList[8].bRender =  true;
		m_CommonList[9].bRender =  true;
		m_CommonList[10].bRender = true;
		m_CommonList[12].bRender = true;
		m_CommonList[13].bRender = true;
		m_CommonList[14].bRender = true;
		m_CommonList[15].bRender = false;

	}
	if (100000 > CurrentMoney) //만
	{
		m_CommonList[8].bRender =  true;
		m_CommonList[9].bRender =  true;
		m_CommonList[10].bRender = true;
		m_CommonList[12].bRender = true;
		m_CommonList[13].bRender = true;
		m_CommonList[14].bRender = false;
		m_CommonList[15].bRender = false;

	}
	if (10000 > CurrentMoney) // 천
	{
		m_CommonList[8].bRender =  true;
		m_CommonList[9].bRender =  true;
		m_CommonList[10].bRender = true;
		m_CommonList[12].bRender = true;
		m_CommonList[13].bRender = false;
		m_CommonList[14].bRender = false;
		m_CommonList[15].bRender = false;
	}
	if (1000 > CurrentMoney) //백
		{
			m_CommonList[8].bRender  = true;
			m_CommonList[9].bRender  = true;
			m_CommonList[10].bRender = true;
			m_CommonList[12].bRender = false;
			m_CommonList[13].bRender = false;
			m_CommonList[14].bRender = false;
			m_CommonList[15].bRender = false;
		}
	if (100 > CurrentMoney) //십
	{
		m_CommonList[8].bRender = true;
		m_CommonList[9].bRender = true;
		m_CommonList[10].bRender = false;
		m_CommonList[12].bRender = false;
		m_CommonList[13].bRender = false;
		m_CommonList[14].bRender = false;
		m_CommonList[15].bRender = false;

	}
	if(10 > CurrentMoney) //일
	{
		m_CommonList[8].bRender = true;
		m_CommonList[9].bRender = false;
		m_CommonList[10].bRender = false;
		m_CommonList[12].bRender = false;
		m_CommonList[13].bRender = false;
		m_CommonList[14].bRender = false;
		m_CommonList[15].bRender = false;
	}



	if (CurrentMoney < iTotal)
	{
		m_DetailsList[27].iTexNum = 22;
	}
}

void CUI_Panhua::SettingCurOwnTexNum()
{
	CGameInstance * pGame = CGameInstance::GetInstance();

	if (pSelectSlot == (&m_0Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::RICE); }
	if (pSelectSlot == (&m_1Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::TOFU); }
	if (pSelectSlot == (&m_2Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::FLOUR); }
	if (pSelectSlot == (&m_3Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::MUSHROOM); }
	if (pSelectSlot == (&m_4Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::CHICKEN); }
	if (pSelectSlot == (&m_5Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::EGG); }
	if (pSelectSlot == (&m_6Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::MEAT); }
	if (pSelectSlot == (&m_7Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::HERB); }
	if (pSelectSlot == (&m_8Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::VIOLET); }
	if (pSelectSlot == (&m_9Slot))  { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DANDELION); }
	if (pSelectSlot == (&m_10Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::PEPPER); }
	if (pSelectSlot == (&m_11Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::SUGAR); }
	if (pSelectSlot == (&m_12Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::SALT); }
	if (pSelectSlot == (&m_13Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::OIL); }
	if (pSelectSlot == (&m_14Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::SOYSAUCE); }
	if (pSelectSlot == (&m_15Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::RECIPE0); }
	if (pSelectSlot == (&m_16Slot)) { CurrentOwn = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::RECIPE1); }



	string strCoint = to_string(CurrentOwn);
	_uint iDigit = (_uint)strCoint.size();
	
	for (_uint i = 0; i < iDigit; ++i)
	{
		_int Num = i;
		switch (Num)
		{
		case 0: {m_DetailsList[6].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 1: {m_DetailsList[7].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		case 2: {m_DetailsList[8].iTexNum = (strCoint[i] - '0') + 27; }
				break;
		}


	}

	if (1000 > CurrentOwn) //백
	{
		m_DetailsList[6].bRender = true;
		m_DetailsList[7].bRender = true;
		m_DetailsList[8].bRender = true;
	}
	if (100 > CurrentOwn) //십
	{
		m_DetailsList[6].bRender = true;
		m_DetailsList[7].bRender = true;
		m_DetailsList[8].bRender = false;

	}
	if (10 > CurrentOwn) //일
	{
		m_DetailsList[6].bRender = true;
		m_DetailsList[7].bRender = false;
		m_DetailsList[8].bRender = false;
	}

}

void CUI_Panhua::SettingBuyTexNum()
{
	m_DetailsList[17].bRender = false;
	m_DetailsList[18].bRender = false;
	m_DetailsList[19].iTexNum = BuyNum + 27;
	
	if (Limited - (*pLimibuycount) < BuyNum)
	{
		m_DetailsList[27].iTexNum = 22;
	}
}
void CUI_Panhua::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI_POST, this);
}

HRESULT CUI_Panhua::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;
	switch (Situation)
	{
	case Client::CUI_Panhua::MEET:
	{
		for (_uint i = 0; i < (_int)m_PanList.size(); ++i)
		{
			if (m_PanList[i].bRender)
			{
				if (FAILED(Setup_PanShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Panhua::MENU:
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
	case Client::CUI_Panhua::INMENU:
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
		for (_uint i = 0; i < (_int)m_8Slot.size(); ++i)
		{
			if (m_8Slot[i].bRender)
			{
				if (FAILED(Setup_8SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_9Slot.size(); ++i)
		{
			if (m_9Slot[i].bRender)
			{
				if (FAILED(Setup_9SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_10Slot.size(); ++i)
		{
			if (m_10Slot[i].bRender)
			{
				if (FAILED(Setup_10SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_11Slot.size(); ++i)
		{
			if (m_11Slot[i].bRender)
			{
				if (FAILED(Setup_11SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_12Slot.size(); ++i)
		{
			if (m_12Slot[i].bRender)
			{
				if (FAILED(Setup_12SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_13Slot.size(); ++i)
		{
			if (m_13Slot[i].bRender)
			{
				if (FAILED(Setup_13SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_14Slot.size(); ++i)
		{
			if (m_14Slot[i].bRender)
			{
				if (FAILED(Setup_14SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_15Slot.size(); ++i)
		{
			if (m_15Slot[i].bRender)
			{
				if (FAILED(Setup_15SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		for (_uint i = 0; i < (_int)m_16Slot.size(); ++i)
		{
			if (m_16Slot[i].bRender)
			{
				if (FAILED(Setup_16SlotShader(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
	}
	break;
	case Client::CUI_Panhua::DETAILS:
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
	case Client::CUI_Panhua::CONFRIM:
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
	case Client::CUI_Panhua::BYE:
	{
	}
	break;
	}
	}


	return S_OK;
}

void CUI_Panhua::RenderGUI()
{
}

_bool CUI_Panhua::SelectUI(PANDESC* pDesc)
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

HRESULT CUI_Panhua::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIPANHUA,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_Panhua::Setup_PanShader(_uint index)
{
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_PanList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_PanList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_PanList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_PanList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_PanList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_PanList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_PanList[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_MenuShader(_uint index)
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

void CUI_Panhua::Set_SituMeet()
{
	SetState(ACTIVE);
	Situation = PANSITUINDEX::MEET;
}

void CUI_Panhua::Set_END()
{
	Situation = PANSITUINDEX::PANEND;
	m_Count = 0;
	m_InMenuRenderStart = true;
	m_MenuRenderStart = true;
	m_DetailRenderStart = true;
	m_ConfirmRenderStart = true;
	m_bOverPurchase = false;
	pSelectSlot = nullptr;
	pLimibuycount = nullptr;
	m_bMouseMoveStart = false;
	mouse = 0l;
	BuyNum = 0;
	CurrentMoney = 0;
	CurrentOwn = 0;
	iTotal = 0;
	m_NPCbye = false;
	for (auto& Desc : m_PanList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_MenuList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_CommonList)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_DetailsList)
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
	for (auto& Desc : m_14Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_15Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}
	for (auto& Desc : m_16Slot)
	{
		Desc.fColorA = Desc.Color.w;
	}


	SetState(DISABLE);
}

void CUI_Panhua::IsMouseinRect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (SelectUI(&m_0Slot[0]))
	{
		m_0Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[0];
			Limited = 5;
			Cost = 1000;
			m_DetailsList[31].iTexNum = 81; //설명0
			m_DetailsList[32].iTexNum = 82; //설명1
			m_DetailsList[33].iTexNum = 131; //설명타입
			m_DetailsList[30].iTexNum = itemDesc[0].iImageIndex; //아이템 이미지
			pSelectSlot = &m_0Slot;
		}
	}
	else
		m_0Slot[8].bRender = false;

	if (SelectUI(&m_1Slot[0]))
	{
		m_1Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[1];
			Limited = 5;
			Cost = 1000;
			m_DetailsList[31].iTexNum = 84;
			m_DetailsList[32].iTexNum = 85;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[1].iImageIndex;
			pSelectSlot = &m_1Slot;
		}
	}
	else
		m_1Slot[8].bRender = false;

	if (SelectUI(&m_2Slot[0]))
	{
		m_2Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[2];
			Limited = 5;
			Cost = 1000;
			m_DetailsList[31].iTexNum = 87;
			m_DetailsList[32].iTexNum = 88;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[2].iImageIndex;
			pSelectSlot = &m_2Slot;
		}
	}
	else
		m_2Slot[8].bRender = false;

	if (SelectUI(&m_3Slot[0]))
	{
		m_3Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[3];
			Limited = 5;
			Cost = 1000;
			m_DetailsList[31].iTexNum = 90;
			m_DetailsList[32].iTexNum = 91;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[3].iImageIndex;
			pSelectSlot = &m_3Slot;
		}
	}
	else
		m_3Slot[8].bRender = false;

	if (SelectUI(&m_4Slot[0]))
	{
		m_4Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[4];
			Limited = 5;
			Cost = 1000;
			m_DetailsList[31].iTexNum = 93;
			m_DetailsList[32].iTexNum = 94;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[4].iImageIndex;
			pSelectSlot = &m_4Slot;
		}
	}
	else
		m_4Slot[8].bRender = false;

	if (SelectUI(&m_5Slot[0]))
	{
		m_5Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[5];
			Limited = 3;
			Cost = 2000;
			m_DetailsList[31].iTexNum = 96;
			m_DetailsList[32].iTexNum = 97;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[5].iImageIndex;
			pSelectSlot = &m_5Slot;
		}
	}
	else
		m_5Slot[8].bRender = false;

	if (SelectUI(&m_6Slot[0]))
	{
		m_6Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[6];
			Limited = 3;
			Cost = 2000;
			m_DetailsList[31].iTexNum = 99;
			m_DetailsList[32].iTexNum = 100;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[6].iImageIndex;
			pSelectSlot = &m_6Slot;
		}
	}
	else
		m_6Slot[8].bRender = false;

	if (SelectUI(&m_7Slot[0]))
	{
		m_7Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[7];
			Limited = 3;
			Cost = 2000;
			m_DetailsList[31].iTexNum = 102;
			m_DetailsList[32].iTexNum = 103;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[7].iImageIndex;
			pSelectSlot = &m_7Slot;
		}
	}
	else
		m_7Slot[8].bRender = false;

	if (SelectUI(&m_8Slot[0]))
	{
		m_8Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[8];
			Limited = 3;
			Cost = 2000;
			m_DetailsList[31].iTexNum = 105;
			m_DetailsList[32].iTexNum = 106;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[8].iImageIndex;
			pSelectSlot = &m_8Slot;
		}
	}
	else
		m_8Slot[8].bRender = false;

	if (SelectUI(&m_9Slot[0]))
	{
		m_9Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[9];
			Limited = 2;
			Cost = 2000;
			m_DetailsList[31].iTexNum = 108;
			m_DetailsList[32].iTexNum = 109;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[9].iImageIndex;
			pSelectSlot = &m_9Slot;
		}
	}
	else
		m_9Slot[8].bRender = false;

	if (SelectUI(&m_10Slot[0]))
	{
		m_10Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[10];
			Limited = 2;
			Cost = 3000;
			m_DetailsList[31].iTexNum = 111;
			m_DetailsList[32].iTexNum = 112;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[10].iImageIndex;
			pSelectSlot = &m_10Slot;
		}
	}
	else
		m_10Slot[8].bRender = false;

	if (SelectUI(&m_11Slot[0]))
	{
		m_11Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[11];
			Limited = 2;
			Cost = 3000;
			m_DetailsList[31].iTexNum = 114;
			m_DetailsList[32].iTexNum = 115;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[11].iImageIndex;
			pSelectSlot = &m_11Slot;
		}
	}
	else
		m_11Slot[8].bRender = false;

	if (SelectUI(&m_12Slot[0]))
	{
		m_12Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[12];
			Limited = 2;
			Cost = 3000;
			m_DetailsList[31].iTexNum = 117;
			m_DetailsList[32].iTexNum = 118;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[12].iImageIndex;
			pSelectSlot = &m_12Slot;
		}
	}
	else
		m_12Slot[8].bRender = false;

	if (SelectUI(&m_13Slot[0]))
	{
		m_13Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[13];
			Limited = 2;
			Cost = 3000;
			m_DetailsList[31].iTexNum = 120;
			m_DetailsList[32].iTexNum = 121;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[13].iImageIndex;
			pSelectSlot = &m_13Slot;
		}
	}
	else
		m_13Slot[8].bRender = false;

	if (SelectUI(&m_14Slot[0]))
	{
		m_14Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[14];
			Limited = 2;
			Cost = 3000;
			m_DetailsList[31].iTexNum = 123;
			m_DetailsList[32].iTexNum = 124;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[14].iImageIndex;
			pSelectSlot = &m_14Slot;
		}
	}
	else
		m_14Slot[8].bRender = false;

	if (SelectUI(&m_15Slot[0]))
	{
		m_15Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[15];
			Limited = 1;
			Cost = 10000;
			m_DetailsList[31].iTexNum = 126;
			m_DetailsList[32].iTexNum = 127;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[15].iImageIndex;
			pSelectSlot = &m_15Slot;
		}
	}
	else
		m_15Slot[8].bRender = false;

	if (SelectUI(&m_16Slot[0]))
	{
		m_16Slot[8].bRender = true; //올렸을때
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			Situation = CUI_Panhua::DETAILS;
			pLimibuycount = &Limibuycount[16];
			Limited = 1;
			Cost = 10000;
			m_DetailsList[31].iTexNum = 129;
			m_DetailsList[32].iTexNum = 130;
			m_DetailsList[33].iTexNum = 131;
			m_DetailsList[30].iTexNum = itemDesc[16].iImageIndex;
			pSelectSlot = &m_16Slot;
		}
	}
	else
		m_16Slot[8].bRender = false;

}
_bool CUI_Panhua::AddAlpha(PANDESC* pDesc, _double TimeDelta)
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

_bool CUI_Panhua::MinusAlpha(PANDESC* pDesc, _double TimeDelta)
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

_bool CUI_Panhua::AddAlphaW(vector<PANDESC>* pDesc, _double TimeDelta)
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

_bool CUI_Panhua::MinusAlphaW(vector<PANDESC>* pDesc, _double TimeDelta)
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

void CUI_Panhua::ColorP(PANDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_Panhua::ColorM(PANDESC* pDesc, _float4 fcolor, _double TimeDelta)
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

void CUI_Panhua::MouseMove()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	mouse = pGameInstance->InputMouseMove(DIMM_Y);
	(134.f > m_CommonList[11].fY ? m_CommonList[11].fY += -mouse*0.5f : m_CommonList[11].fY = 134.f);
	(-156.f <= m_CommonList[11].fY ? m_CommonList[11].fY += -mouse*0.5f : m_CommonList[11].fY = -156.f);


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
	for (auto& Slot : m_8Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_9Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_10Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_11Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_12Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_13Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_14Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_15Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
	for (auto& Slot : m_16Slot)
	{
		Slot.fY += mouse;
		XMStoreFloat4x4(&(Slot.WorldMatrix), XMMatrixScaling(Slot.fWidth, Slot.fHeight, 1.f)
			* XMMatrixTranslation(Slot.fX, Slot.fY, Slot.fZ));
	}
}

void CUI_Panhua::Load()
{
	//
	_uint index = 3;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Panhua%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_PanList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 6;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/PanMenu%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		Desc.OriPos = _float2(Desc.fX, Desc.fY);
		Desc.AimPos = _float2(Desc.fX - 10.f, Desc.fY);
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_MenuList.push_back(Desc);
		CloseHandle(hFile);

	}
	//
	index = 16;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/PanCommon%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/PanFinal%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
	index = 38;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/PanDetails%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan0Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan1Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan2Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan3Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan4Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan5Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan6Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		wsprintf(szFileName, L"../../Data/UI/Pan7Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan8Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_8Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan9Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_9Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan10Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_10Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan11Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_11Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan12Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_12Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan13Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_13Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan14Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_14Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan15Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_15Slot.push_back(Desc);
		CloseHandle(hFile);
	}

	//
	index = 9;
	for (_uint i = 0; i < index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan16Slot%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		PANDESC Desc;
		ZeroMemory(&Desc, sizeof(PANDESC));

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
		m_16Slot.push_back(Desc);
		CloseHandle(hFile);
	}

}








void CUI_Panhua::Save()
{
	//
	_uint index = 0;
	for (auto& Desc : m_PanList)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Panhua%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/PanMenu%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/PanCommon%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/PanFinal%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/PanDetails%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan0Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan1Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan2Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan3Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan4Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan5Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan6Slot%d.dat", index);
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
		wsprintf(szFileName, L"../../Data/UI/Pan7Slot%d.dat", index);
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
	for (auto& Desc : m_8Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan8Slot%d.dat", index);
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
	for (auto& Desc : m_9Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan9Slot%d.dat", index);
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
	for (auto& Desc : m_10Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan10Slot%d.dat", index);
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
	for (auto& Desc : m_11Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan11Slot%d.dat", index);
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
	for (auto& Desc : m_12Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan12Slot%d.dat", index);
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
	for (auto& Desc : m_13Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan13Slot%d.dat", index);
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
	for (auto& Desc : m_14Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan14Slot%d.dat", index);
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
	for (auto& Desc : m_15Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan15Slot%d.dat", index);
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
	for (auto& Desc : m_16Slot)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/Pan16Slot%d.dat", index);
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

HRESULT CUI_Panhua::Setup_CommonShader(_uint index)
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

HRESULT CUI_Panhua::Setup_DetailsShader(_uint index)
{
	if(30 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_DetailsList[30].iTexNum, m_pShader, "g_MyTexture")))
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
HRESULT CUI_Panhua::Setup_0SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_0Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_1SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_1Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_2SlotShader(_uint index)
{
	if (2 == index) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_2Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_3SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_3Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_4SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_4Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_5SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_5Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_6SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_6Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_7SlotShader(_uint index)
{
	if (2 == index) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_7Slot[index].iTexNum, m_pShader, "g_MyTexture")))
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

HRESULT CUI_Panhua::Setup_8SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_8Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_8Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_8Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_8Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_8Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_8Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_8Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_8Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_9SlotShader(_uint index)
{
	if (2 == index) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_9Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_9Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_9Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_9Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_9Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_9Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_9Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_9Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_10SlotShader(_uint index)
{
	if (2 == index) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_10Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_10Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_10Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_10Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_10Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_10Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_10Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_10Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_11SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_11Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_11Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_11Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_11Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_11Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_11Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_11Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_11Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_12SlotShader(_uint index)
{
	if (2 == index) 
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_12Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_12Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_12Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_12Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_12Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_12Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_12Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_12Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_13SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_13Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_13Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_13Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_13Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_13Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_13Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_13Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_12Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_14SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_14Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_14Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_14Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_14Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_14Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_14Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_14Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_14Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_15SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_15Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_15Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_15Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_15Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_15Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_15Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_15Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_15Slot[index].iPass;

	return S_OK;
}

HRESULT CUI_Panhua::Setup_16SlotShader(_uint index)
{
	if (2 == index)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		if (FAILED(pGameInstance->SetupSRV(m_16Slot[index].iTexNum, m_pShader, "g_MyTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_16Slot[index].iTexNum)))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_16Slot[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_16Slot[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_16Slot[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_16Slot[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_16Slot[index].fColorA), sizeof(_float))))
		return E_FAIL;
	
	m_iPass = m_16Slot[index].iPass;

	return S_OK;
}


HRESULT CUI_Panhua::Setup_FinalShader(_uint index)
{
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


CUI_Panhua* CUI_Panhua::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Panhua* pInstance = new CUI_Panhua(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Panhua";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Panhua::Clone(void* pArg)
{
	CUI_Panhua* pInstance = new CUI_Panhua(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Panhua";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Panhua::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}