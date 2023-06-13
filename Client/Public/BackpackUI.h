#pragma once

#include "UIInterface.h"
#include "GameObject.h"
#include "Inventory.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CShader;

END

BEGIN(Client)

class CBackpackUI
	: public CGameObject
	, public IOnClick
	, public IActivate
{

public:
	explicit CBackpackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBackpackUI(const CBackpackUI& rhs);
	virtual ~CBackpackUI() = default;

public://GameObject
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI() override;

public://Interface
	virtual void OnClick() override;

	virtual void SetActive(_bool bValue) override;
	virtual void SetRender(_bool bValue) override;

private:
	HRESULT addComponents();
	void stateActive();
	void stateDisable();

	void elemAlphaUpdate(_double TimeDelta);
	void elemAlphaReset(_uint iStartIndex = 0);

	void keyInput(_double TimeDelta);
	
private: // Render
	_float3 getGradeColor(_uint iGrade);
	_float3 getGradeToneDownColor(_uint iGrade);

	HRESULT mainRender();

public:
	static CBackpackUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader* m_pShader = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;

private:
	class CTerminalUI* m_pTerminal = nullptr;
	class CInventory* m_pInven = nullptr;
	class CPlayerState* m_pPlayerState = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;
	class CEchoSystem* m_pEchoSystem = nullptr;

private:
	_float4x4 m_ViewMatrix, m_ProjMatrix;

private:
	enum { ITEM_SLOT_MAX = 20 };

#pragma region MAIN_FRAME
	ORTHO_DESC m_OrthoBackground;

	// Smooth UI
	enum { SMOOTH_MAX = 30 };
	_float m_fElemAlpha[SMOOTH_MAX];
	_bool m_bElemAlphaStart[SMOOTH_MAX];

	_float m_fCurFlashAcc[ITEM_SLOT_MAX];
	_float m_fCurFlashIndex[ITEM_SLOT_MAX];
	_bool m_bFlashBegin[ITEM_SLOT_MAX];

	_bool m_bOneFlash[ITEM_SLOT_MAX];
#pragma endregion

	ORTHO_DESC m_OrthoBackpackSlot;
	ORTHO_DESC m_OrthoBackpackIcon;
	ORTHO_DESC m_OrthoSlot[ITEM_SLOT_MAX];
	ORTHO_DESC m_OrthoItemTextBack[ITEM_SLOT_MAX];
	ORTHO_DESC m_OrthoItemText[ITEM_SLOT_MAX];

	ORTHO_DESC m_OrthoItemAmountText[ITEM_SLOT_MAX];
	ORTHO_DESC m_OrthoItemAmount[ITEM_SLOT_MAX][6];

	ORTHO_DESC m_OrthoTapBack[CInventory::INVEN_MAX];
	ORTHO_DESC m_OrthoTapText[CInventory::INVEN_MAX];

	ORTHO_DESC m_OrthoItemIcon[ITEM_SLOT_MAX];

	ORTHO_DESC m_OrthoDetailBack;
	ORTHO_DESC m_OrthoDetailIcon;
	ORTHO_DESC m_OrthoDetailName;
	ORTHO_DESC m_OrthoDetailDescBack;
	ORTHO_DESC m_OrthoDetailDesc;

	ORTHO_DESC m_OrthoMainCoinBack;
	ORTHO_DESC m_OrthoMainCoinIcon;
	ORTHO_DESC m_OrthoMainCoinAmount[7];

	ORTHO_DESC m_OrthoSubCoinIcon;
	ORTHO_DESC m_OrthoSubCoinAmount[7];
	
	ORTHO_DESC m_OrthoFoodUse;
	ORTHO_DESC m_OrthoItemDiscard;

	ORTHO_DESC m_OrthoDiscardBack;
	ORTHO_DESC m_OrthoDiscardSlot;
	ORTHO_DESC m_OrthoDiscardIcon;

	ORTHO_DESC m_OrthoDiscardExit;
	ORTHO_DESC m_OrthoCalcKey[12];
	ORTHO_DESC m_OrthoOwnText;
	ORTHO_DESC m_OrthoOwnAmount[6];
	ORTHO_DESC m_OrthoDiscardText;
	ORTHO_DESC m_OrthoDiscardAmount[6];
	
	_uint m_iDiscardCurDigit = 0;
	_uint m_iDiscardAmount[7];
	_uint m_iCalcResultAmount;

	CInventory::INVEN_TYPE eInvenType = CInventory::INVEN_WEAPON;

	_uint m_iCurSlot = 0;
	_float m_fTapAlpha[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	_float m_fCurDescAlpha = 0.f;

	_bool m_bDiscardPanel = false;

private:
	_bool m_bActive = false;
	_bool m_bRender = false;

};

END