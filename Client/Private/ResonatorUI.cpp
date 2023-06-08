#include "stdafx.h"
#include "..\Public\ResonatorUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "CameraMovement.h"
#include "PlayerState.h"
#include "Inventory.h"
#include "UICharacter.h"

#include "TerminalUI.h"
#include "ItemDB.h"

#include "EchoSystem.h"

CResonatorUI::CResonatorUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CResonatorUI::CResonatorUI(const CResonatorUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CResonatorUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CResonatorUI::Initialize(void * pArg)
{
	CAppManager* pApp = CAppManager::GetInstance();

	assert(pArg);
	m_pTerminal = static_cast<CTerminalUI*>(pArg);

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	ZeroMemory(m_fElemAlpha, sizeof(m_fElemAlpha));
	ZeroMemory(m_bElemAlphaStart, sizeof(m_bElemAlphaStart));

	ZeroMemory(m_OrthoButtons, sizeof(ORTHO_DESC) * BTN_END);
	ZeroMemory(m_fButtonAlpha, sizeof(_float) * BTN_END);

	// Ortho Settings
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

#pragma region FRAME_ORTHO
	// Background
	m_OrthoBackground.fWidth = g_iWinSizeX;
	m_OrthoBackground.fHeight = g_iWinSizeY;
	m_OrthoBackground.fX = g_iWinSizeX >> 1;
	m_OrthoBackground.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoBackground);

	// SlotText
	m_OrthoTitleText.fWidth = 512.f * 0.65f;
	m_OrthoTitleText.fHeight = 64.f * 0.65f;
	m_OrthoTitleText.fX = 300.f;
	m_OrthoTitleText.fY = 120.f;
	CAppManager::ComputeOrtho(&m_OrthoTitleText);

#pragma endregion

#pragma region STATE_DETAILS
	// Character Icon
	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoCharBack[i].fWidth = 90.f;
		m_OrthoCharBack[i].fHeight = 90.f;
		m_OrthoCharBack[i].fX = g_iWinSizeX - 90.f;
		m_OrthoCharBack[i].fY = 100.f + (i * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoCharBack[i]);

		m_OrthoCharIcon[i].fWidth = 70.f;
		m_OrthoCharIcon[i].fHeight = 70.f;
		m_OrthoCharIcon[i].fX = g_iWinSizeX - 90.f;
		m_OrthoCharIcon[i].fY = 100.f + (i * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoCharIcon[i]);
	}

	// PlayerText
	m_OrthoPlayerText.fWidth = 512.f * 0.6f;
	m_OrthoPlayerText.fHeight = 64.f * 0.6f;
	m_OrthoPlayerText.fX = 320.f;
	m_OrthoPlayerText.fY = 185.f;
	CAppManager::ComputeOrtho(&m_OrthoPlayerText);

	// LevelText
	m_OrthoLevelText.fWidth = 512.f * 0.6f;
	m_OrthoLevelText.fHeight = 64.f * 0.6f;
	m_OrthoLevelText.fX = 320.f;
	m_OrthoLevelText.fY = 175.f + 64.f * 0.6f;
	CAppManager::ComputeOrtho(&m_OrthoLevelText);

	// CurLevel
	for (_uint i = 0; i < 5; ++i)
	{
		m_OrthoLevel[i].fWidth = 16.f * 0.8f;
		m_OrthoLevel[i].fHeight = 25.f * 0.8f;
		m_OrthoLevel[i].fX = 230.f + (i * (18.f * 0.7f));
		m_OrthoLevel[i].fY = 173.f + 64.f * 0.6f;
		CAppManager::ComputeOrtho(&m_OrthoLevel[i]);
	}

	// Exp
	m_OrthoExp.fWidth = 220.f;
	m_OrthoExp.fHeight = 8.f;
	m_OrthoExp.fX = 270.f;
	m_OrthoExp.fY = 200.f + 64.f * 0.6f;
	CAppManager::ComputeOrtho(&m_OrthoExp);

	// Buttons
	for (_uint i = 0; i < BTN_END; ++i)
	{
		//Slot
		m_OrthoButtons[i].fWidth = 90.f;
		m_OrthoButtons[i].fHeight = 90.f;
		m_OrthoButtons[i].fX = 90.f;
		m_OrthoButtons[i].fY = 200.f + (i * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoButtons[i]);

		// Icon
		m_OrthoButtonIcons[i].fWidth = 65.f;
		m_OrthoButtonIcons[i].fHeight = 65.f;
		m_OrthoButtonIcons[i].fX = 90.f;
		m_OrthoButtonIcons[i].fY = 202.f + (i * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoButtonIcons[i]);
	}

	// HP, Attack, Defense, Crit
	const _float fRatio = 0.4f;
	const _float fOffset = 5.f;

	m_OrthoState[0].fWidth = 400.f * fRatio;
	m_OrthoState[0].fHeight = 150.f * fRatio;
	m_OrthoState[0].fX = 240.f;
	m_OrthoState[0].fY = 300.f;
	CAppManager::ComputeOrtho(&m_OrthoState[0]);

	m_OrthoState[1].fWidth = 400.f * fRatio;
	m_OrthoState[1].fHeight = 150.f * fRatio;
	m_OrthoState[1].fX = m_OrthoState[0].fX + (m_OrthoState[0].fWidth) + fOffset;
	m_OrthoState[1].fY = 300.f;
	CAppManager::ComputeOrtho(&m_OrthoState[1]);

	m_OrthoState[2].fWidth = 400.f * fRatio;
	m_OrthoState[2].fHeight = 150.f * fRatio;
	m_OrthoState[2].fX = m_OrthoState[0].fX;
	m_OrthoState[2].fY = 300.f + (m_OrthoState[0].fHeight + fOffset);
	CAppManager::ComputeOrtho(&m_OrthoState[2]);

	m_OrthoState[3].fWidth = 400.f * fRatio;
	m_OrthoState[3].fHeight = 150.f * fRatio;
	m_OrthoState[3].fX = m_OrthoState[0].fX + (m_OrthoState[0].fWidth) + fOffset;
	m_OrthoState[3].fY = 300.f + (m_OrthoState[0].fHeight + fOffset);
	CAppManager::ComputeOrtho(&m_OrthoState[3]);

	// StateData ��� �ִ� 6�ڸ�
	for (_uint i = 0; i < STATE_END; ++i)
	{
		for (_uint digit = 0; digit < 6; ++digit)
		{
			m_OrthoStateData[i][digit].fWidth = 12.f;
			m_OrthoStateData[i][digit].fHeight = 19.f;
			m_OrthoStateData[i][digit].fX = m_OrthoState[i].fX - 30.f + (digit * 12.f);
			m_OrthoStateData[i][digit].fY = m_OrthoState[i].fY + 7.f;

			CAppManager::ComputeOrtho(&m_OrthoStateData[i][digit]);
		}
	}

	// UpgradeSlot
	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoUpgradeSlot[i].fWidth = 80.f;
		m_OrthoUpgradeSlot[i].fHeight = 80.f;
		m_OrthoUpgradeSlot[i].fX = 200.f + (i * 85.f);
		m_OrthoUpgradeSlot[i].fY = 450.f;
		CAppManager::ComputeOrtho(&m_OrthoUpgradeSlot[i]);

		m_OrthoUpgradeCapsule[i] = m_OrthoUpgradeSlot[i];
		m_OrthoUpgradeCapsule[i].fWidth = 60;
		m_OrthoUpgradeCapsule[i].fHeight = 60;
		CAppManager::ComputeOrtho(&m_OrthoUpgradeCapsule[i]);
	}

	for (_uint iGrade = 0; iGrade < 4; ++iGrade)
	{
		for (_uint iDigit = 0; iDigit < 4; ++ iDigit)
		{
			m_OrthoCapsuleAmount[iGrade][iDigit].fWidth = 8.f;
			m_OrthoCapsuleAmount[iGrade][iDigit].fHeight = 12.5f;
			m_OrthoCapsuleAmount[iGrade][iDigit].fX = (m_OrthoUpgradeSlot[iGrade].fX - 25.f) + (iDigit * 6.25f);
			m_OrthoCapsuleAmount[iGrade][iDigit].fY = 475.f;
			CAppManager::ComputeOrtho(&m_OrthoCapsuleAmount[iGrade][iDigit]);
		}

	}

	// Upgrade Text
	m_OrthoUpgradeText.fWidth = 256.f;
	m_OrthoUpgradeText.fHeight = 32.f;
	m_OrthoUpgradeText.fX = g_iWinSizeX >> 2;
	m_OrthoUpgradeText.fY = g_iWinSizeY - 125.f;
	CAppManager::ComputeOrtho(&m_OrthoUpgradeText);

	// Upgrade Button
	m_OrthoUpgradeBtn.fWidth = 80.f;
	m_OrthoUpgradeBtn.fHeight = 80.f;
	m_OrthoUpgradeBtn.fX = g_iWinSizeX >> 2;
	m_OrthoUpgradeBtn.fY = g_iWinSizeY - 85.f;
	CAppManager::ComputeOrtho(&m_OrthoUpgradeBtn);
		
	// Consume Coin Back
	m_OrthoCoinBack.fWidth = 380.f;
	m_OrthoCoinBack.fHeight = 75.f;
	m_OrthoCoinBack.fX = 325.f;
	m_OrthoCoinBack.fY = g_iWinSizeY - 180.f;
	CAppManager::ComputeOrtho(&m_OrthoCoinBack);

	// ConsumeCoinText
	m_OrthoConsumeText.fWidth = 256.f * 0.9f;
	m_OrthoConsumeText.fHeight = 32.f * 0.9f;
	m_OrthoConsumeText.fX = 285.f;
	m_OrthoConsumeText.fY = g_iWinSizeY - 205.f;
	CAppManager::ComputeOrtho(&m_OrthoConsumeText);

	// OwnCoinText
	m_OrthoOwnText.fWidth = 256.f * 0.9f;
	m_OrthoOwnText.fHeight = 32.f * 0.9f;
	m_OrthoOwnText.fX = 445.f;
	m_OrthoOwnText.fY = g_iWinSizeY - 205.f;
	CAppManager::ComputeOrtho(&m_OrthoOwnText);

	// Consume Coin Icon
	m_OrthoCoinIcon.fWidth = 35.f;
	m_OrthoCoinIcon.fHeight = 35.f;
	m_OrthoCoinIcon.fX = 180.f;
	m_OrthoCoinIcon.fY = g_iWinSizeY - 170.f;
	CAppManager::ComputeOrtho(&m_OrthoCoinIcon);

	// Consume Coin & Own Coin
	for (_uint i = 0; i < DRAW_MAX; ++i)
	{
		m_OrthoConsumeCoin[i].fWidth = 16.f * 0.7f;
		m_OrthoConsumeCoin[i].fHeight = 25.f  * 0.7f;
		m_OrthoConsumeCoin[i].fX = 200.f + (i * m_OrthoConsumeCoin[i].fWidth);
		m_OrthoConsumeCoin[i].fY = g_iWinSizeY - 170.f;
		CAppManager::ComputeOrtho(&m_OrthoConsumeCoin[i]);

		m_OrthoOwnCoin[i].fWidth = 16.f * 0.7f;
		m_OrthoOwnCoin[i].fHeight = 25.f  * 0.7f;
		m_OrthoOwnCoin[i].fX = 335.f + (i * m_OrthoConsumeCoin[i].fWidth);
		m_OrthoOwnCoin[i].fY = g_iWinSizeY - 170.f;
		CAppManager::ComputeOrtho(&m_OrthoOwnCoin[i]);
	}

#pragma endregion

#pragma region WEAPON_ORGHO
	// WeaponName
	m_OrthoWeaponName.fWidth = 512.f * 0.6f;
	m_OrthoWeaponName.fHeight = 64.f * 0.6f;
	m_OrthoWeaponName.fX = 320.f;
	m_OrthoWeaponName.fY = 185.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponName);

	// WeaponTool Desc
	m_OrthoWeaponDesc.fWidth = 512.f * 0.5f;
	m_OrthoWeaponDesc.fHeight = 64.f * 0.5f;
	m_OrthoWeaponDesc.fX = 295.f;
	m_OrthoWeaponDesc.fY = 220.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponDesc);

	// ��ȭ ǥ�� 0/5
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		m_OrthoUpgradeSign[i].fWidth = 40.f;
		m_OrthoUpgradeSign[i].fHeight = 40.f;
		m_OrthoUpgradeSign[i].fX = 180.f + (i * 35.f);
		m_OrthoUpgradeSign[i].fY = 260.f;
		CAppManager::ComputeOrtho(&m_OrthoUpgradeSign[i]);
	}

	// ���ݷ�, ġ��Ÿ ��׶���
	m_OrthoDamageBack.fWidth = 512.f * 0.7f;
	m_OrthoDamageBack.fHeight = 64.f * 0.5f;
	m_OrthoDamageBack.fX = 295.f;
	m_OrthoDamageBack.fY = 300.f;
	CAppManager::ComputeOrtho(&m_OrthoDamageBack);

	m_OrthoCriDamageBack.fWidth = 512.f * 0.7f;
	m_OrthoCriDamageBack.fHeight = 64.f * 0.5f;
	m_OrthoCriDamageBack.fX = 295.f;
	m_OrthoCriDamageBack.fY = 340.f;
	CAppManager::ComputeOrtho(&m_OrthoCriDamageBack);

	// Damage Text
	m_OrthoAttackText[0].fWidth = 512.f * 0.5f;
	m_OrthoAttackText[0].fHeight = 64.f * 0.5f;
	m_OrthoAttackText[0].fX = 295.f;
	m_OrthoAttackText[0].fY = 300.f;
	CAppManager::ComputeOrtho(&m_OrthoAttackText[0]);

	// CriRate Text
	m_OrthoAttackText[1].fWidth = 512.f * 0.5f;
	m_OrthoAttackText[1].fHeight = 64.f * 0.5f;
	m_OrthoAttackText[1].fX = 295.f;
	m_OrthoAttackText[1].fY = 340.f;
	CAppManager::ComputeOrtho(&m_OrthoAttackText[1]);
	
	for (_uint i = 0; i < DAMAGE_MAX; ++i)
	{
		m_OrthoDamage[i].fWidth = 16.f * 0.7f;
		m_OrthoDamage[i].fHeight = 25.f * 0.7f;
		m_OrthoDamage[i].fX = 380.f + (i * (16.f * 0.7f));
		m_OrthoDamage[i].fY = 300.f;
		CAppManager::ComputeOrtho(&m_OrthoDamage[i]);

		m_OrthoCriRate[i].fWidth = 16.f * 0.7f;
		m_OrthoCriRate[i].fHeight = 25.f * 0.7f;
		m_OrthoCriRate[i].fX = 380.f + (i * (16.f * 0.7f));
		m_OrthoCriRate[i].fY = 340.f;
		CAppManager::ComputeOrtho(&m_OrthoCriRate[i]);
	}

	// Weapon Upgrade Text
	m_OrthoWeaponUpgradeText.fWidth = 256.f;
	m_OrthoWeaponUpgradeText.fHeight = 32.f;
	m_OrthoWeaponUpgradeText.fX = (g_iWinSizeX >> 2) - 60.f;
	m_OrthoWeaponUpgradeText.fY = g_iWinSizeY - 125.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradeText);

	// Weapon Upgrade Button
	m_OrthoWeaponUpgradeButton.fWidth = 80.f;
	m_OrthoWeaponUpgradeButton.fHeight = 80.f;
	m_OrthoWeaponUpgradeButton.fX = (g_iWinSizeX >> 2) - 60.f;
	m_OrthoWeaponUpgradeButton.fY = g_iWinSizeY - 85.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradeButton);

	// Weapon Switch Text
	m_OrthoWeaponSwitchText.fWidth = 256.f;
	m_OrthoWeaponSwitchText.fHeight = 32.f;
	m_OrthoWeaponSwitchText.fX = (g_iWinSizeX >> 2) + 60.f;
	m_OrthoWeaponSwitchText.fY = g_iWinSizeY - 125.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponSwitchText);

	// Weapon Switch Button
	m_OrthoWeaponSwitchButton.fWidth = 80.f;
	m_OrthoWeaponSwitchButton.fHeight = 80.f;
	m_OrthoWeaponSwitchButton.fX = (g_iWinSizeX >> 2) + 60.f;
	m_OrthoWeaponSwitchButton.fY = g_iWinSizeY - 85.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponSwitchButton);

	// Weapon UpgradeCost Back
	m_OrthoUpgradeCostBack.fWidth = 512.f * 0.7f;
	m_OrthoUpgradeCostBack.fHeight = 96.f;
	m_OrthoUpgradeCostBack.fX = 295.f;
	m_OrthoUpgradeCostBack.fY = 442.f;
	CAppManager::ComputeOrtho(&m_OrthoUpgradeCostBack);

	// Weapon UpgradeCost Text
	m_OrthoUpgradeCostText.fWidth = 512.f * 0.5f;
	m_OrthoUpgradeCostText.fHeight = 64.f * 0.5f;
	m_OrthoUpgradeCostText.fX = 295.f;
	m_OrthoUpgradeCostText.fY = 410.f;
	CAppManager::ComputeOrtho(&m_OrthoUpgradeCostText);

	// Weapon Own Text
	m_OrthoUpgradeOwnCostText.fWidth = 512.f * 0.5f;
	m_OrthoUpgradeOwnCostText.fHeight = 64.f * 0.5f;
	m_OrthoUpgradeOwnCostText.fX = 475.f;
	m_OrthoUpgradeOwnCostText.fY = 410.f;
	CAppManager::ComputeOrtho(&m_OrthoUpgradeOwnCostText);

	// Gem Back
	m_OrthoGemIconBack.fWidth = 75.f;
	m_OrthoGemIconBack.fHeight = 75.f;
	m_OrthoGemIconBack.fX = 205.f;
	m_OrthoGemIconBack.fY = 455.f;
	CAppManager::ComputeOrtho(&m_OrthoGemIconBack);

	// Gem Icon
	m_OrthoGemIcon.fWidth = 60.f;
	m_OrthoGemIcon.fHeight = 60.f;
	m_OrthoGemIcon.fX = 205.f;
	m_OrthoGemIcon.fY = 455.f;
	CAppManager::ComputeOrtho(&m_OrthoGemIcon);

	for (_uint i = 0; i < GEM_DIGIT_MAX; ++i)
	{
		m_OrthoGemCost[i].fWidth = 16.f * 0.7f;
		m_OrthoGemCost[i].fHeight = 25.f * 0.7f;
		m_OrthoGemCost[i].fX = 245.f + (i * (16.f * 0.7f));
		m_OrthoGemCost[i].fY = 455.f;
		CAppManager::ComputeOrtho(&m_OrthoGemCost[i]);

		m_OrthoOwnGemCost[i].fWidth = 16.f * 0.7f;
		m_OrthoOwnGemCost[i].fHeight = 25.f * 0.7f;
		m_OrthoOwnGemCost[i].fX = 365.f + (i * (16.f * 0.7f));
		m_OrthoOwnGemCost[i].fY = 455.f;
		CAppManager::ComputeOrtho(&m_OrthoOwnGemCost[i]);
	}

	m_OrthoWeaponInvenBack.fWidth = 1080.f;
	m_OrthoWeaponInvenBack.fHeight = 620.f;
	m_OrthoWeaponInvenBack.fX = g_iWinSizeX >> 1;
	m_OrthoWeaponInvenBack.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoWeaponInvenBack);

	m_OrthoSwitchBackpage.fWidth = 70.f;
	m_OrthoSwitchBackpage.fHeight = 70.f;
	m_OrthoSwitchBackpage.fX = (g_iWinSizeX >> 1) + (m_OrthoWeaponInvenBack.fWidth * 0.5f) - m_OrthoSwitchBackpage.fWidth;
	m_OrthoSwitchBackpage.fY = (g_iWinSizeY >> 1) - (m_OrthoWeaponInvenBack.fHeight * 0.5f) + m_OrthoSwitchBackpage.fHeight;
	CAppManager::ComputeOrtho(&m_OrthoSwitchBackpage);
	
	//Select Weapon Text
	m_OrthoSelectWeaponText.fWidth = 512.f * 0.75f;
	m_OrthoSelectWeaponText.fHeight = 64.f * 0.75f;
	m_OrthoSelectWeaponText.fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 256.f;
	m_OrthoSelectWeaponText.fY = (g_iWinSizeY >> 1) - (m_OrthoWeaponInvenBack.fHeight * 0.5f) + m_OrthoSelectWeaponText.fHeight + 70.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponText);

	// Weapon Slot
	for (_uint i = 0; i < 20; ++i)
	{
		m_OrthoWeaponSlot[i].fWidth = 100.f;
		m_OrthoWeaponSlot[i].fHeight = 100.f;
		m_OrthoWeaponSlot[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 100.f + ((i % 5) * 100.f);
		m_OrthoWeaponSlot[i].fY = 250 + ((i/5) * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoWeaponSlot[i]);

		m_OrthoWeaponIcon[i].fWidth = 70.f;
		m_OrthoWeaponIcon[i].fHeight = 70.f;
		m_OrthoWeaponIcon[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 100.f + ((i % 5) * 100.f);
		m_OrthoWeaponIcon[i].fY = 250 + ((i / 5) * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoWeaponIcon[i]);

		// ������ �� ��ȭ Ƚ��
		m_OrthoWeaponUpgradeCount[i].fWidth = 16.f * 0.7f;
		m_OrthoWeaponUpgradeCount[i].fHeight = 25.f * 0.7f;
		m_OrthoWeaponUpgradeCount[i].fX = m_OrthoWeaponSlot[i].fX + 25.f;
		m_OrthoWeaponUpgradeCount[i].fY = m_OrthoWeaponSlot[i].fY + 25.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradeCount[i]);

		// ������ �� ��ȭ Ƚ�� + �̹���
		m_OrthoWeaponUpgradePlus[i].fWidth = 16.f * 0.7f;
		m_OrthoWeaponUpgradePlus[i].fHeight = 25.f * 0.7f;
		m_OrthoWeaponUpgradePlus[i].fX = m_OrthoWeaponSlot[i].fX + 25.f - (16.f * 0.7f);
		m_OrthoWeaponUpgradePlus[i].fY = m_OrthoWeaponSlot[i].fY + 25.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradePlus[i]);
	}

	//Select Weapon Slot
	m_OrthoSelectWeaponSlot.fWidth = 240.f;
	m_OrthoSelectWeaponSlot.fHeight = 240.f;
	m_OrthoSelectWeaponSlot.fX = (g_iWinSizeX >> 1) + (m_OrthoWeaponInvenBack.fWidth * 0.25f);
	m_OrthoSelectWeaponSlot.fY = (g_iWinSizeY >> 1) - 130.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponSlot);

	//Select Weapon Icon
	m_OrthoSelectWeapon.fWidth = 180.f;
	m_OrthoSelectWeapon.fHeight = 180.f;
	m_OrthoSelectWeapon.fX = (g_iWinSizeX >> 1) + (m_OrthoWeaponInvenBack.fWidth * 0.25f);
	m_OrthoSelectWeapon.fY = (g_iWinSizeY >> 1) - 130.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeapon);

	//Select Weapon Text
	m_OrthoSelectWeaponName.fWidth = 512.f * 0.6f;
	m_OrthoSelectWeaponName.fHeight = 64.f * 0.6f;
	m_OrthoSelectWeaponName.fX = m_OrthoSelectWeapon.fX;
	m_OrthoSelectWeaponName.fY = m_OrthoSelectWeapon.fY + 130.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponName);

	// Select Weapon ��ȭ Ƚ��
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		m_OrthoSelectWeaponUpgrade[i].fWidth = 50.f;
		m_OrthoSelectWeaponUpgrade[i].fHeight = 50.f;
		m_OrthoSelectWeaponUpgrade[i].fX = m_OrthoSelectWeaponName.fX - 100.f + (i * 50.f);
		m_OrthoSelectWeaponUpgrade[i].fY = m_OrthoSelectWeaponName.fY + 40.f;
		CAppManager::ComputeOrtho(&m_OrthoSelectWeaponUpgrade[i]);
	}

	// Select Weapon ���ݷ� ��׶���
	m_OrthoSelectWeaponCriBack.fWidth = 512.f * 0.6f;
	m_OrthoSelectWeaponCriBack.fHeight = 64.f * 0.6f;
	m_OrthoSelectWeaponCriBack.fX = m_OrthoSelectWeaponName.fX;
	m_OrthoSelectWeaponCriBack.fY = m_OrthoSelectWeaponUpgrade[0].fY + (m_OrthoSelectWeaponUpgrade[0].fHeight * 0.5f) + (m_OrthoSelectWeaponCriBack.fHeight * 0.5f) + 10.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponCriBack);

	// Select Weapon ġ��ŸȮ�� ��׶���
	m_OrthoSelectWeaponAtkBack.fWidth = 512.f * 0.6f;
	m_OrthoSelectWeaponAtkBack.fHeight = 64.f * 0.6f;
	m_OrthoSelectWeaponAtkBack.fX = m_OrthoSelectWeaponName.fX;
	m_OrthoSelectWeaponAtkBack.fY = m_OrthoSelectWeaponCriBack.fY + m_OrthoSelectWeaponAtkBack.fHeight + 10.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponAtkBack);

	for (_uint i = 0; i < 6; ++i)
	{
		m_OrthoSelectWeaponAttack[i].fWidth = 16.f * 0.8f;
		m_OrthoSelectWeaponAttack[i].fHeight = 25.f * 0.8f;
		m_OrthoSelectWeaponAttack[i].fX = m_OrthoSelectWeaponName.fX + 50.f + (i * (16.f * 0.8f));
		m_OrthoSelectWeaponAttack[i].fY = m_OrthoSelectWeaponCriBack.fY;
		CAppManager::ComputeOrtho(&m_OrthoSelectWeaponAttack[i]);
	}

	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoSelectWeaponCri[i].fWidth = 16.f * 0.8f;
		m_OrthoSelectWeaponCri[i].fHeight = 25.f * 0.8f;
		m_OrthoSelectWeaponCri[i].fX = m_OrthoSelectWeaponName.fX + 50.f + (i * (16.f * 0.8f));
		m_OrthoSelectWeaponCri[i].fY = m_OrthoSelectWeaponAtkBack.fY;
		CAppManager::ComputeOrtho(&m_OrthoSelectWeaponCri[i]);
	}

	m_OrthoWeaponConfirmButton.fWidth = 362.f * 0.8f;
	m_OrthoWeaponConfirmButton.fHeight = 214.f * 0.5f;
	m_OrthoWeaponConfirmButton.fX = m_OrthoSelectWeaponName.fX;
	m_OrthoWeaponConfirmButton.fY = m_OrthoSelectWeaponAtkBack.fY + 60.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponConfirmButton);

	m_OrthoWeaponConfirmText.fWidth = 512.f * 0.7f;
	m_OrthoWeaponConfirmText.fHeight = 64.f * 0.7f;
	m_OrthoWeaponConfirmText.fX = m_OrthoSelectWeaponName.fX;
	m_OrthoWeaponConfirmText.fY = m_OrthoSelectWeaponAtkBack.fY + 65.f;
	CAppManager::ComputeOrtho(&m_OrthoWeaponConfirmText);


#pragma endregion

#pragma region ECHO_ORGHO

	// ���ε��� ���ڽ���
	m_OrthoBindEcho.fWidth = 100.f;
	m_OrthoBindEcho.fHeight = 100.f;
	m_OrthoBindEcho.fX = 200.f;
	m_OrthoBindEcho.fY = 200.f;
	CAppManager::ComputeOrtho(&m_OrthoBindEcho);

	m_OrthoBindEchoIcon.fWidth = 60.f;
	m_OrthoBindEchoIcon.fHeight = 60.f;
	m_OrthoBindEchoIcon.fX = 200.f;
	m_OrthoBindEchoIcon.fY = 200.f;
	CAppManager::ComputeOrtho(&m_OrthoBindEchoIcon);

	m_OrthoBindEchoEmpty.fWidth = 73.f;
	m_OrthoBindEchoEmpty.fHeight = 73.f;
	m_OrthoBindEchoEmpty.fX = 201.f;
	m_OrthoBindEchoEmpty.fY = 201.f;
	CAppManager::ComputeOrtho(&m_OrthoBindEchoEmpty);

	// ���ε� ���� �̸�
	m_OrthoEchoBack[0].fWidth = 256;
	m_OrthoEchoBack[0].fHeight = 32.f;
	m_OrthoEchoBack[0].fX = 360.f;
	m_OrthoEchoBack[0].fY = 182.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoBack[0]);

	m_OrthoEchoBack[1].fWidth = 256;
	m_OrthoEchoBack[1].fHeight = 32.f;
	m_OrthoEchoBack[1].fX = 360.f;
	m_OrthoEchoBack[1].fY = 222.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoBack[1]);

	m_OrthoEchoInfo[0].fWidth = 256 * 0.8f;
	m_OrthoEchoInfo[0].fHeight = 32.f * 0.8f;
	m_OrthoEchoInfo[0].fX = 370.f;
	m_OrthoEchoInfo[0].fY = 182.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoInfo[0]);

	m_OrthoEchoInfo[1].fWidth = 256;
	m_OrthoEchoInfo[1].fHeight = 32.f;
	m_OrthoEchoInfo[1].fX = 395.f;
	m_OrthoEchoInfo[1].fY = 223.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoInfo[1]);

	// ���ε� ���� ���ݷ�
	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoEchoDamage[i].fWidth = 16.f * 0.7f;
		m_OrthoEchoDamage[i].fHeight = 25.f * 0.7f;
		m_OrthoEchoDamage[i].fX = 335.f + (i * (15.f * 0.7f));
		m_OrthoEchoDamage[i].fY = 222.f;
		CAppManager::ComputeOrtho(&m_OrthoEchoDamage[i]);
	}

	// ���� ����
	for (_uint i = 0; i < ECHO_SLOT_MAX; ++i)
	{
		m_OrthoEchoSlot[i].fWidth = 74.f;
		m_OrthoEchoSlot[i].fHeight = 74.f;
		m_OrthoEchoSlot[i].fX = 200.f + ((i % 4) * 74.f);
		m_OrthoEchoSlot[i].fY = 300.f + ((i / 4) * 74.f);
		CAppManager::ComputeOrtho(&m_OrthoEchoSlot[i]);

		// �׷��̵� 3�ܰ�
		for (_uint iGrade = 0; iGrade < 3; ++iGrade)
		{
			m_OrthoGradeImage[i][iGrade].fWidth  = 16.f;
			m_OrthoGradeImage[i][iGrade].fHeight = 4.f;
			m_OrthoGradeImage[i][iGrade].fX = m_OrthoEchoSlot[i].fX - 22.f;
			m_OrthoGradeImage[i][iGrade].fY = m_OrthoEchoSlot[i].fY + 29.f - (5.f * iGrade);
			CAppManager::ComputeOrtho(&m_OrthoGradeImage[i][iGrade]);
		}

		m_OrthoEchoIcon[i].fWidth = 64.f;
		m_OrthoEchoIcon[i].fHeight = 64.f;
		m_OrthoEchoIcon[i].fX = 200.f + ((i % 4) * 74.f);
		m_OrthoEchoIcon[i].fY = 300.f + ((i / 4) * 74.f);
		CAppManager::ComputeOrtho(&m_OrthoEchoIcon[i]);

		m_OrthoEchoToolTip[i].fWidth = 256.f * 0.8f;
		m_OrthoEchoToolTip[i].fHeight = 129.f * 0.8f;
		m_OrthoEchoToolTip[i].fX = m_OrthoEchoSlot[i].fX + (256.f * 0.8f) * 0.5f + 35.f;
		m_OrthoEchoToolTip[i].fY = m_OrthoEchoIcon[i].fY + 10.f;
		CAppManager::ComputeOrtho(&m_OrthoEchoToolTip[i]);

		m_OrthoEchoToolTipName[i].fWidth = 256.f * 0.7f;
		m_OrthoEchoToolTipName[i].fHeight = 32.f * 0.7f;
		m_OrthoEchoToolTipName[i].fX = m_OrthoEchoSlot[i].fX + (256.f * 0.8f) * 0.5f + 38.f;
		m_OrthoEchoToolTipName[i].fY = m_OrthoEchoIcon[i].fY - (32.f * 0.8f) * 0.5f;
		CAppManager::ComputeOrtho(&m_OrthoEchoToolTipName[i]);

		m_OrthoEchoToolTipAttack[i].fWidth = 256.f * 0.8f;
		m_OrthoEchoToolTipAttack[i].fHeight = 32.f * 0.8f;
		m_OrthoEchoToolTipAttack[i].fX = m_OrthoEchoSlot[i].fX + (256.f * 0.8f) * 0.5f + 52.f;
		m_OrthoEchoToolTipAttack[i].fY = m_OrthoEchoIcon[i].fY + (32.f * 0.8f) * 0.5f;
		CAppManager::ComputeOrtho(&m_OrthoEchoToolTipAttack[i]);

		for (_uint iDigit = 0; iDigit < 3; ++iDigit)
		{
			m_OrthoEchoToolTipDamage[i][iDigit].fWidth = 16.f * 0.5f;
			m_OrthoEchoToolTipDamage[i][iDigit].fHeight = 25.f * 0.5f;
			m_OrthoEchoToolTipDamage[i][iDigit].fX = m_OrthoEchoSlot[i].fX + (256.f * 0.8f) * 0.5f + (iDigit * (15.f * 0.5f));
			m_OrthoEchoToolTipDamage[i][iDigit].fY = m_OrthoEchoToolTipAttack[i].fY - 1.f;
			CAppManager::ComputeOrtho(&m_OrthoEchoToolTipDamage[i][iDigit]);
		}
	}

	//���� ���׷��̵� ��׶���
	m_OrthoEchoUpgradeBack.fWidth = 340.f;
	m_OrthoEchoUpgradeBack.fHeight = 100.f;
	m_OrthoEchoUpgradeBack.fX = g_iWinSizeX >> 2;
	m_OrthoEchoUpgradeBack.fY = g_iWinSizeY - 165.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoUpgradeBack);

	// ��Ϲ�ư
	m_OrthoRegisterButton.fWidth = 236.f;
	m_OrthoRegisterButton.fHeight = 90.f;
	m_OrthoRegisterButton.fX = (g_iWinSizeX >> 2) + 50.f;
	m_OrthoRegisterButton.fY = g_iWinSizeY - 90.f;
	CAppManager::ComputeOrtho(&m_OrthoRegisterButton);
	m_OrthoRegisterText.fWidth = 256.f * 0.6f;
	m_OrthoRegisterText.fHeight = 32.f * 0.6f;
	m_OrthoRegisterText.fX = (g_iWinSizeX >> 2) + 50.F;
	m_OrthoRegisterText.fY = g_iWinSizeY - 85.f;
	CAppManager::ComputeOrtho(&m_OrthoRegisterText);
	//�� ���� �����̤�

#pragma endregion
	
	// ��Ÿ �ӽ� ����Ʈ ����
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

	CItemDB* pDB = CItemDB::GetInstance();
	CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::EXP0);
	itemDesc.iAmount = 10;
	m_pInven->PushItemDesc(itemDesc);

	itemDesc = pDB->GetItemData(ITEM::EXP1);
	itemDesc.iAmount = 5;
	m_pInven->PushItemDesc(itemDesc);

	itemDesc = pDB->GetItemData(ITEM::EXP2);
	itemDesc.iAmount = 5;
	m_pInven->PushItemDesc(itemDesc);
	
	return S_OK;
}

void CResonatorUI::Start()
{
}

void CResonatorUI::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	if (!m_bRender)
		return;

	stateUpdate(TimeDelta);
	elemAlphaUpdate(TimeDelta);
	keyInput(TimeDelta);

}

void CResonatorUI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if(m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CResonatorUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(defaultSlotRender()))
		return E_FAIL;

	switch (m_eCurButton)
	{
	case CResonatorUI::BTN_STATE:
		if (FAILED(stateRender()))
			return E_FAIL;

		if (FAILED(stateData()))
			return E_FAIL;

		break;
	case CResonatorUI::BTN_WEAPON:
		if (FAILED(weaponStateRender()))
			return E_FAIL;

		if (FAILED(weaponSwitchRender()))
			return E_FAIL;

		break;
	case CResonatorUI::BTN_ECHO:
		if (FAILED(echoSlotRender()))
			return E_FAIL;

		break;
	case CResonatorUI::BTN_RESONANCE:
		break;
	case CResonatorUI::BTN_WUTERIDE:
		break;
	case CResonatorUI::BTN_END:
		break;
	}

	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

void CResonatorUI::RenderGUI()
{
}

void CResonatorUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CResonatorUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();

}

void CResonatorUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CResonatorUI::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CResonatorUI::elemAlphaUpdate(_double TimeDelta)
{
	// ���� ��� ���� �ø��� 10���� ���� ����
	for (_uint i = 0; i < SMOOTH_MAX; ++i)
	{
		if (m_bElemAlphaStart[i])
		{
			m_fElemAlpha[i] += (_float)TimeDelta * 2.f;
			if (m_fElemAlpha[i] >= 1.f)
				m_fElemAlpha[i] = 1.f;
		}

		if (m_fElemAlpha[i] >= 0.3f)
		{
			if (i < SMOOTH_MAX - 1)
				m_bElemAlphaStart[i + 1] = true;

		}
	}
	
	if (m_eCurButton == BTN_STATE)
	{
		if (m_fElemAlpha[6] >= 1.f)
		{
			if (!m_bSlotRenderFinish)
			{
				m_bSlotRenderFinish = true;
				for (_uint i = 0; i < 4; ++i)
					m_bFlashStart[i] = true;
			}
		}
	}
	else if (m_eCurButton == BTN_WEAPON)
	{
		if (m_fElemAlpha[7] >= 1.f)
		{
			if (!m_bSlotRenderFinish)
			{
				m_bSlotRenderFinish = true;
				for (_uint i = 0; i < 4; ++i)
					m_bFlashStart[i] = true;
			}
		}
	}


}

void CResonatorUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}

	ZeroMemory(m_iUseAmount, sizeof m_iUseAmount);
	ZeroMemory(m_fFlashCount, sizeof m_fFlashCount);
	ZeroMemory(m_bFlashStart, sizeof m_bFlashStart);
	ZeroMemory(m_fFlashAcc, sizeof m_fFlashAcc);
	ZeroMemory(m_fToolTipAcc, sizeof m_fToolTipAcc);
	ZeroMemory(m_bToolTip, sizeof m_bToolTip);

	m_iOnMouseEchoSlot = 0;
	m_bSwitchButton = false;
	m_bElemAlphaStart[0] = true;
	m_bSlotRenderFinish = false;
	m_iSelectSlot = 0;
}

void CResonatorUI::stateActive()
{
	//�ʱ���� ����
	m_bRender = true;
	m_fBackgroundAlpha = 1.f;
	m_bButtonActive[0] = true;
	m_eCurButton = BTN_STATE;
	m_bSlotRenderFinish = false;
	m_bSwitchButton = false;

	ZeroMemory(m_iUseAmount, sizeof m_iUseAmount);
	ZeroMemory(m_fFlashCount, sizeof m_fFlashCount);
	ZeroMemory(m_bFlashStart, sizeof m_bFlashStart);
	ZeroMemory(m_fFlashAcc, sizeof m_fFlashAcc);
	m_iSelectSlot = 0;

	ZeroMemory(m_fToolTipAcc, sizeof m_fToolTipAcc);
	ZeroMemory(m_bToolTip, sizeof m_bToolTip);
	m_iOnMouseEchoSlot = 0;

	elemAlphaReset();

	m_pUICharacter->ExitAnimation(UICharacter::UI_STATE);
	m_pCamMovement->UseCamera(CCameraMovement::CAM_UI);
}

void CResonatorUI::stateDisable()
{
	m_pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
}

void CResonatorUI::stateUpdate(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_bActive)
	{
		for (_uint i = 0; i < BTN_END; ++i)
		{
			if (m_bButtonActive[i])
			{
				m_fButtonAlpha[i] += (_float)TimeDelta * 2.f;

				if (pGameMode->OnMouse(m_OrthoButtons[i]) 
					&& pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					if (m_eCurButton != (RESONATOR_BTN)i)
					{
						elemAlphaReset();
						m_pUICharacter->SetAnimation((UICharacter::UIANIMATION)i);
					}

					m_eCurButton = (RESONATOR_BTN)i;
				}

			}

			if (m_fButtonAlpha[i] >= 0.5f && i < BTN_END - 1)
				m_bButtonActive[i + 1] = true;

			if (m_fButtonAlpha[i] > 1.f)
				m_fButtonAlpha[i] = 1.f;
		}

		
	}
	else
	{
		for (_uint i = 0; i < BTN_END; ++i)
		{
			m_fBackgroundAlpha = m_fButtonAlpha[i] -= (_float)TimeDelta;
			if (m_fButtonAlpha[i] < 1.f)
			{
				m_fBackgroundAlpha = 0.f;
				m_fButtonAlpha[i] = 0.f;
				m_bButtonActive[i] = false;
				m_bRender = false;
			}

		}
	}
}

_uint CResonatorUI::getCurSlotText(RESONATOR_BTN eButton)
{
	_uint iTextureID = 0;

	switch (eButton)
	{
	case BTN_STATE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_STATETEXT;
		break;
	case BTN_WEAPON:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_WEAPONTEXT;
		break;
	case BTN_ECHO:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_ECHOTEXT;
		break;
	case BTN_RESONANCE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_RESONANCETEXT;
		break;
	case BTN_WUTERIDE:
		iTextureID = STATIC_IMAGE::UI_RESONATOR_WUTHERIDETEXT;
		break;
	}

	return iTextureID;
}

_uint CResonatorUI::getCurSlotIcon(RESONATOR_BTN eButton)
{
	_uint iTextureID = 0;

	switch (eButton)
	{
	case BTN_STATE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_STATE;
		break;
	case BTN_WEAPON:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_WEAPON;
		break;
	case BTN_ECHO:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_ECHO;
		break;
	case BTN_RESONANCE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_RENONANCE;
		break;
	case BTN_WUTERIDE:
		iTextureID = STATIC_IMAGE::ICON_RESONATOR_WUTHERIDE;
		break;
	}

	return iTextureID;
}

_uint CResonatorUI::getStateImage(_uint iIndex)
{
	_uint iTextureID = 0;

	switch (iIndex)
	{
	case 0:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_HP;
		break;
	case 1:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_ATTACK;
		break;
	case 2:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_DEFENSE;
		break;
	case 3:
		iTextureID = STATIC_IMAGE::RESONATOR_STATE_CRITRATE;
		break;
	}

	return iTextureID;
}

_uint CResonatorUI::getWeaponText()
{
	CItemDB* pDB = CItemDB::GetInstance();
	CItem::ITEM_DESC* tagItem = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter);
	
	_uint iTitleTextureID = 0;

	if (m_iCurCharacter == CPlayerState::CHARACTERS::CHARACTER_CHIXIA)
	{
		switch (tagItem->iItemID)
		{
		case ITEM::GUN0:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_GUN_0;
			break;
		case ITEM::GUN1:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_GUN_1;
			break;
		case ITEM::GUN2:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_GUN_2;
			break;
		case ITEM::GUN3:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_GUN_3;
			break;
		case ITEM::GUN4:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_GUN_4;
			break; 
		}
	}
	else
	{
		switch (tagItem->iItemID)
		{
		case ITEM::SWORD0:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_SWORD_0;
			break;
		case ITEM::SWORD1:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_SWORD_1;
			break;
		case ITEM::SWORD2:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_SWORD_2;
			break;
		case ITEM::SWORD3:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_SWORD_3;
			break;
		case ITEM::SWORD4:
			iTitleTextureID = STATIC_IMAGE::TEXT_TITLE_SWORD_4;
			break;
		}
	}

	return iTitleTextureID;
}

HRESULT CResonatorUI::defaultSlotRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Background
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
	//	return E_FAIL;

	//if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fBackgroundAlpha, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DETAILPANEL, m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(10);
	//m_pVIBuffer->Render();

	// ButtonTitleText
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTitleText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(getCurSlotText(m_eCurButton), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// BTN (Slot)
	for (_uint i = 0; i < BTN_END; ++i)
	{
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButtons[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fButtonAlpha[i], sizeof(_float))))
			return E_FAIL;

		if (m_eCurButton == i)
		{
			_float3 vColor = LEGEND_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
		}
		else
		{
			_float3 vColor = DEFAULT_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
		}


		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOW_SOLID_BUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoButtonIcons[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(getCurSlotIcon((RESONATOR_BTN)i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

HRESULT CResonatorUI::stateRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//���� ĳ���� ����
	chooseCharacter();

	// ����ġ ĸ������
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;

	for (_uint iGrade = 0; iGrade < 4; ++iGrade)
	{
		CItemDB* pDB = CItemDB::GetInstance();
		CItem::ITEM_DESC itemDesc;

		_float3 v3Color;
		switch (iGrade)
		{
		case 0:
			v3Color = SLOT_COLOR;
			itemDesc = pDB->GetItemData(ITEM::EXP0);
			break;
		case 1:
			v3Color = SLOT_ADVANCED_COLOR;
			itemDesc = pDB->GetItemData(ITEM::EXP1);
			break;
		case 2:
			v3Color = SLOT_RARE_COLOR;
			itemDesc = pDB->GetItemData(ITEM::EXP2);
			break;
		case 3:
			v3Color = SLOT_UNIQUE_COLOR;
			itemDesc = pDB->GetItemData(ITEM::EXP3);
			break;
		}
		
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeSlot[iGrade].WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &v3Color, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCapsule[iGrade].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(itemDesc.iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		//����ġ ĸ�� ����
		_uint iAmountCapsule = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::EXP0 + iGrade);

		//������ 0���϶�
		if (iAmountCapsule == 0)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCapsule[iGrade].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SLOT_EMPTY, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			continue;
		}
		else
		{
			if (m_bFlashStart[iGrade])
			{
				//OnMouse�ϋ� Ŭ���ҋ� �����ϵ��� �����Ұ�
				_float2 fXY = { 5.f, 6.f };
				_float4 vColor4 = { 1.f, 1.f, 1.f, 0.3f };
				if (FAILED(m_pShader->SetRawValue("g_CurrentCount", &m_fFlashCount[iGrade], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_SpriteXY", &fXY, sizeof(_float2))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCapsule[iGrade].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SPRITE_UI_FLASH, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;

				m_pShader->Begin(6);
				m_pVIBuffer->Render();
			}

		}

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;

		//ĸ�� ����� ���� 4�ڸ� ������ ǥ��
		string strUseAmount = to_string(m_iUseAmount[iGrade]);
		_uint  iDigitCount = (_uint)strUseAmount.size();

		for (_uint iDigit = 0; iDigit < iDigitCount; ++iDigit)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCapsuleAmount[iGrade][iDigit].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strUseAmount[iDigit] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	// Consume Coin Back
	_float fAlpha = m_fElemAlpha[7] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCoinBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	// Consume Coin Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoConsumeText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CONSUME, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Own Coin Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoOwnText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_OWN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Need Consume Coin 
	string strOwnCoin = to_string(m_iCurCoin);
	_uint iOwnDigit = (_uint)strOwnCoin.size();

	string strNeedCoin = to_string(m_iTotalConsume);
	_uint iDigit = (_uint)strNeedCoin.size();
	if (iDigit >= DRAW_MAX || iOwnDigit >= DRAW_MAX)
		iDigit = DRAW_MAX;

	_float3 vCoinColor = m_iCurCoin > m_iTotalConsume ? _float3(1.f, 1.f, 1.f) : _float3(0.8f, 0.1f, 0.1f);
	for (_uint i = 0; i < iDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vCoinColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoConsumeCoin[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strNeedCoin[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// Total Own Coin 
	for (_uint i = 0; i < iOwnDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoOwnCoin[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strOwnCoin[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// CoinIcon
	CItemDB* pDB = CItemDB::GetInstance();
	CItem::ITEM_DESC coinDesc = pDB->GetItemData(ITEM::TACTITE_COIN);

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCoinIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(coinDesc.iImageIndex, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Upgrade Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Upgrade Button
	_float3 vBtnColor = DEFAULT_COLOR;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vBtnColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeBtn.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOW_UPARROW_BUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// Upgrade Button(Push Color)
	if (m_fPushAcc > 0.f)
	{
		_float3 vBtnColor = LEGEND_COLOR;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vBtnColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fPushAcc, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeBtn.WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOW_UPARROW_BUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(3);
		m_pVIBuffer->Render();
	}

	// State
	for (_uint i = 0; i < STATE_END; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoState[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_LISTBACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(getStateImage(i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// Player Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoPlayerText.WorldMatrix)))
		return E_FAIL;

	switch (m_iCurCharacter)
	{
	case 0:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_ROVER, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_YANGYANG, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_CHIXIA, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Level Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevelText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_RESONATOR_LEVEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	CPlayerState::CHARACTER_STATE* pPlayerState = m_pPlayerState->Get_CharState_byChar(m_iCurCharacter);
	string strCurLevel = to_string((_int)pPlayerState->iCurLevel);
	_uint iLevelDigit = (_uint)strCurLevel.size();

	//CurLevel
	for (_uint i = 0; i < iLevelDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCurLevel[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// ������
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SLASH, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	// MaxLevel = 50
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit + 1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_5, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[iLevelDigit + 2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_0, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Exp Gage Back
	_float3 vColor = { 0.7f, 0.7f, 0.7f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoExp.WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))	
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	_float fFill = pPlayerState->fCurExp / pPlayerState->fMaxExp;

	// ExpGage Front
	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &fFill, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoExp.WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(2);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CResonatorUI::stateData()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CPlayerState::CHARACTER_STATE* pPlayerState = m_pPlayerState->Get_CharState_byChar((CPlayerState::CHARSLOT)m_iCurCharacter);

	// ����� ���� ���� ���Ŀ� ���� ���õ� ĳ���� �Ķ���ͷ� ������ ���� �����ֱ�
	string strData[4];
	strData[0] = to_string((_int)pPlayerState->fMaxHP);
	strData[1] = to_string((_int)pPlayerState->fAttack[0]);
	strData[2] = to_string((_int)pPlayerState->fDefense[0]);
	strData[3] = to_string((_int)pPlayerState->fCriticalRate[0]);

	// State
	for (_uint i = 0; i < STATE_END; ++i)
	{
		// State Data
		_uint dataSize = (_uint)strData[i].size();
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
			return E_FAIL;

		for (_uint iDigit = 0; iDigit < dataSize; ++iDigit)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoStateData[i][iDigit].WorldMatrix)))
				return E_FAIL;

			_uint dataNumber = strData[i][iDigit] - '0';
			if (FAILED(pGameInstance->SetupSRV(dataNumber, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	// �ۼ�Ʈ ������
	_uint iPerIndex = (_uint)strData[3].size();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoStateData[STATE_CRITRATE][iPerIndex].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	return S_OK;
}

void CResonatorUI::selectCharacter(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
	{
		//Chracter Icon Click
		for (_uint i = 0; i < 3; ++i)
		{
			if (pGM->OnMouse(m_OrthoCharBack[i]))
			{
				if (m_iCurCharacter != i)
					elemAlphaReset(1);

				m_iCurCharacter = i;
			}

		}

	}
}

void CResonatorUI::stateKeyInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!m_bSwitchButton)
	{
		selectCharacter(TimeDelta);

		// ����ġ ���� �ӽ�
		if (pGameInstance->InputKey(DIK_B) == KEY_STATE::TAP)
			m_pPlayerState->AddExp((CPlayerState::CHARACTERS)m_iCurCharacter, 10.f);

		//����� ����ġ ĸ�� �߰�/����
		for (_uint iCapsule = 0; iCapsule < 4; ++iCapsule)
		{
			_uint iItemAmount = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::EXP0 + iCapsule);
			if (pGM->OnMouse(m_OrthoUpgradeSlot[iCapsule]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					if (iItemAmount > m_iUseAmount[iCapsule])
					{
						//��Ŭ���� ����
						m_fFlashCount[iCapsule] = 0.f;
						m_fFlashAcc[iCapsule] = 0.f;

						m_bFlashStart[iCapsule] = true;
						m_iUseAmount[iCapsule]++;
					}
				}

				if (pGameInstance->InputMouse(DIMK_RB) == KEY_STATE::TAP)
				{
					if (0 < m_iUseAmount[iCapsule])
					{
						//��Ŭ���� ����
						m_fFlashCount[iCapsule] = 0.f;
						m_fFlashAcc[iCapsule] = 0.f;

						m_bFlashStart[iCapsule] = true;
						m_iUseAmount[iCapsule]--;
					}
				}
			}
		}

		m_fTotalExp = 0.f;
		m_iTotalConsume = 0;
		m_iCurCoin = m_pInven->GetCoin();
		CItemDB* pDB = CItemDB::GetInstance();
		for (_uint i = 0; i < 4; ++i)
		{
			CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::EXP0 + i);
			m_fTotalExp += itemDesc.iData[0] * m_iUseAmount[i];
			m_iTotalConsume += itemDesc.iData[1] * m_iUseAmount[i];
		}

		if (pGM->OnMouse(m_OrthoUpgradeBtn))
		{
			if (m_iTotalConsume > 0 && m_iTotalConsume <= m_iCurCoin &&
				pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			{
				m_fPushAcc += (_float)TimeDelta;
				if (m_fPushAcc > 1.f)
				{
					m_fPushAcc = 1.f;
					m_bUpgradeConfirm = true;
				}
			}
			else
			{
				m_fPushAcc = 0.f;	
				m_bUpgradeConfirm = false;
			}
		}
		else
		{
			m_fPushAcc = 0.f;
			m_bUpgradeConfirm = false;
		}

		if (m_bUpgradeConfirm)
		{
			m_fPushAcc = 0.f;
			m_bUpgradeConfirm = false;
			upgradeCharacter(m_iCurCharacter);
			ZeroMemory(m_iUseAmount, sizeof(_int) * 4);
		}
	}

	//Capsule FlashSlot
	for (_uint i = 0; i < 4; ++i)
	{
		if (m_bFlashStart[i])
		{
			m_fFlashAcc[i] += (_float)TimeDelta;
			if (m_fFlashAcc[i] >= 0.02f)
			{
				m_fFlashAcc[i] = 0.f;

				m_fFlashCount[i] += 1.f;
				if (m_fFlashCount[i] > 29.f)
				{
					m_bFlashStart[i] = false;
					m_fFlashCount[i] = 0.f;
				}
			}
		}
	}
}

void CResonatorUI::echoKeyInput(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	selectCharacter(TimeDelta);

	// Tooltip Alpha ����
	for (_uint i = 0; i < ECHO_SLOT_MAX; ++i)
	{
		CEchoSystem::ECHO EchoData = m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)i);
		if (pGameMode->OnMouse(m_OrthoEchoIcon[i]) && EchoData.bActive)
		{
			if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				m_iCurSelectEcho = i;

			m_bToolTip[i] = true;
			m_iOnMouseEchoSlot = i;
			m_fToolTipAcc[i] += (_float)TimeDelta * 4.f;
			if (m_fToolTipAcc[i] > 8.f)
				m_fToolTipAcc[i] = 8.f;
		}
		else
		{
			m_bToolTip[i] = false;
			m_fToolTipAcc[i] = 0.f;
		}
	}

	//���� ����
	const CEchoSystem::ECHO* pBindEcho = m_pEchoSystem->GetRegisteredEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter);
	if (pBindEcho)
	{
		if (pGameMode->OnMouse(m_OrthoBindEcho))
		{
			if (pGameInstnace->InputMouse(DIMK_RB) == KEY_STATE::TAP)
				m_pEchoSystem->ReleaseEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter);
		}
	}

	//���� ���
	if (pGameMode->OnMouse(m_OrthoRegisterText))
	{
		m_iRegisterBtnState = 1;
		if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			m_iRegisterBtnState = 2;

		if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
			m_pEchoSystem->BindEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter, (CEchoSystem::ECHO_TYPE)m_iCurSelectEcho);

	}
	else
	{
		m_iRegisterBtnState = 0;
	}

}

void CResonatorUI::weaponKeyInput(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	if (!m_bSwitchButton)
	{
		selectCharacter(TimeDelta);
		pushUpgradeButton(TimeDelta);
		upshSwitchButton(TimeDelta);
	}
	else
	{
		// Select Weapon ������ �ݱ�
		if (m_bSwitchButton)
		{
			if (pGameMode->OnMouse(m_OrthoSwitchBackpage))
			{
				if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					m_bSwitchButton = false;
			}

			//���� üũ
			for (_uint i = 0; i < 20; ++i)
			{
				if (pGameMode->OnMouse(m_OrthoWeaponIcon[i]))
				{
					if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						m_iSelectSlot = i;
				}
			}

			// WeaponChange Confirm
			if (pGameMode->OnMouse(m_OrthoWeaponConfirmText))
			{
				if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
					m_iConfirmButtonState = BTN_CLICK;
				else
				{
					if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::AWAY && m_iConfirmButtonState == BTN_CLICK)
					{
						m_bWeaponChangeConfirm = true;
						m_iConfirmButtonState = BTN_ON;
						m_pInven->SwapWeapon(m_iCurCharacter, m_iSelectSlot);
						//���⺯��
					}
				}
			}
			else
			{
				m_iConfirmButtonState = BTN_NONE;
			}
		}
	}

	flashGemSlot(TimeDelta);
}

void CResonatorUI::pushUpgradeButton(_double TimeDelta)
{
	// ���׷��̵� ��ư ��
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	CItem::ITEM_DESC* pWeapon = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter);
	m_iOwnGem = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::GEM);
	m_iGemCost = (pWeapon->eItemGrade + 1) * 10;
	if (pWeapon->iData[2] < 5 && m_iOwnGem >= m_iGemCost)
	{
		if (pGameMode->OnMouse(m_OrthoWeaponUpgradeButton))
		{
			if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			{
				m_fPushAcc += (_float)TimeDelta;
				if (m_fPushAcc >= 1.f)
				{
					m_fPushAcc = 0.f;
					m_bUpgradeConfirm = true;
				}
			}
			else
			{
				m_fPushAcc = 0.f;
				m_bUpgradeConfirm = false;
			}

		}
		else
		{
			m_bUpgradeConfirm = false;
		}

		if (m_bUpgradeConfirm)
		{
			m_bUpgradeConfirm = false;
			upgradeWeapon();
		}
	}
	else
	{
		m_fPushAcc = 0.f;
	}
}

void CResonatorUI::upshSwitchButton(_double TimeDelta)
{
	// ����ġ ��ư ��
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnace = CGameInstance::GetInstance();

	if (!m_bSwitchButton)
	{
		if (pGameMode->OnMouse(m_OrthoWeaponSwitchButton))
		{
			if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			{
				m_fSwitchWeaponAcc += (_float)TimeDelta;
				if (m_fSwitchWeaponAcc >= 1.f)
				{
					m_fSwitchWeaponAcc = 0.f;
					m_bSwitchButton = true;
				}
			}
			else
			{
				m_fSwitchWeaponAcc = 0.f;
				m_bSwitchButton = false;
			}

		}
		else
		{
			m_fSwitchWeaponAcc = 0.f;
		}
	}
	else
	{
		m_fSwitchWeaponAcc = 0.f;
	}
}

void CResonatorUI::flashGemSlot(_double TimeDelta)
{
	//Capsule FlashSlot
	for (_uint i = 0; i < 4; ++i)
	{
		if (m_bFlashStart[i])
		{
			m_fFlashAcc[i] += (_float)TimeDelta;
			if (m_fFlashAcc[i] >= 0.02f)
			{
				m_fFlashAcc[i] = 0.f;

				m_fFlashCount[i] += 1.f;
				if (m_fFlashCount[i] > 29.f)
				{
					m_bFlashStart[i] = false;
					m_fFlashCount[i] = 0.f;
				}
			}
		}
	}
}

void CResonatorUI::upgradeCharacter(_uint iCharacterType)
{
	for (_uint i = 0; i < 4; ++i)
		m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::EXP0 + i, m_iUseAmount[i]);

	m_pInven->DeleteCoin(m_iTotalConsume);
	m_pPlayerState->AddExp((CPlayerState::CHARACTERS)iCharacterType, m_fTotalExp);
}

HRESULT CResonatorUI::weaponStateRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	//���� ĳ���� ����
	chooseCharacter();

	//���� �̸�
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponName.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(getWeaponText(), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//���� �� ����
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponDesc.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_WEAPONDESC, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	_float3 vDescColor = { 0.5f, 0.5f, 0.5f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vDescColor, sizeof(_float3))))
		return E_FAIL;

	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// ���� ��ȭ ǥ��
	CItem::tagItemDesc* pItem = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter);
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeSign[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_UPGRADE_SIGN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		//������ ��ȭ Ƚ��
		_float3 vSignColor = LEGEND_COLOR;
		if (i < (_uint)pItem->iData[2])
		{
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vSignColor, sizeof(_float3))))
				return E_FAIL;
		}
		else
		{
			vSignColor = { 0.5f, 0.5f, 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vSignColor, sizeof(_float3))))
				return E_FAIL;
		}

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// ���ݷ� ��׶���
	_float fAlpha = m_fElemAlpha[4] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDamageBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	// ġ��Ÿ ������ ��׶���
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCriDamageBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	// PhisicalRate Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackText[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PHISICAL_RATE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ������
	_uint iDamage = CItemDB::ComputeWeaponDamage(*pItem);
	string strDamage = to_string(iDamage);
	_uint iDamageDigit = (_uint)strDamage.size();

	for (_uint iDamageIndex = 0; iDamageIndex < iDamageDigit; ++iDamageIndex)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDamage[iDamageIndex].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strDamage[iDamageIndex] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// CriticalDamage Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackText[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CRITICAL_DAMAGE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ũ��Ƽ�� Ȯ��
	iDamage = CItemDB::ComputeWeaponCriticalRate(*pItem);
	strDamage = to_string(iDamage);
	iDamageDigit = (_uint)strDamage.size();

	for (_uint iDamageIndex = 0; iDamageIndex < iDamageDigit; ++iDamageIndex)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCriRate[iDamageIndex].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strDamage[iDamageIndex] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// �ۼ�Ʈ
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCriRate[iDamageDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// �ڽ�Ʈ ��׶���
	fAlpha = m_fElemAlpha[6] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCostBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// �ڽ�Ʈ �ؽ�Ʈ
	vDescColor = { 0.5f, 0.5f, 0.5f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vDescColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCostText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADECOST, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// ���� �� �ؽ�Ʈ
	vDescColor = { 0.5f, 0.5f, 0.5f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vDescColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeOwnCostText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_OWN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// Gem ����
	CItemDB* pDB = CItemDB::GetInstance();
	_float3 vGemSlotColor = CItemDB::GetItemColor(pDB->GetItemData(ITEM::GEM).eItemGrade);
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vGemSlotColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGemIconBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGemIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_GEM, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Flash
	if (m_fFlashCount[0] > 0.f)
	{
		_float2 fSpriteXY = { 5.f, 6.f };
		_float4 vColor4 = { 1.f, 1.f, 1.f, 0.3f };
		if (FAILED(m_pShader->SetRawValue("g_SpriteXY", &fSpriteXY, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_CurrentCount", &m_fFlashCount[0], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGemIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SPRITE_UI_FLASH, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(6);
		m_pVIBuffer->Render();
	}

	string strCost = to_string(m_iOwnGem);
	_uint iCostDigit = (_uint)strCost.size();
	// �ڽ�Ʈ (������)
	for (_uint iDigit = 0; iDigit < iCostDigit; ++iDigit)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoOwnGemCost[iDigit].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCost[iDigit] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// �ڽ�Ʈ (�ʿ�)
	strCost = to_string(m_iGemCost);
	iCostDigit = (_uint)strCost.size();
	
	// Text X
	_float3 vColor = { 1.f, 1.f, 1.f };
	if (m_iOwnGem < m_iGemCost)
		vColor = { 0.8f, 0.1f, 0.1f };

	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGemCost[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	for (_uint iDigit = 1; iDigit < iCostDigit + 1; ++iDigit)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGemCost[iDigit].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCost[iDigit - 1] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// ���׷��̵� ��ư
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradeButton.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_WEAPON_UPGRADE_BUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Fill
	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fPushAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradeButton.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_WEAPON_UPGRADE_BUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	// ���׷��̵� �ؽ�Ʈ
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradeText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ����ġ ��ư
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSwitchButton.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_WEAPON_SWITCH_BUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Fill
	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fSwitchWeaponAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSwitchButton.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_WEAPON_SWITCH_BUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	// ����ġ �ؽ�Ʈ
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSwitchText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SWITCH, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	return S_OK;
}

void CResonatorUI::upgradeWeapon()
{
	CItem::ITEM_DESC* pWeaponDesc = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter);
	pWeaponDesc->iData[2]++;
	m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::GEM, m_iGemCost);
}

HRESULT CResonatorUI::echoSlotRender()
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//���� ĳ���� ����
	chooseCharacter();

	// ���ε��� ����
	const CEchoSystem::ECHO* pBindEcho = m_pEchoSystem->GetRegisteredEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter);

	_float3 vColor;
	if (pBindEcho)
		vColor = GetEchoGradeColor3(pBindEcho->iGrade);
	else
		vColor = DEFAULT_COLOR;

	_float4 vColor4 = { vColor.x, vColor.y, vColor.z, m_fElemAlpha[1] };
	if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBindEcho.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(11);
	m_pVIBuffer->Render();	

	if (pBindEcho)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBindEchoIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(pBindEcho->iIconTextureID, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}
	else
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBindEchoEmpty.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::EMPTY_PLUS_SLOT, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// ���ε����� �̸�
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoBack[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	if (pBindEcho)
	{
		vColor = GetEchoGradeColor3(pBindEcho->iGrade);
		_float4 vColor4 = { vColor.x, vColor.y, vColor.z, m_fElemAlpha[1] };
		if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoInfo[0].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(pBindEcho->iNameTextureID, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(11);
		m_pVIBuffer->Render();
	}

	// ���ε����� ���ݷ�
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoBack[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	if (pBindEcho)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoInfo[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ATTACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// ���ε� ���ݷ� ��ġ
		string strDamage = to_string((_int)pBindEcho->fAttack);
		_uint iDigit = (_uint)strDamage.size();

		for (_uint i = 0; i < iDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoDamage[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strDamage[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

	}

	//���� ����
	for (_uint i = 0; i < ECHO_SLOT_MAX; ++i)
	{
		CEchoSystem::ECHO EchoData = m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)i);
		_float3 vColor = DEFAULT_COLOR;
		switch (EchoData.iGrade)
		{
		case 1:
			vColor = SLOT_ADVANCED_COLOR;
			break;
		case 2:
			vColor = SLOT_RARE_COLOR;
			break;
		case 3:
			vColor = SLOT_UNIQUE_COLOR;
			break;
		case 4:
			vColor = SLOT_LEGEND_COLOR;
			break;
		case 5:
			vColor = SLOT_RED_COLOR;
			break;
		}

		vColor4 = { vColor.x, vColor.y, vColor.z, m_fElemAlpha[2] };
		if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoSlot[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SLOT74, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(11);
		m_pVIBuffer->Render();

		if (EchoData.bActive)
		{
			//���� ������
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(EchoData.iIconTextureID, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoIcon[i].WorldMatrix)))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			for (_uint iLevel = 0; iLevel < EchoData.iLevel; ++iLevel)
			{
				/*vColor = GetEchoGradeColor3(4);
				if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
					return E_FAIL;*/
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GRADE_IMAGE, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoGradeImage[i][iLevel].WorldMatrix)))
					return E_FAIL;

				m_pShader->Begin(10);
				m_pVIBuffer->Render();
			}
		}
		else
		{
			// ���� ȸ�� ������
			vColor4 = { m_fElemAlpha[3], m_fElemAlpha[3] , m_fElemAlpha[3] , m_fElemAlpha[3] };
			if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(EchoData.iIconTextureID, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoIcon[i].WorldMatrix)))
				return E_FAIL;

			m_pShader->Begin(12);
			m_pVIBuffer->Render();

			// �ӽ� ������ (UnkownSlot)
			_float fAlpha = m_fElemAlpha[3] * 0.5f;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoSlot[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_ECHO_UNKNOWN, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

		}

		if (EchoData.bBind)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoSlot[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_USE, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	// ����
	if (m_bToolTip[m_iOnMouseEchoSlot])
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fToolTipAcc[m_iOnMouseEchoSlot], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoToolTip[m_iOnMouseEchoSlot].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_PANEL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		//���� ����
		CEchoSystem::ECHO EchoData = m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)m_iOnMouseEchoSlot);
		_float fTextAlpha = m_fToolTipAcc[m_iOnMouseEchoSlot] / 8.f;
		
		_float3 vEchoColor = GetEchoGradeColor3(EchoData.iGrade);
		_float4 vEchoNameColor = { vEchoColor.x, vEchoColor.y, vEchoColor.z, fTextAlpha };

		if (FAILED(m_pShader->SetRawValue("g_vColor4", &vEchoNameColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fTextAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoToolTipName[m_iOnMouseEchoSlot].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(EchoData.iNameTextureID, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(11);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fTextAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoToolTipAttack[m_iOnMouseEchoSlot].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ATTACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// ���� ������
		string strDamage = to_string((_int)EchoData.fAttack);
		_uint iDigitCount = (_uint)strDamage.size();

		for (_uint iDigit = 0; iDigit < iDigitCount; ++iDigit)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fTextAlpha, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoToolTipDamage[m_iOnMouseEchoSlot][iDigit].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strDamage[iDigit] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

	}

	// ���� Glow
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoSlot[m_iCurSelectEcho].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ���� ���׷��̵� ��׶���
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoUpgradeBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	// ��� ��ư
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoRegisterButton.WorldMatrix)))
		return E_FAIL;

	switch (m_iRegisterBtnState)
	{
	case 0:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_ON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_NONE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_CLICK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoRegisterText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_REGISTER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CResonatorUI::weaponSwitchRender()
{
	if (!m_bSwitchButton)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponInvenBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GRADBACKGROUND, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SELECTWEAPON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSwitchBackpage.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_BACKPAGE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Select Weapon Text
	_uint iSlotCount = m_pInven->GetInvenTotalCount(CInventory::INVEN_WEAPON);
	for (_uint i = 0; i < iSlotCount; ++i)
	{
		CItem::ITEM_DESC pSlotItem = m_pInven->GetSlotData(CInventory::INVEN_WEAPON, i);
	
		if (i == m_iSelectSlot)
		{
			/*if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
				return E_FAIL;*/
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlot[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

		// Weapon Slot
		_float3 vItemSlotColor = CItemDB::GetItemSlotColor(pSlotItem.eItemGrade);
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlot[i].WorldMatrix)))
			return E_FAIL;
		/*if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;*/
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SLOT74, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		// Weapon Icon
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponIcon[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(pSlotItem.iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (pSlotItem.iData[2] > 0)
		{
			// Weapon Upgrade Count +
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradePlus[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PLUS, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();

			// Weapon Upgrade Count
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradeCount[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(pSlotItem.iData[2], m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}
	}

	// Select Weapon Slot
	CItem::ITEM_DESC pSlotItem = m_pInven->GetSlotData(CInventory::INVEN_WEAPON, m_iSelectSlot);

	_float3 vItemSlotColor = CItemDB::GetItemSlotColor(pSlotItem.eItemGrade);
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// Select Weapon Icon
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeapon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(pSlotItem.iImageIndex, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Select Weapon Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponName.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(pSlotItem.iImageTextIndex, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// ���õ� ���� ��ȭ Ƚ��
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponUpgrade[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_UPGRADE_SIGN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		//������ ��ȭ Ƚ��
		_float3 vSignColor = LEGEND_COLOR;
		if (i < (_uint)pSlotItem.iData[2])
		{
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vSignColor, sizeof(_float3))))
				return E_FAIL;
		}
		else
		{
			vSignColor = { 0.5f, 0.5f, 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vSignColor, sizeof(_float3))))
				return E_FAIL;
		}

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// Info Back 0
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponCriBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();


	// Info Back 1
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponAtkBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	//// ��ġ (���� �ݴ�� ���ִµ�)
	// Phisical Damage Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponAtkBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CRITICAL_DAMAGE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Critical Rate Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponCriBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PHISICAL_RATE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//// ������
	_uint iDamage = CItemDB::ComputeWeaponDamage(pSlotItem);
	string strDamage = to_string(iDamage);
	_uint iDamageDigit = (_uint)strDamage.size();

	for (_uint iDamageIndex = 0; iDamageIndex < iDamageDigit; ++iDamageIndex)
	{
		_float3 vWeaponDMGColor;
		if (iDamage < (_uint)m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter)->iData[0])
			vWeaponDMGColor = _float3(0.8f, 0.2f, 0.2f);
		else if (iDamage == (_uint)m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter)->iData[0])
			vWeaponDMGColor = DEFAULT_COLOR;
		else
			vWeaponDMGColor = ADVANCED_COLOR;

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vWeaponDMGColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponAttack[iDamageIndex].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strDamage[iDamageIndex] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	//// ũ��Ƽ�� Ȯ��
	iDamage = CItemDB::ComputeWeaponCriticalRate(pSlotItem);
	strDamage = to_string(iDamage);
	iDamageDigit = (_uint)strDamage.size();

	_float3 vWeaponDMGColor;
	if (iDamage < (_uint)m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter)->iData[1])
		vWeaponDMGColor = _float3(0.8f, 0.2f, 0.2f);
	else if (iDamage == (_uint)m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter)->iData[1])
		vWeaponDMGColor = DEFAULT_COLOR;
	else
		vWeaponDMGColor = ADVANCED_COLOR;

	if (FAILED(m_pShader->SetRawValue("g_vColor", &vWeaponDMGColor, sizeof(_float3))))
		return E_FAIL;

	for (_uint iDamageIndex = 0; iDamageIndex < iDamageDigit; ++iDamageIndex)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponCri[iDamageIndex].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strDamage[iDamageIndex] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// �ۼ�Ʈ
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponCri[iDamageDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// Confirm ��ư
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponConfirmButton.WorldMatrix)))
		return E_FAIL;
	switch (m_iConfirmButtonState)
	{
	case BTN_NONE:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_NONE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case BTN_ON:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_ON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	case BTN_CLICK:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_CLICK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		break;
	}

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Confirm Text
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponConfirmText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_CONFIRM, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	return S_OK;
}

void CResonatorUI::keyInput(_double TimeDelta)
{
	switch (m_eCurButton)
	{
	case BTN_STATE:
		stateKeyInput(TimeDelta);
		break;
	case BTN_WEAPON:
		weaponKeyInput(TimeDelta);
		break;
	case BTN_ECHO:
		echoKeyInput(TimeDelta);
		break;

	default:
		break;
	}
}

HRESULT CResonatorUI::chooseCharacter()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Right Character Icons : 3
	for (_uint i = 0; i < 3; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 1], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharBack[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::GLOW_CIRCLE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		//���õ� �����ܻ��� ����
		if (m_iCurCharacter == i)
		{
			_float3 vColor = LEGEND_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;

			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}
		else
		{
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCharIcon[i].WorldMatrix)))
			return E_FAIL;

		_uint iTextureID = 0;
		switch (i)
		{
		case 0:
			iTextureID = STATIC_IMAGE::ICON_ROVER_GRAD;
			break;
		case 1:
			iTextureID = STATIC_IMAGE::ICON_YANGYANG_GRAD;
			break;
		case 2:
			iTextureID = STATIC_IMAGE::ICON_GUNNER_GRAD;
			break;
		}

		if (FAILED(pGameInstance->SetupSRV(iTextureID, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

CResonatorUI * CResonatorUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CResonatorUI*	pInstance = new CResonatorUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CResonatorUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CResonatorUI::Clone(void * pArg)
{
	CResonatorUI*	pInstance = new CResonatorUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CResonatorUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CResonatorUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}

_float3 CResonatorUI::GetEchoGradeColor3(_uint iGrade)
{
	_float3 vColor;
	switch (iGrade)
	{
	case 1:
		vColor = SLOT_ADVANCED_COLOR;
		break;
	case 2:
		vColor = SLOT_RARE_COLOR;
		break;
	case 3:
		vColor = SLOT_UNIQUE_COLOR;
		break;
	case 4:
		vColor = SLOT_LEGEND_COLOR;
		break;
	case 5:
		vColor = SLOT_RED_COLOR;
		break;
	}

	return vColor;
}
