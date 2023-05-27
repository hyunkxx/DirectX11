#pragma once

#include "Level.h"

BEGIN(Engine)
class CCamera;
class CGameObject;
END

BEGIN(Client)

class CLevel_Test final : public CLevel
{
private:
	explicit CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Test() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void RenderLevelUI() override;
private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar * pLayerTag);

public:
	static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	CGameObject* m_pAcquireSystem = nullptr;
};

END