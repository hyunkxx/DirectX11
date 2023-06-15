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

class CHuluUI final
	: public CGameObject
	, public IOnClick
	, public IActivate
{
protected:
	explicit CHuluUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHuluUI(const CHuluUI& rhs);
	virtual ~CHuluUI() = default;

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
	static CHuluUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	ORTHO_DESC m_OrthoHuru;
	ORTHO_DESC m_OrthoTitleText;
	ORTHO_DESC m_OrthoTitleSub;
	ORTHO_DESC m_OrthoActiveGage;
	ORTHO_DESC m_OrthoActiveText;

	ORTHO_DESC m_OrthoMainSlot;
	ORTHO_DESC m_OrthoMainIcon;

	ORTHO_DESC m_OrthoAbsorb;
	ORTHO_DESC m_OrthoBack[4];
	ORTHO_DESC m_OrthoText[4];
	ORTHO_DESC m_OrthoHuluIcon;
	ORTHO_DESC m_OrthoActiveCount[5];
	
	_uint m_iActiveEchoCount = 0;

private:
	_bool m_bActive = false;
	_bool m_bRender = false;

};

END