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

#include "UICam.h"
#include "EchoSystem.h"
#include "Layer.h"

#include "Effect.h"

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
	ZeroMemory(iResonanceSocket, sizeof(iResonanceSocket));
	ZeroMemory(m_eSkillState, sizeof(m_eSkillState));

	for(_uint Char = 0 ; Char < 3 ; ++Char)
	for (_uint i = 0; i < SKILL_TYPE::NONE; ++i)
		m_eSkillState[Char][i] = SKILL_STATE::LOCK;
	
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
	m_OrthoTitleText.fX = 330.f;
	m_OrthoTitleText.fY = 120.f;
	CAppManager::ComputeOrtho(&m_OrthoTitleText);

#pragma endregion

#pragma region STATE_DETAILS
	// Character Icon
	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoCharBack[i].fWidth = 70.f;
		m_OrthoCharBack[i].fHeight = 70.f;
		m_OrthoCharBack[i].fX = g_iWinSizeX - 90.f;
		m_OrthoCharBack[i].fY = 70.f + (i * 80.f);
		CAppManager::ComputeOrtho(&m_OrthoCharBack[i]);

		m_OrthoCharIcon[i].fWidth = 60.f;
		m_OrthoCharIcon[i].fHeight = 60.f;
		m_OrthoCharIcon[i].fX = g_iWinSizeX - 90.f;
		m_OrthoCharIcon[i].fY = 70.f + (i * 80.f);
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

	// StateData 출력 최대 6자리
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

	// 강화 표식 0/5
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		m_OrthoUpgradeSign[i].fWidth = 40.f;
		m_OrthoUpgradeSign[i].fHeight = 40.f;
		m_OrthoUpgradeSign[i].fX = 180.f + (i * 35.f);
		m_OrthoUpgradeSign[i].fY = 260.f;
		CAppManager::ComputeOrtho(&m_OrthoUpgradeSign[i]);
	}

	// 공격력, 치명타 백그라운드
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

		m_OrthoWeaponSlotBack[i].fWidth = 100.f;
		m_OrthoWeaponSlotBack[i].fHeight = 20.f;
		m_OrthoWeaponSlotBack[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 100.f + ((i % 5) * 100.f);
		m_OrthoWeaponSlotBack[i].fY = 250 + ((i / 5) * 100.f) - 40.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponSlotBack[i]);

		m_OrthoWeaponSlotName[i].fWidth = 512.f * 0.33f;
		m_OrthoWeaponSlotName[i].fHeight = 64.f * 0.33f;
		m_OrthoWeaponSlotName[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 100.f + ((i % 5) * 100.f);
		m_OrthoWeaponSlotName[i].fY = 250 + ((i / 5) * 100.f) - 38.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponSlotName[i]);

		m_OrthoWeaponIcon[i].fWidth = 70.f;
		m_OrthoWeaponIcon[i].fHeight = 70.f;
		m_OrthoWeaponIcon[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 110.f + ((i % 5) * 100.f);
		m_OrthoWeaponIcon[i].fY = 260 + ((i / 5) * 100.f);
		CAppManager::ComputeOrtho(&m_OrthoWeaponIcon[i]);

		// 아이템 별 강화 횟수
		m_OrthoWeaponUpgradeCount[i].fWidth = 16.f * 0.7f;
		m_OrthoWeaponUpgradeCount[i].fHeight = 25.f * 0.7f;
		m_OrthoWeaponUpgradeCount[i].fX = m_OrthoWeaponSlot[i].fX + 25.f;
		m_OrthoWeaponUpgradeCount[i].fY = m_OrthoWeaponSlot[i].fY + 25.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradeCount[i]);

		// 아이템 별 강화 횟수 + 이미지
		m_OrthoWeaponUpgradePlus[i].fWidth = 16.f * 0.7f;
		m_OrthoWeaponUpgradePlus[i].fHeight = 25.f * 0.7f;
		m_OrthoWeaponUpgradePlus[i].fX = m_OrthoWeaponSlot[i].fX + 25.f - (16.f * 0.7f);
		m_OrthoWeaponUpgradePlus[i].fY = m_OrthoWeaponSlot[i].fY + 25.f;
		CAppManager::ComputeOrtho(&m_OrthoWeaponUpgradePlus[i]);
	}

	//Select Weapon Slot
	m_OrthoSelectWeaponSlot.fWidth = 220.f;
	m_OrthoSelectWeaponSlot.fHeight = 220.f;
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

	// Select Weapon 강화 횟수
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		m_OrthoSelectWeaponUpgrade[i].fWidth = 50.f;
		m_OrthoSelectWeaponUpgrade[i].fHeight = 50.f;
		m_OrthoSelectWeaponUpgrade[i].fX = m_OrthoSelectWeaponName.fX - 100.f + (i * 50.f);
		m_OrthoSelectWeaponUpgrade[i].fY = m_OrthoSelectWeaponName.fY + 40.f;
		CAppManager::ComputeOrtho(&m_OrthoSelectWeaponUpgrade[i]);
	}

	// Select Weapon 공격력 백그라운드
	m_OrthoSelectWeaponCriBack.fWidth = 512.f * 0.6f;
	m_OrthoSelectWeaponCriBack.fHeight = 64.f * 0.6f;
	m_OrthoSelectWeaponCriBack.fX = m_OrthoSelectWeaponName.fX;
	m_OrthoSelectWeaponCriBack.fY = m_OrthoSelectWeaponUpgrade[0].fY + (m_OrthoSelectWeaponUpgrade[0].fHeight * 0.5f) + (m_OrthoSelectWeaponCriBack.fHeight * 0.5f) + 10.f;
	CAppManager::ComputeOrtho(&m_OrthoSelectWeaponCriBack);

	// Select Weapon 치명타확률 백그라운드
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

	// 바인딩된 에코슬롯
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

	// 바인딩 에코 이름
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

	// 바인딩 에코 공격력
	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoEchoDamage[i].fWidth = 16.f * 0.7f;
		m_OrthoEchoDamage[i].fHeight = 25.f * 0.7f;
		m_OrthoEchoDamage[i].fX = 335.f + (i * (15.f * 0.7f));
		m_OrthoEchoDamage[i].fY = 222.f;
		CAppManager::ComputeOrtho(&m_OrthoEchoDamage[i]);
	}

	// 에코 슬롯
	for (_uint i = 0; i < ECHO_SLOT_MAX; ++i)
	{
		m_OrthoEchoSlot[i].fWidth = 74.f;
		m_OrthoEchoSlot[i].fHeight = 74.f;
		m_OrthoEchoSlot[i].fX = 200.f + ((i % 4) * 74.f);
		m_OrthoEchoSlot[i].fY = 300.f + ((i / 4) * 74.f);
		CAppManager::ComputeOrtho(&m_OrthoEchoSlot[i]);

		// 그레이드 3단계
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

	//에코 업그레이드 백그라운드
	m_OrthoEchoUpgradeBack.fWidth = 340.f;
	m_OrthoEchoUpgradeBack.fHeight = 120.f;
	m_OrthoEchoUpgradeBack.fX = g_iWinSizeX >> 2;
	m_OrthoEchoUpgradeBack.fY = g_iWinSizeY - 165.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoUpgradeBack);

	// 등록버튼
	m_OrthoRegisterButton.fWidth = 200.f;
	m_OrthoRegisterButton.fHeight = 90.f;
	m_OrthoRegisterButton.fX = (g_iWinSizeX >> 2) - 80.f;
	m_OrthoRegisterButton.fY = g_iWinSizeY - 75.f;
	CAppManager::ComputeOrtho(&m_OrthoRegisterButton);

	m_OrthoRegisterText.fWidth = 256.f * 0.6f;
	m_OrthoRegisterText.fHeight = 32.f * 0.6f;
	m_OrthoRegisterText.fX = (g_iWinSizeX >> 2) - 80.f;
	m_OrthoRegisterText.fY = g_iWinSizeY - 70.f;
	CAppManager::ComputeOrtho(&m_OrthoRegisterText);

	// 업그레이드 버튼
	m_OrthoEchoUpgradeButton.fWidth = 200.f;
	m_OrthoEchoUpgradeButton.fHeight = 90.f;
	m_OrthoEchoUpgradeButton.fX = (g_iWinSizeX >> 2) + 80.f;
	m_OrthoEchoUpgradeButton.fY = g_iWinSizeY - 75.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoUpgradeButton);

	m_OrthoEchoUpgradeButtonText.fWidth = 256.f * 0.8f;
	m_OrthoEchoUpgradeButtonText.fHeight = 32.f * 0.8f;
	m_OrthoEchoUpgradeButtonText.fX = (g_iWinSizeX >> 2) + 80.f;
	m_OrthoEchoUpgradeButtonText.fY = g_iWinSizeY - 70.f;
	CAppManager::ComputeOrtho(&m_OrthoEchoUpgradeButtonText);

	// 인식표 슬롯
	m_OrthoDogTagSlot.fWidth = 80.f;
	m_OrthoDogTagSlot.fHeight = 80.f;
	m_OrthoDogTagSlot.fX = m_OrthoEchoUpgradeBack.fX - 80.f;
	m_OrthoDogTagSlot.fY = m_OrthoEchoUpgradeBack.fY + 10.f;
	CAppManager::ComputeOrtho(&m_OrthoDogTagSlot);

	m_OrthoDogTagIcon.fWidth = 60.f;
	m_OrthoDogTagIcon.fHeight = 60.f;
	m_OrthoDogTagIcon.fX = m_OrthoEchoUpgradeBack.fX - 80.f;
	m_OrthoDogTagIcon.fY = m_OrthoEchoUpgradeBack.fY + 10.f;
	CAppManager::ComputeOrtho(&m_OrthoDogTagIcon);

	// 인식표 코스트 텍스트
	m_OrthoDogTagCostText.fWidth = 256.f * 0.8f;
	m_OrthoDogTagCostText.fHeight = 32.f * 0.8f;
	m_OrthoDogTagCostText.fX = m_OrthoDogTagSlot.fX  + 70;
	m_OrthoDogTagCostText.fY = m_OrthoEchoUpgradeBack.fY - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoDogTagCostText);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoDogTagCost[i].fWidth = 16.f * 0.7f;
		m_OrthoDogTagCost[i].fHeight = 25.f * 0.7f;
		m_OrthoDogTagCost[i].fX = m_OrthoDogTagSlot.fX + 50.f + (i * 14.f);
		m_OrthoDogTagCost[i].fY = m_OrthoDogTagSlot.fY;
		CAppManager::ComputeOrtho(&m_OrthoDogTagCost[i]);
	}

	// 인식표 보유 코스트 텍스트
	m_OrthoDogTagCostOwnText.fWidth = 256.f * 0.8f;
	m_OrthoDogTagCostOwnText.fHeight = 32.f * 0.8f;
	m_OrthoDogTagCostOwnText.fX = m_OrthoDogTagSlot.fX + 225;
	m_OrthoDogTagCostOwnText.fY = m_OrthoEchoUpgradeBack.fY - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoDogTagCostOwnText);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoDogTagCostOwn[i].fWidth = 16.f * 0.7f;
		m_OrthoDogTagCostOwn[i].fHeight = 25.f * 0.7f;
		m_OrthoDogTagCostOwn[i].fX = m_OrthoDogTagSlot.fX + 140.f + (i * 14.f);
		m_OrthoDogTagCostOwn[i].fY = m_OrthoDogTagSlot.fY;
		CAppManager::ComputeOrtho(&m_OrthoDogTagCostOwn[i]);
	}


#pragma endregion
	
#pragma region RESONANCE
	// Back 로고
	m_OrthoActivateBack.fWidth = 688.f * 0.5f;
	m_OrthoActivateBack.fHeight = 432.f * 0.5f;
	m_OrthoActivateBack.fX = 320.f;
	m_OrthoActivateBack.fY = g_iWinSizeY >> 2;
	CAppManager::ComputeOrtho(&m_OrthoActivateBack);

	// Activate Text
	m_OrthoActivateText.fWidth = 512.f * 0.85f;
	m_OrthoActivateText.fHeight = 64.f * 0.85f;
	m_OrthoActivateText.fX = 320.f;
	m_OrthoActivateText.fY = (g_iWinSizeY >> 2) + 64.f;
	CAppManager::ComputeOrtho(&m_OrthoActivateText);

	for (_uint i = 0; i < 2; ++i)
	{
		m_OrthoActivateCount[i].fWidth = 16.f * 0.8f;
		m_OrthoActivateCount[i].fHeight = 25.f * 0.8f;
		m_OrthoActivateCount[i].fX = 300.f + ( i * (15.f * 0.8f));
		m_OrthoActivateCount[i].fY = m_OrthoActivateText.fY + 20.f;
		CAppManager::ComputeOrtho(&m_OrthoActivateCount[i]);
	}

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoActivateLimit[i].fWidth = 16.f * 0.8f;
		m_OrthoActivateLimit[i].fHeight = 25.f * 0.8f;
		m_OrthoActivateLimit[i].fX = 300.f + ((16.f * 0.8f) * 2.F) + (i * (15.f * 0.8f));
		m_OrthoActivateLimit[i].fY = m_OrthoActivateText.fY + 20.f;
		CAppManager::ComputeOrtho(&m_OrthoActivateLimit[i]);
	}

	// 시퀀스 슬롯 (1 ~ 4)

	//1
	m_OrthoSequenceSlot1[0].fWidth = 100.f;
	m_OrthoSequenceSlot1[0].fHeight = 100.f;
	m_OrthoSequenceSlot1[0].fX = 320.f;
	m_OrthoSequenceSlot1[0].fY = (g_iWinSizeY >> 1) + 40.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot1[0]);

	m_OrthoSequenceSlot1[1].fWidth = 80.f;
	m_OrthoSequenceSlot1[1].fHeight = 80.f;
	m_OrthoSequenceSlot1[1].fX = 320.f;
	m_OrthoSequenceSlot1[1].fY = (g_iWinSizeY >> 1) + 40.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot1[1]);

	m_OrthoSequenceText[0].fWidth = 512.f * 0.6f;
	m_OrthoSequenceText[0].fHeight = 64.f * 0.6f;
	m_OrthoSequenceText[0].fX = 400.f;
	m_OrthoSequenceText[0].fY = (g_iWinSizeY >> 1) + 60.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceText[0]);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoSequenceSlotSocket1[i].fWidth = 30.f;
		m_OrthoSequenceSlotSocket1[i].fHeight = 30.f;
		m_OrthoSequenceSlotSocket1[i].fX = 290.f + (i * 30.f);
		m_OrthoSequenceSlotSocket1[i].fY = m_OrthoSequenceSlot1[0].fY + 50.f;
		CAppManager::ComputeOrtho(&m_OrthoSequenceSlotSocket1[i]);
	}

	//2
	m_OrthoSequenceSlot2[0].fWidth = 100.f;
	m_OrthoSequenceSlot2[0].fHeight = 100.f;
	m_OrthoSequenceSlot2[0].fX = 450.f;
	m_OrthoSequenceSlot2[0].fY = (g_iWinSizeY >> 1) + 170.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot2[0]);
	m_OrthoSequenceSlot2[1].fWidth = 80.f;
	m_OrthoSequenceSlot2[1].fHeight = 80.f;
	m_OrthoSequenceSlot2[1].fX = 450.f;
	m_OrthoSequenceSlot2[1].fY = (g_iWinSizeY >> 1) + 170.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot2[1]);

	m_OrthoSequenceText[1].fWidth = 512.f * 0.6f;
	m_OrthoSequenceText[1].fHeight = 64.f * 0.6f;
	m_OrthoSequenceText[1].fX = 530.f;
	m_OrthoSequenceText[1].fY = (g_iWinSizeY >> 1) + 190.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceText[1]);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoSequenceSlotSocket2[i].fWidth = 30.f;
		m_OrthoSequenceSlotSocket2[i].fHeight = 30.f;
		m_OrthoSequenceSlotSocket2[i].fX = 420.f + (i * 30.f);
		m_OrthoSequenceSlotSocket2[i].fY = m_OrthoSequenceSlot2[0].fY + 50.f;
		CAppManager::ComputeOrtho(&m_OrthoSequenceSlotSocket2[i]);
	}

	//3
	m_OrthoSequenceSlot3[0].fWidth = 100.f;
	m_OrthoSequenceSlot3[0].fHeight = 100.f;
	m_OrthoSequenceSlot3[0].fX = 870.f;
	m_OrthoSequenceSlot3[0].fY = (g_iWinSizeY >> 1) - 170.F;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot3[0]);

	m_OrthoSequenceSlot3[1].fWidth = 80.f;
	m_OrthoSequenceSlot3[1].fHeight = 80.f;
	m_OrthoSequenceSlot3[1].fX = 870.f;
	m_OrthoSequenceSlot3[1].fY = (g_iWinSizeY >> 1) - 170.F;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot3[1]);

	m_OrthoSequenceText[2].fWidth = 512.f * 0.6f;
	m_OrthoSequenceText[2].fHeight = 64.f * 0.6f;
	m_OrthoSequenceText[2].fX = 950.f;
	m_OrthoSequenceText[2].fY = (g_iWinSizeY >> 1) - 150.;
	CAppManager::ComputeOrtho(&m_OrthoSequenceText[2]);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoSequenceSlotSocket3[i].fWidth = 30.f;
		m_OrthoSequenceSlotSocket3[i].fHeight = 30.f;
		m_OrthoSequenceSlotSocket3[i].fX = 840.f + (i * 30.f);
		m_OrthoSequenceSlotSocket3[i].fY = m_OrthoSequenceSlot3[0].fY + 50.f;
		CAppManager::ComputeOrtho(&m_OrthoSequenceSlotSocket3[i]);
	}

	//4
	m_OrthoSequenceSlot4[0].fWidth = 100.f;
	m_OrthoSequenceSlot4[0].fHeight = 100.f;
	m_OrthoSequenceSlot4[0].fX = 1000.f;
	m_OrthoSequenceSlot4[0].fY = (g_iWinSizeY >> 1) - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot4[0]);

	m_OrthoSequenceSlot4[1].fWidth = 80.f;
	m_OrthoSequenceSlot4[1].fHeight = 80.f;
	m_OrthoSequenceSlot4[1].fX = 1000.f;
	m_OrthoSequenceSlot4[1].fY = (g_iWinSizeY >> 1) - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceSlot4[1]);

	m_OrthoSequenceText[3].fWidth = 512.f * 0.6f;
	m_OrthoSequenceText[3].fHeight = 64.f * 0.6f;
	m_OrthoSequenceText[3].fX = 1080.f;
	m_OrthoSequenceText[3].fY = (g_iWinSizeY >> 1) - 20.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceText[3]);

	for (_uint i = 0; i < 3; ++i)
	{
		m_OrthoSequenceSlotSocket4[i].fWidth = 30.f;
		m_OrthoSequenceSlotSocket4[i].fHeight = 30.f;
		m_OrthoSequenceSlotSocket4[i].fX = 970.f + (i * 30.f);
		m_OrthoSequenceSlotSocket4[i].fY = m_OrthoSequenceSlot4[0].fY + 50.f;
		CAppManager::ComputeOrtho(&m_OrthoSequenceSlotSocket4[i]);
	}

	m_OrthoSequenceUpgradeBack.fWidth = 400.f;
	m_OrthoSequenceUpgradeBack.fHeight = 200.f;
	m_OrthoSequenceUpgradeBack.fX = g_iWinSizeX - 210.f;
	m_OrthoSequenceUpgradeBack.fY = g_iWinSizeY  - 110.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceUpgradeBack);

	m_OrthoSequenceCurSequenceText.fWidth = 512.f * 0.7f;
	m_OrthoSequenceCurSequenceText.fHeight = 128.f * 0.7f;
	m_OrthoSequenceCurSequenceText.fX = m_OrthoSequenceUpgradeBack.fX + 30.f;
	m_OrthoSequenceCurSequenceText.fY = m_OrthoSequenceUpgradeBack.fY - 35.f;
	CAppManager::ComputeOrtho(&m_OrthoSequenceCurSequenceText);

	m_OrthoSQCostSlot.fWidth = 70.f;
	m_OrthoSQCostSlot.fHeight = 70.f;
	m_OrthoSQCostSlot.fX = g_iWinSizeX - 210.f - 100.f;
	m_OrthoSQCostSlot.fY = g_iWinSizeY - 80.f;
	CAppManager::ComputeOrtho(&m_OrthoSQCostSlot);

	for (_uint i = 0; i < 2; ++i)
	{
		m_OrthoSQCost[i].fWidth = 16.f * 0.8f;
		m_OrthoSQCost[i].fHeight = 25.f * 0.8f;
		m_OrthoSQCost[i].fX = (g_iWinSizeX - 210.f - 120.f + 60.f) + (i * (14.f * 0.8f));
		m_OrthoSQCost[i].fY = g_iWinSizeY - 80.f;
		CAppManager::ComputeOrtho(&m_OrthoSQCost[i]);
	}

	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoSQOwn[i].fWidth = 16.f * 0.8f;
		m_OrthoSQOwn[i].fHeight = 25.f * 0.8f;
		m_OrthoSQOwn[i].fX = (g_iWinSizeX - 210.f - 120.f + 180.f) + (i * (14.f * 0.8f));
		m_OrthoSQOwn[i].fY = g_iWinSizeY - 80.f;
		CAppManager::ComputeOrtho(&m_OrthoSQOwn[i]);
	}

	m_OrthoSQCostIcon.fWidth = 40.f;
	m_OrthoSQCostIcon.fHeight = 40.f;
	m_OrthoSQCostIcon.fX = g_iWinSizeX - 210.f - 100.f;
	m_OrthoSQCostIcon.fY = g_iWinSizeY - 80.f;
	CAppManager::ComputeOrtho(&m_OrthoSQCostIcon);

	m_OrthoSQUpgradeButton.fWidth = 150.f;
	m_OrthoSQUpgradeButton.fHeight = 70.f;
	m_OrthoSQUpgradeButton.fX = g_iWinSizeX - 200.f;
	m_OrthoSQUpgradeButton.fY = g_iWinSizeY - 40.f;
	CAppManager::ComputeOrtho(&m_OrthoSQUpgradeButton);

	m_OrthoSQUpgradeButtonText.fWidth = 512.f * 0.5f;
	m_OrthoSQUpgradeButtonText.fHeight = 64.f * 0.5f;
	m_OrthoSQUpgradeButtonText.fX = g_iWinSizeX - 200.f;
	m_OrthoSQUpgradeButtonText.fY = g_iWinSizeY - 37.f;
	CAppManager::ComputeOrtho(&m_OrthoSQUpgradeButtonText);

#pragma endregion

#pragma region FORTE
	// Back 로고
	m_OrthoForteBack.fWidth = 2560.f * 0.5f;
	m_OrthoForteBack.fHeight = 1280.f * 0.5f;
	m_OrthoForteBack.fX = (g_iWinSizeX >> 2) + 125.f;
	m_OrthoForteBack.fY = (g_iWinSizeY >> 1) + 50.f;
	CAppManager::ComputeOrtho(&m_OrthoForteBack);

	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoAttackSlot[i].fWidth = 100.f;
		m_OrthoAttackSlot[i].fHeight = 100.f;
		m_OrthoAttackSlot[i].fX = m_OrthoForteBack.fX;
		m_OrthoAttackSlot[i].fY = g_iWinSizeY - 150.f - ( i * 115.f);
		CAppManager::ComputeOrtho(&m_OrthoAttackSlot[i]);

	}

	m_OrthoQTESlot.fWidth = 100.f;
	m_OrthoQTESlot.fHeight = 100.f;
	m_OrthoQTESlot.fX = m_OrthoForteBack.fX - 110.f;
	m_OrthoQTESlot.fY = m_OrthoAttackSlot[1].fY + 55.f;
	CAppManager::ComputeOrtho(&m_OrthoQTESlot);

	m_OrthoBurstSlot.fWidth = 100.f;
	m_OrthoBurstSlot.fHeight = 100.f;
	m_OrthoBurstSlot.fX = m_OrthoForteBack.fX + 110.f;
	m_OrthoBurstSlot.fY = m_OrthoAttackSlot[2].fY - 55.f;
	CAppManager::ComputeOrtho(&m_OrthoBurstSlot);

	//ICon
	for (_uint i = 0; i < 4; ++i)
	{
		m_OrthoAttackIcon[i].fWidth = 70.f;
		m_OrthoAttackIcon[i].fHeight = 70.f;
		m_OrthoAttackIcon[i].fX = m_OrthoForteBack.fX;
		m_OrthoAttackIcon[i].fY = g_iWinSizeY - 150.f - (i * 115.f);
		CAppManager::ComputeOrtho(&m_OrthoAttackIcon[i]);

		m_OrthoAttackNeedLevel[i].fWidth = 512.f * 0.4f;
		m_OrthoAttackNeedLevel[i].fHeight = 64.f * 0.4f;
		m_OrthoAttackNeedLevel[i].fX = m_OrthoForteBack.fX;
		m_OrthoAttackNeedLevel[i].fY = g_iWinSizeY - 100.f - (i * 115.f);
		CAppManager::ComputeOrtho(&m_OrthoAttackNeedLevel[i]);

	}

	m_OrthoQTEIcon.fWidth = 70.f;
	m_OrthoQTEIcon.fHeight = 70.f;
	m_OrthoQTEIcon.fX = m_OrthoForteBack.fX - 110.f;
	m_OrthoQTEIcon.fY = m_OrthoAttackSlot[1].fY + 55.f;
	CAppManager::ComputeOrtho(&m_OrthoQTEIcon);

	m_OrthoBurstIcon.fWidth = 70.f;
	m_OrthoBurstIcon.fHeight = 70.f;
	m_OrthoBurstIcon.fX = m_OrthoForteBack.fX + 110.f;
	m_OrthoBurstIcon.fY = m_OrthoAttackSlot[2].fY - 55.f;
	CAppManager::ComputeOrtho(&m_OrthoBurstIcon);

	m_OrthoSkillPointText.fWidth = 512.f * 0.6f;
	m_OrthoSkillPointText.fHeight = 64.f * 0.6f;
	m_OrthoSkillPointText.fX = m_OrthoForteBack.fX - 16.f;
	m_OrthoSkillPointText.fY = g_iWinSizeY - 72.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillPointText);

	for (_uint i = 0; i < 2; ++i)
	{
		m_OrthoSkillPoint[i].fWidth = 16.f * 0.8f;
		m_OrthoSkillPoint[i].fHeight = 25.f * 0.8f;
		m_OrthoSkillPoint[i].fX = m_OrthoForteBack.fX + 52 + (i * (14.f * 0.8f));
		m_OrthoSkillPoint[i].fY = g_iWinSizeY - 70.f;
		CAppManager::ComputeOrtho(&m_OrthoSkillPoint[i]);
	}

	m_OrthoSkillDesc.fWidth = 512.f * 0.7f;
	m_OrthoSkillDesc.fHeight = 128.f * 0.7f;
	m_OrthoSkillDesc.fX = 330.f;
	m_OrthoSkillDesc.fY = (g_iWinSizeY >> 2) + 30.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillDesc);

	m_OrthoSkillState.fWidth = 512.f * 0.5f;
	m_OrthoSkillState.fHeight = 64.f * 0.5f;
	m_OrthoSkillState.fX = 400.f;
	m_OrthoSkillState.fY = (g_iWinSizeY >> 2) - 5.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillState);
	
	m_OrthoSkillNeedDesc.fWidth = 512.f * 0.5f;
	m_OrthoSkillNeedDesc.fHeight = 64.f * 0.5f;
	m_OrthoSkillNeedDesc.fX = 278.f;
	m_OrthoSkillNeedDesc.fY = (g_iWinSizeY >> 2) + 70.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillNeedDesc);

	m_OrthoSkillUpgradeBtn.fWidth = 170.f;
	m_OrthoSkillUpgradeBtn.fHeight = 70.f;
	m_OrthoSkillUpgradeBtn.fX = m_OrthoForteBack.fX;
	m_OrthoSkillUpgradeBtn.fY = g_iWinSizeY - 35.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillUpgradeBtn);

	m_OrthoSkillUpgradeText.fWidth = 512.f * 0.6f;
	m_OrthoSkillUpgradeText.fHeight = 64.f * 0.6f;
	m_OrthoSkillUpgradeText.fX = m_OrthoForteBack.fX;
	m_OrthoSkillUpgradeText.fY = g_iWinSizeY - 30.f;
	CAppManager::ComputeOrtho(&m_OrthoSkillUpgradeText);

#pragma endregion

	// 기타 임시 디폴트 세팅
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
	itemDesc.iAmount = 20;
	m_pInven->PushItemDesc(itemDesc);
	
	itemDesc = pDB->GetItemData(ITEM::SWORD4);
	itemDesc.iAmount = 5;
	m_pInven->PushItemDesc(itemDesc);

	itemDesc = pDB->GetItemData(ITEM::GUN3);
	itemDesc.iAmount = 5;
	m_pInven->PushItemDesc(itemDesc);

	_matrix IdenMatrix = XMMatrixIdentity();
	
	XMStoreFloat4x4(&m_WorldMatrix, IdenMatrix);
	m_WorldMatrix._41 = -999.f;
	m_WorldMatrix._42 = 0.f;
	m_WorldMatrix._43 = -1000.f;

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

	m_fParticleTime += (_float)TimeDelta;
	if (m_fParticleTime > 2.f)
	{
		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"UI_Particle_Effect", EFFECT_ID::COMON);
		if (nullptr != pEffect)
		{
			pEffect->Play_Effect(&m_WorldMatrix, false);
		}
		m_fParticleTime = 0.f;
	}

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
		if (FAILED(resonanceRender()))
			return E_FAIL;

		break;
	case CResonatorUI::BTN_WUTERIDE:
		if (FAILED(forteRender()))
			return E_FAIL;

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
	CCamera* pCam = m_pCamMovement->GetCamera(CCameraMovement::CAM_UI);

	if (m_eCurButton == BTN_RESONANCE)
	{
		m_bElemAlphaStart[0] = true;
		m_fElemAlpha[0] = 1.f;

		if (static_cast<CUICam*>(pCam)->IsMoveFinish())
		{
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
		}
	}
	else
	{
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
	//초기상태 세팅
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

	//우상단 캐릭터 선택
	chooseCharacter();

	// 경험치 캡슐슬롯
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

		//경험치 캡슐 수량
		_uint iAmountCapsule = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::EXP0 + iGrade);

		//수량이 0개일때
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
				//OnMouse일떄 클릭할떄 동작하도록 수정할것
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

		//캡술 사용할 수량 4자리 수까지 표기
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

	// 슬래쉬
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

	// 연산된 최종 스텟 추후에 현재 선택된 캐릭터 파라미터로 던져서 스텟 보여주기
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

	// 퍼센트 붙히기
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
				{
					elemAlphaReset(1);
					m_pUICharacter->ChangeCharacter((UICharacter::MODEL)i);
				}

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

		// 경험치 증가 임시
		if (pGameInstance->InputKey(DIK_B) == KEY_STATE::TAP)
			m_pPlayerState->AddExp((CPlayerState::CHARACTERS)m_iCurCharacter, 10.f);

		//사용할 경험치 캡슐 추가/감소
		for (_uint iCapsule = 0; iCapsule < 4; ++iCapsule)
		{
			_uint iItemAmount = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::EXP0 + iCapsule);
			if (pGM->OnMouse(m_OrthoUpgradeSlot[iCapsule]))
			{
				if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					if (iItemAmount > m_iUseAmount[iCapsule])
					{
						//재클릭시 리셋
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
						//재클릭시 리셋
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

	// Tooltip Alpha 세팅
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

	//에코 해제
	const CEchoSystem::ECHO* pBindEcho = m_pEchoSystem->GetRegisteredEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter);
	if (pBindEcho)
	{
		if (pGameMode->OnMouse(m_OrthoBindEcho))
		{
			if (pGameInstnace->InputMouse(DIMK_RB) == KEY_STATE::TAP)
				m_pEchoSystem->ReleaseEcho((CEchoSystem::ECHO_SLOT)m_iCurCharacter);
		}
	}

	//에코 등록 버튼
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

	//에코 업그레이드 버튼
	_uint iEchoGrade = m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)m_iCurSelectEcho).iGrade;
	_uint iEchoLevel = m_pEchoSystem->GetEchoLevel((CEchoSystem::ECHO_TYPE)m_iCurSelectEcho);

	_uint iOwnCost;
	_uint iConsumeCost;

	switch (iEchoGrade)
	{
	case 1:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG0);
		break;
	case 2:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG1);
		break;
	case 3:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG2);
		break;
	case 4:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG3);
		break;
	case 5:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG3);
		break;
	}

	if (iEchoLevel < 2)
		iConsumeCost = 10;
	else
		iConsumeCost = 2;

	if (iEchoLevel < 3 && iOwnCost >= iConsumeCost)
	{
		m_bUpgradeLimit = false;
	}
	else
	{
		m_iUpgradeBtnState = 2;
		m_bUpgradeLimit = true;
	}

	if (pGameMode->OnMouse(m_OrthoEchoUpgradeButtonText))
	{
		if (!m_bUpgradeLimit)
		{
			m_iUpgradeBtnState = 1;
			if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
				m_iUpgradeBtnState = 2;

			if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
			{
				// 업그레이드 처리
				m_pEchoSystem->Levelup((CEchoSystem::ECHO_TYPE)m_iCurSelectEcho);
				
				switch (iEchoGrade)
				{
				case 1:
					m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::DOGTAG0, 10);
					break;
				case 2:
					m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::DOGTAG1, 2);
					break;
				case 3:
					m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::DOGTAG2, 2);
					break;
				case 4:
					m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::DOGTAG3, 2);
					break;
				case 5:
					m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::DOGTAG3, 2);
					break;
				}
			}
		}

	}
	else
	{
		if (!m_bUpgradeLimit)
			m_iUpgradeBtnState = 0;
		else
			m_iUpgradeBtnState = 2;
	}


	switch (m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)m_iCurSelectEcho).iGrade)
	{
	case 1:
		iDogTagIcon = STATIC_IMAGE::ICON_DOGTAG0;
		break;
	case 2:
		iDogTagIcon = STATIC_IMAGE::ICON_DOGTAG1;
		break;
	case 3:
		iDogTagIcon = STATIC_IMAGE::ICON_DOGTAG2;
		break;
	case 4:
		FALL_THROUGH;
	case 5:
		iDogTagIcon = STATIC_IMAGE::ICON_DOGTAG3;
		break;
	}

}

void CResonatorUI::resonanceInput(_double TimeDelta)
{
	selectCharacter(TimeDelta);
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnance = CGameInstance::GetInstance();
	
	// SQ 선택
	if (pGameMode->OnMouse(m_OrthoSequenceSlot1[1]))
	{
		if(pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iSelectSQ = SQ_1;
	}
	else if (pGameMode->OnMouse(m_OrthoSequenceSlot2[1]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iSelectSQ = SQ_2;
	}
	else if (pGameMode->OnMouse(m_OrthoSequenceSlot3[1]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iSelectSQ = SQ_3;
	}
	else if (pGameMode->OnMouse(m_OrthoSequenceSlot4[1]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iSelectSQ = SQ_4;
	}
	else
	{
		if (!pGameMode->OnMouse(m_OrthoSequenceUpgradeBack))
		{
			if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				m_iSelectSQ = SQ_NONE;
		}
	}

	if (pGameMode->OnMouse(m_OrthoSQUpgradeButtonText))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			m_bSQUpgradeClick = true;
		else
		{
			m_bSQUpgradeClick = false;
		}

		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
		{
			_uint iAmount = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::SEQUENCE_GEM);
			if (iAmount > 0 && iResonanceSocket[m_iSelectSQ] < 3)
			{
				iResonanceSocket[m_iSelectSQ]++;
				m_pInven->EraseItem(CInventory::INVEN_MATERIAL, ITEM::SEQUENCE_GEM, 1);
				m_pPlayerState->SetSequenceLevel((CPlayerState::SQ_TYPE)m_iSelectSQ, iResonanceSocket[m_iSelectSQ]);
			}
		}

	}
	else
	{
		m_bSQUpgradeClick = false;
	}

}

void CResonatorUI::forteInput(_double TimeDelta)
{
	selectCharacter(TimeDelta);
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstnance = CGameInstance::GetInstance();

	if (pGameMode->OnMouse(m_OrthoAttackIcon[0]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = ATTACK1;
	}
	else if (pGameMode->OnMouse(m_OrthoAttackIcon[1]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = ATTACK2;
	}
	else if (pGameMode->OnMouse(m_OrthoAttackIcon[2]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = ATTACK3;
	}
	else if (pGameMode->OnMouse(m_OrthoAttackIcon[3]))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = ATTACK4;
	}
	else if (pGameMode->OnMouse(m_OrthoQTESlot))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = QTE;
	}
	else if (pGameMode->OnMouse(m_OrthoBurstSlot))
	{
		if (pGameInstnance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_iCurSelectSkill = BURST;
	}

	upgradeSkill(TimeDelta);
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
		// Select Weapon 페이지 닫기
		if (m_bSwitchButton)
		{
			if (pGameMode->OnMouse(m_OrthoSwitchBackpage))
			{
				if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					m_bSwitchButton = false;
			}

			//슬롯 체크
			for (_uint i = 0; i < 20; ++i)
			{
				if (pGameMode->OnMouse(m_OrthoWeaponSlot[i]))
				{
					if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::TAP)
						m_iSelectSlot = i;
				}
			}

			// WeaponChange Confirm
			if (pGameMode->OnMouse(m_OrthoWeaponConfirmText))
			{
				CItem::ITEM_DESC pSlotItem = m_pInven->GetSlotData(CInventory::INVEN_WEAPON, m_iSelectSlot);
				if ((pSlotItem.iData[3] == 1 && m_iCurCharacter == CPlayerState::CHARACTER_CHIXIA)
					|| (pSlotItem.iData[3] == 0 && m_iCurCharacter == CPlayerState::CHARACTER_ROVER)
					|| (pSlotItem.iData[3] == 0 && m_iCurCharacter == CPlayerState::CHARACTER_YANGYANG))
				{
					if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
						m_iConfirmButtonState = BTN_CLICK;
					else
					{
						if (pGameInstnace->InputMouse(DIMK_LB) == KEY_STATE::AWAY && m_iConfirmButtonState == BTN_CLICK)
						{
							m_pUICharacter->ChangeCharacter((UICharacter::MODEL)m_iCurCharacter); // 모션 다시 돌리기
							m_bSwitchButton = false;
							m_bWeaponChangeConfirm = true;
							m_iConfirmButtonState = BTN_ON;
							m_pInven->SwapWeapon(m_iCurCharacter, m_iSelectSlot);
						}
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
	// 업그레이드 버튼 꾹
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
	// 스위치 버튼 꾹
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

	_int iPrevLevel = m_pPlayerState->Get_CharState_byChar(iCharacterType)->iCurLevel;

	m_pPlayerState->AddExp((CPlayerState::CHARACTERS)iCharacterType, m_fTotalExp);
	_int iCurLevel = m_pPlayerState->Get_CharState_byChar(iCharacterType)->iCurLevel;

	if (iPrevLevel != iCurLevel)
	{
		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"P_Level_Up_Effect",EFFECT_ID::COMON);
		if (nullptr != pEffect)
		{
			pEffect->Play_Effect(&m_WorldMatrix , false);
		}
		CGameInstance::GetInstance()->PlaySoundEx(L"Play_AE_UI_FB_LevelUp.wem.wav",
			SOUND_UI_TEMP, 1.f);
	}

}

HRESULT CResonatorUI::weaponStateRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	//우상단 캐릭터 선택
	chooseCharacter();

	//무기 이름
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponName.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(getWeaponText(), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//무기 탭 설명
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

	// 무기 강화 표식
	CItem::tagItemDesc* pItem = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)m_iCurCharacter);
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeSign[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_UPGRADE_SIGN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		//무기의 강화 횟수
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

	// 공격력 백그라운드
	_float fAlpha = m_fElemAlpha[4] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDamageBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	// 치명타 데미지 백그라운드
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

	// 데미지
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

	// 크리티컬 확률
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

	// 퍼센트
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCriRate[iDamageDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 코스트 백그라운드
	fAlpha = m_fElemAlpha[6] * 0.3f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUpgradeCostBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 코스트 텍스트
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

	// 보유 잼 텍스트
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

	// Gem 슬롯
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
	// 코스트 (보유량)
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

	// 코스트 (필요)
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

	// 업그레이드 버튼
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

	// 업그레이드 텍스트
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponUpgradeText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 스위치 버튼
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

	// 스위치 텍스트
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

	//우상단 캐릭터 선택
	chooseCharacter();

	// 바인딩된 에코
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

	// 바인딩에코 이름
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

	// 바인딩에코 공격력
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

		// 바인딩 공격력 수치
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

	//에코 슬롯
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
			//에코 아이콘
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
			// 에코 회색 아이콘
			vColor4 = { m_fElemAlpha[3], m_fElemAlpha[3] , m_fElemAlpha[3] , m_fElemAlpha[3] };
			if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(EchoData.iIconTextureID, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoIcon[i].WorldMatrix)))
				return E_FAIL;

			m_pShader->Begin(12);
			m_pVIBuffer->Render();

			// 임시 가리기 (UnkownSlot)
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

	// 툴팁
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

		//에코 정보
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

		// 툴팁 데미지
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

	// 선택 Glow
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoSlot[m_iCurSelectEcho].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 에코 업그레이드 백그라운드
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoUpgradeBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(9);
	m_pVIBuffer->Render();

	// 인식표 코스트
	vColor = { 0.6f, 0.6f, 0.6f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCostText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADECOST, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// 에코 인식표
	_uint iCurEchoGrade = m_pEchoSystem->GetEcho((CEchoSystem::ECHO_TYPE)m_iCurSelectEcho).iGrade;
	vColor = GetEchoGradeColor3(iCurEchoGrade);
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// 에코 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(iDogTagIcon, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 보유 독텍
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCostOwnText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_OWN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	_uint iOwnCost = 0;
	switch (iDogTagIcon)
	{
	case STATIC_IMAGE::ICON_DOGTAG0:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG0);
		break;
	case STATIC_IMAGE::ICON_DOGTAG1:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG1);
		break;
	case STATIC_IMAGE::ICON_DOGTAG2:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG2);
		break;
	case STATIC_IMAGE::ICON_DOGTAG3:
		iOwnCost = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::DOGTAG3);
		break;
	}
	
	string strCost = to_string(iOwnCost);
	_uint iCostDigit = (_uint)strCost.size();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCostOwn[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	for (_uint i = 0; i < iCostDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCostOwn[i + 1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCost[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// 1렙만 코스트 10개
	if (iCurEchoGrade < 2)
	{
		string strCost = to_string(10);
		_uint iCostDigit = (_uint)strCost.size();

		_bool bValue;

		if (iOwnCost >= 10)
			bValue = true;
		else
			bValue = false;

		_float3 vColor = DEFAULT_COLOR;
		if (!bValue)
			vColor = { 0.8f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCost[0].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		for (_uint i = 0; i < iCostDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCost[i + 1].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strCost[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}
	}
	else
	{
		string strCost = to_string(2);
		_uint iCostDigit = (_uint)strCost.size();

		_bool bValue;

		if (iOwnCost >= 2)
			bValue = true;
		else
			bValue = false;

		_float3 vColor = DEFAULT_COLOR;
		if (!bValue)
			vColor = { 0.8f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCost[0].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		for (_uint i = 0; i < iCostDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[6], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDogTagCost[i + 1].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strCost[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}
	}

	// 등록 버튼
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
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

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoRegisterText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_REGISTER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 업그레이드 버튼
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoUpgradeButton.WorldMatrix)))
		return E_FAIL;

	switch (m_iUpgradeBtnState)
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

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoEchoUpgradeButtonText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CResonatorUI::resonanceRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	chooseCharacter();

	_uint iTotalActivate = iResonanceSocket[0] + iResonanceSocket[1] + iResonanceSocket[2] + iResonanceSocket[3];

	_float3 vColor;
	_float4 vColor4;

	_float fAlpha = m_fElemAlpha[1] * 0.5f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	vColor = SLOT_LEGEND_COLOR;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ACTIVATE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	string strActivateCount = to_string(iTotalActivate);
	_uint iDigit = (_uint)strActivateCount.size();

	for (_uint i = 0; i < iDigit; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;

		if (iDigit < 2)
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateCount[1].WorldMatrix)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateCount[i].WorldMatrix)))
				return E_FAIL;
		}


		if (FAILED(pGameInstance->SetupSRV(strActivateCount[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	fAlpha = m_fElemAlpha[19] * 2.f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateLimit[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SLASH, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateLimit[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoActivateLimit[2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_2, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();


	// 시퀀스 슬롯 1
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot1[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot1[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceText[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SEQUENCE1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 3; ++i)
	{
		fAlpha = m_fElemAlpha[6] * 2.f;

		if (i < iResonanceSocket[0])
			vColor = { 1.f, 1.f, 1.f };
		else
			vColor = { 0.2f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlotSocket1[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_SOCKET, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}


	// 시퀀스 슬롯 2
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot2[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot2[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE2, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceText[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SEQUENCE2, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 3; ++i)
	{
		fAlpha = m_fElemAlpha[5] * 2.f;

		if (i < iResonanceSocket[1])
			vColor = { 1.f, 1.f, 1.f };
		else
			vColor = { 0.2f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlotSocket2[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_SOCKET, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// 시퀀스 슬롯 3
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot3[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot3[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE3, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceText[2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SEQUENCE3, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 3; ++i)
	{
		fAlpha = m_fElemAlpha[5] * 2.f;
		if (i < iResonanceSocket[2])
			vColor = { 1.f, 1.f, 1.f };
		else
			vColor = { 0.2f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlotSocket3[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_SOCKET, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// 시퀀스 슬롯 4
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot4[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[4], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot4[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE4, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceText[3].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SEQUENCE4, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	for (_uint i = 0; i < 3; ++i)
	{
		fAlpha = m_fElemAlpha[6] * 2.f;
		if (i < iResonanceSocket[3])
			vColor = { 1.f, 1.f, 1.f };
		else
			vColor = { 0.2f, 0.2f, 0.2f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlotSocket4[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_SOCKET, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	if (m_iSelectSQ != SQ_NONE)
	{
		fAlpha = m_fElemAlpha[7] * 0.5f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceUpgradeBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();


		vColor = UNIQUE_COLOR;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQCostSlot.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQCostIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_SEQUENCE_GEM, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		//비용
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQCost[0].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQCost[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_1, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		//보유
		_uint iAmount = m_pInven->GetTotalAmount(CInventory::INVEN_MATERIAL, ITEM::SEQUENCE_GEM);
		string strOwnGemAmount = to_string(iAmount);
		_uint iDigit = (_uint)strOwnGemAmount.size();


		if (iAmount < 1)
			vColor = { 0.8f, 0.2f, 0.2f };
		else
			vColor = DEFAULT_COLOR;

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQOwn[0].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		for (_uint i = 0; i < iDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQOwn[i + 1].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strOwnGemAmount[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}

		if (m_bSQUpgradeClick)
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_CLICK, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_NONE, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQUpgradeButton.WorldMatrix)))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSQUpgradeButtonText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_UPGRADE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// SQ
	switch (m_iSelectSQ)
	{
	case SQ_1:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot1[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceCurSequenceText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SQTEXT1, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		break;
	case SQ_2:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot2[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceCurSequenceText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SQTEXT2, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		break;
	case SQ_3:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot3[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceCurSequenceText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SQTEXT3, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		break;
	case SQ_4:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceSlot4[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[7], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSequenceCurSequenceText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SQTEXT4, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		break;
	case SQ_NONE:
		break;
	}


	return S_OK;
}

HRESULT CResonatorUI::forteRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	chooseCharacter();
	
	_float3 vColor;
	_float4 vColor4;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoForteBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_ATTACKDESC_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//기공 4타
	for (_uint i = 0; i < 4; ++i)
	{
		if (m_eSkillState[m_iCurCharacter][i] == ACTIVATE)
			vColor = LEGEND_COLOR;
		else
			vColor = DEFAULT_COLOR;

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[3 + i], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackSlot[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackIcon[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_MOON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		//락 언락 처리
		if (m_eSkillState[m_iCurCharacter][i] == LOCK)
		{
			if (FAILED(m_pShader->SetRawValue("g_vColor", &m_fElemAlpha[4 + i], sizeof(_float3))))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_LOCK, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}
		else if (m_eSkillState[m_iCurCharacter][i] == UNLOCK)
		{
			vColor = { m_fElemAlpha[4 + i] * 0.5f , m_fElemAlpha[4 + i] * 0.5f, m_fElemAlpha[4 + i] * 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_ATTACK1 + i, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
		}
		else if (m_eSkillState[m_iCurCharacter][i] == ACTIVATE)
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_ATTACK1 + i, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackNeedLevel[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LV5 + i, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// QTE
	if (m_eSkillState[m_iCurCharacter][QTE] == ACTIVATE)
		vColor = LEGEND_COLOR;
	else
		vColor = DEFAULT_COLOR;

	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[8], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoQTESlot.WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoQTEIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_MOON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//락 언락 처리
	switch (m_eSkillState[m_iCurCharacter][QTE])
	{
	case LOCK:
		if (FAILED(m_pShader->SetRawValue("g_vColor", &m_fElemAlpha[9], sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_LOCK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		break;
	case UNLOCK:
		vColor = { m_fElemAlpha[9] * 0.5f , m_fElemAlpha[9] * 0.5f, m_fElemAlpha[9] * 0.5f };
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_QTE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		break;
	case ACTIVATE:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_QTE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		break;
	}

	// 버스트
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBurstSlot.WorldMatrix)))
		return E_FAIL;

	if (m_eSkillState[m_iCurCharacter][BURST] == ACTIVATE)
		vColor = LEGEND_COLOR;
	else
		vColor = DEFAULT_COLOR;

	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBurstSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWCIRCLE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBurstIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_MOON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	//락 언락 처리
	switch (m_eSkillState[m_iCurCharacter][BURST])
	{
	case LOCK:
		if (FAILED(m_pShader->SetRawValue("g_vColor", &m_fElemAlpha[10], sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_LOCK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		break;
	case UNLOCK:
		vColor = { m_fElemAlpha[10] * 0.5f , m_fElemAlpha[10] * 0.5f, m_fElemAlpha[10] * 0.5f };
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_BURST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		break;
	case ACTIVATE:
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_BURST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		break;
	}

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillPointText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	vColor = SLOT_LEGEND_COLOR;
	if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillPointText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_SKILLPOINT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	_uint iCharacterSP = m_pPlayerState->GetCharacterSP((CPlayerState::CHARACTERS)m_iCurCharacter);
	string strNumber = to_string(iCharacterSP);
	_uint iDigit = (_uint)strNumber.size();

	for (_uint i = 0; i < iDigit; ++i)
	{
		vColor = SLOT_LEGEND_COLOR;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillPoint[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strNumber[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	_float fValue = 1.f;
	switch (m_iCurSelectSkill)
	{
	case ATTACK1:
	case ATTACK2:
	case ATTACK3:
	case ATTACK4:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoAttackSlot[m_iCurSelectSkill].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ATTACK1 + m_iCurSelectSkill, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		break;
	case QTE:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_QTE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoQTESlot.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		break;

	case BURST:
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BURST, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[10], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBurstSlot.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SELECT_GLOWBUTTOM_SMALL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		break;
	}

	// Lock, Disable, Activate Text
	if (m_iCurSelectSkill != NONE)
	{
		switch (m_eSkillState[m_iCurCharacter][m_iCurSelectSkill])
		{
		case LOCK:
			vColor = { 0.6f, 0.2f, 0.2f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[11], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillState.WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LOCKED, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();

			if (m_iCurSelectSkill == QTE)
			{
				vColor = { 0.3f, 0.3f, 0.3f };
				if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[12], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillNeedDesc.WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_NEED_FIRSTATTACK, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(8);
				m_pVIBuffer->Render();
			}
			else if (m_iCurSelectSkill == BURST)
			{
				ORTHO_DESC BurstDesc = m_OrthoSkillNeedDesc;
				BurstDesc.fY -= 25.f;
				CAppManager::ComputeOrtho(&BurstDesc);

				vColor = { 0.3f, 0.3f, 0.3f };
				if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[12], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &BurstDesc.WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_NEED_LASTATTACK, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				m_pShader->Begin(8);
				m_pVIBuffer->Render();
			}

			break;
		case UNLOCK:
			vColor = { 0.5f, 0.5f, 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[11], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillState.WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_DISABLED, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
			break;
		case ACTIVATE:
			vColor = SLOT_LEGEND_COLOR;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[11], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillState.WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_LEFT_ACTIVATED, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			m_pShader->Begin(8);
			m_pVIBuffer->Render();
			break;
		}
	}

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[12], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillUpgradeBtn.WorldMatrix)))
		return E_FAIL;

	if (m_bUpgradeButtonClick)
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_CLICK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}
	else
	{
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_YELLOW_BUTTON_ON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
	}

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[12], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSkillUpgradeText.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ACTIVATE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	return S_OK;
}

void CResonatorUI::upgradeSkill(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CPlayerState::CHARACTER_STATE* playerState = m_pPlayerState->Get_CharState_byChar((CPlayerState::CHARACTERS)m_iCurCharacter);
	_uint iCurLevel = playerState->iCurLevel;

	if (iCurLevel >= 5)
	{
		if (m_eSkillState[m_iCurCharacter][ATTACK1] == LOCK)
			m_eSkillState[m_iCurCharacter][ATTACK1] = UNLOCK;
	}

	if (iCurLevel >= 10 && m_eSkillState[m_iCurCharacter][ATTACK1] == ACTIVATE)
	{
		if (m_eSkillState[m_iCurCharacter][ATTACK2] == LOCK)
			m_eSkillState[m_iCurCharacter][ATTACK2] = UNLOCK;
	}

	if (iCurLevel >= 15 && m_eSkillState[m_iCurCharacter][ATTACK2] == ACTIVATE)
	{
		if (m_eSkillState[m_iCurCharacter][ATTACK3] == LOCK)
			m_eSkillState[m_iCurCharacter][ATTACK3] = UNLOCK;
	}
	if (iCurLevel >= 20 && m_eSkillState[ATTACK3][m_iCurCharacter] == ACTIVATE)
	{
		if (m_eSkillState[m_iCurCharacter][ATTACK4] == LOCK)
			m_eSkillState[m_iCurCharacter][ATTACK4] = UNLOCK;
	}

	if (m_eSkillState[m_iCurCharacter][ATTACK1] == ACTIVATE && m_eSkillState[m_iCurCharacter][QTE] != ACTIVATE)
	{
		m_eSkillState[m_iCurCharacter][QTE] = UNLOCK;
	}

	if (m_eSkillState[m_iCurCharacter][ATTACK4] == ACTIVATE && m_eSkillState[m_iCurCharacter][BURST] != ACTIVATE)
	{
		m_eSkillState[m_iCurCharacter][BURST] = UNLOCK;
	}

	if (pGameMode->OnMouse(m_OrthoSkillUpgradeBtn))
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			m_bUpgradeButtonClick = true;
		else if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
			m_bUpgradeButtonClick = true;
		else if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
		{
			m_bUpgradeButtonClick = false;

			_uint iSkillPoint = m_pPlayerState->GetCharacterSP((CPlayerState::CHARACTERS)m_iCurCharacter);

			if (iSkillPoint >= 1)
			{
				switch (m_iCurSelectSkill)
				{
				case ATTACK1:
				case ATTACK2:
				case ATTACK3:
				case ATTACK4:
					if (m_eSkillState[m_iCurCharacter][m_iCurSelectSkill] == UNLOCK)
						m_pPlayerState->SetAttackFactor((CPlayerState::CHARACTERS)m_iCurCharacter, m_iCurSelectSkill, 1.1f);
					
					break;
				case QTE:
					if (m_eSkillState[m_iCurCharacter][m_iCurSelectSkill] == UNLOCK)
						m_pPlayerState->SetAirboneQTE((CPlayerState::CHARACTERS)m_iCurCharacter, true);

					break;
				case BURST:
					if (m_eSkillState[m_iCurCharacter][m_iCurSelectSkill] == UNLOCK)
						m_pPlayerState->SeBurstFactor((CPlayerState::CHARACTERS)m_iCurCharacter, 1.5f);

					break;
				}

				m_eSkillState[m_iCurCharacter][m_iCurSelectSkill] = ACTIVATE;
			}
			else
			{
				m_bUpgradeButtonClick = false;
			}
		}
	}
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

	///
	//if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlot[i].WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(8);
	//m_pVIBuffer->Render();

	//if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_BACK, m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;
	//m_pShader->Begin(10);
	//m_pVIBuffer->Render();

	//// ItemName Back
	//vColor3 = getGradeToneDownColor((_uint)m_pInven->GetSlotData(eInvenType, i).eItemGrade);
	//vColor4 = { vColor3.x, vColor3.y, vColor3.z, (m_fElemAlpha[i + 1] / 1.f) * 0.5f };
	//if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemTextBack[i].WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA_WHITE, m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(11);
	//m_pVIBuffer->Render();

	//// ItemName
	//if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemText[i].WorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, i).iImageTextIndex, m_pShader, "g_DiffuseTexture")))
	//	return E_FAIL;

	//m_pShader->Begin(10);
	//m_pVIBuffer->Render();
	///

	// Select Weapon Text
	_uint iSlotCount = m_pInven->GetInvenTotalCount(CInventory::INVEN_WEAPON);
	for (_uint i = 0; i < iSlotCount; ++i)
	{
		CItem::ITEM_DESC pSlotItem = m_pInven->GetSlotData(CInventory::INVEN_WEAPON, i);
	
		// Weapon Slot
		_float3 vItemSlotColor = CItemDB::GetItemSlotColor(pSlotItem.eItemGrade);
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlot[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
		
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlot[i].WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vItemSlotColor, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlotBack[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA_WHITE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoWeaponSlotName[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(pSlotItem.iImageTextIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
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
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
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

	// 선택된 무기 강화 횟수
	for (_uint i = 0; i < MAX_WEAPON_UPGRADE; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[9], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponUpgrade[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_UPGRADE_SIGN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		//무기의 강화 횟수
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

	//// 수치 (직교 반대로 되있는듯)
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

	//// 데미지
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

	//// 크리티컬 확률
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

	// 퍼센트
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[5], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSelectWeaponCri[iDamageDigit].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_PER, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	// Confirm 버튼
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
	case BTN_RESONANCE:
		resonanceInput(TimeDelta);
		break;
	case BTN_WUTERIDE:
		forteInput(TimeDelta);
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

		//선택된 아이콘색상 변경
		if (m_iCurCharacter == i)
		{
			_float3 vColor = UNIQUE_COLOR;
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
	CResonatorUI* pInstance = new CResonatorUI(pDevice, pContext);
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
