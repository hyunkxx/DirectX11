#pragma once

#include "UIInterface.h"
#include "GameObject.h"
#include "Inventory.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CTeamUI final
	: public CGameObject
	, public IOnClick
	, public IActivate
{
protected:
	explicit CTeamUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTeamUI(const CTeamUI& rhs);
	virtual ~CTeamUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

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

	//Render
	HRESULT mainRender();

public:
	static CTeamUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	ORTHO_DESC m_OrthoBackground;

	// Smooth UI
	enum { SMOOTH_MAX = 30 };
	_float m_fElemAlpha[SMOOTH_MAX];
	_bool m_bElemAlphaStart[SMOOTH_MAX];

private:
	ORTHO_DESC m_OrthoTeamTitle;
	ORTHO_DESC m_OrthoTeamSubTitle;
	ORTHO_DESC m_OrthoPartyBack;

	ORTHO_DESC m_OrthoCharImage[3];
	ORTHO_DESC m_OrthoCharName[3];
	ORTHO_DESC m_OrthoCharGage[3];
	ORTHO_DESC m_OrthoCharIcon[3];
	ORTHO_DESC m_OrthoCharLevelText[3];
	ORTHO_DESC m_OrthoCharLevel[3][2];
private:
	_bool m_bActive = false;
	_bool m_bRender = false;

};

END