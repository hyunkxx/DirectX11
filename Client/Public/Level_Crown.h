#pragma once

#include "Level.h"

BEGIN(Engine)
class CCamera;
class CGameObject;
END

BEGIN(Client)

class CLevel_Crown final : public CLevel
{
private:
	explicit CLevel_Crown(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Crown() = default;

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

	HRESULT Ready_Layer_MapObject_Tree(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Rock(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Floor(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Stairs(const _tchar* pLayerTag);

	HRESULT Ready_Layer_MapObject_Grass(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Vin(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Veg(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Shr(const _tchar* pLayerTag);

	HRESULT Ready_Layer_MapObject_Structure(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Pil(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Statue(const _tchar* pLayerTag);

	HRESULT Ready_Layer_MapObject_NoiRock(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MapObject_Tof_Grass(const _tchar* pLayerTag);

private:
	HRESULT Ready_Layer_Trigger(const _tchar* pLayerTag);
	HRESULT Load_TriggerData(const _tchar* pDataFilePath, const _tchar* pObjectTag, const _tchar* pLayerTag, const _tchar* pEditionFilePath = nullptr);

private:
	// 스폰 포인트 총 갯수
	_uint						m_iSpawnPointCount = { 0 };
	// 스폰 포인트 정보 (위치 , 셀번호)
	vector<SPAWN_POINT>			m_SpawnPoints = {};

public:
	static CLevel_Crown* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	_bool m_bBakeStart = false;
	CGameObject* m_pAcquireSystem = nullptr;
};

END