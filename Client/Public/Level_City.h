#pragma once

#include "Level.h"

BEGIN(Engine)
class CCamera;
class CGameObject;
END

BEGIN(Client)

class CLevel_City final : public CLevel
{
private:
	explicit CLevel_City(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_City() = default;

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

private:
	HRESULT Ready_Layer_CityObject();
	HRESULT Load_CityObject(const _tchar* pDataFilePath, const _tchar* pObjectTag);

public:
	static CLevel_City* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	_bool m_bBakeStart = false;
	CGameObject* m_pAcquireSystem = nullptr;
};

END