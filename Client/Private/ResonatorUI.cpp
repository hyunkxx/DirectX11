#include "stdafx.h"
#include "..\Public\ResonatorUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "PlayerState.h"
#include "Inventory.h"

#include "TerminalUI.h"
#include "ItemDB.h"

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

#pragma region Weapon
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

#pragma endregion



	// 기타 임시 디폴트 세팅
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	
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

		break;
	case CResonatorUI::BTN_ECHO:
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
	// 개별 요소 알파 올리기 8개는 임의 지정
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

	m_bElemAlphaStart[0] = true;
	m_bSlotRenderFinish = false;
}

void CResonatorUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;
	m_fBackgroundAlpha = 1.f;
	m_bButtonActive[0] = true;
	m_eCurButton = BTN_STATE;
	m_bSlotRenderFinish = false;

	ZeroMemory(m_iUseAmount, sizeof m_iUseAmount);
	ZeroMemory(m_fFlashCount, sizeof m_fFlashCount);
	ZeroMemory(m_bFlashStart, sizeof m_bFlashStart);
	ZeroMemory(m_fFlashAcc, sizeof m_fFlashAcc);

	elemAlphaReset();
}

void CResonatorUI::stateDisable()
{

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
						elemAlphaReset();

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
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fBackgroundAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DETAILPANEL, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

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

void CResonatorUI::weaponKeyInput(_double TimeDelta)
{
	selectCharacter(TimeDelta);
	pushUpgradeButton(TimeDelta);
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
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_WEAPON_UPGRADE_BUTTON, m_pShader, "g_DiffuseTexture")))
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