#pragma once

#include "Level.h"

BEGIN(Engine)
class CCamera;
END

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void RenderLevelUI() override;
private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar * pLayerTag);

	HRESULT Ready_Layer_MapObject_Tree(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Rock(const _tchar* pLayerTag);
	
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	class CDynamicCamera* m_pDynamicCamera = nullptr;
	class CPlayerCamera* m_pPlayerCamera = nullptr;
	
	class CAcquireSystem* m_pAcquireSystem = nullptr;
};

END