#pragma once

#include "Level.h"

BEGIN(Engine)
class CCamera;
class CGameObject;
END

BEGIN(Client)

class CLevel_Forest final : public CLevel
{
private:
	explicit CLevel_Forest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Forest() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void RenderLevelUI() override;
private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster();
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

	HRESULT Ready_Interaction_Object(const _tchar* pLayerTag);

private:
	HRESULT Ready_Layer_Trigger(const _tchar* pLayerTag);
	HRESULT Load_TriggerData(const _tchar* pDataFilePath, const _tchar* pObjectTag, const _tchar* pLayerTa, const _tchar* pEditionFilePath = nullptr, const _tchar* pMonsterLayerTag = nullptr);

private:
	// 로드 -> 로드 하기전 스폰 포인트 정보를 초기화 후 로드 한 데이터에 맞게 세팅함.
	HRESULT Load_SpawnPoint(const _tchar* pDataFilePath);
	// 초기화 -> 한 스폰 포인트 데이터를 불러온 개수만큼 적용후 초기화.
	void Clear_SpawnPoint();
	// 몇번째 스폰 포인트 , 어떤 몬스터 추가 ( 최대 스폰 포인트 개수 초과시 추가 x , 몬스터 번호 등록 안된거 추가시 추가 x )
	HRESULT Add_Monstaer(_uint iSpawnPointNum, _uint iMonsterModelNum, const _tchar* pLayerTag);

	// 몬스터 추가만 해놓는다. -> 레이어에서 하나씩 부를때.
	HRESULT Add_Monstaer(_uint iMonsterModelNum, const _tchar* pLayerTag);

private:
	// 스폰 포인트 총 갯수
	_uint						m_iSpawnPointCount = { 0 };
	// 스폰 포인트 정보 (위치 , 셀번호)
	vector<SPAWN_POINT>			m_SpawnPoints = {};
	// 스폰 포인트 인덱스
	_uint						m_iSpawnPointIndex = { 0 };

public:
	static CLevel_Forest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	_bool m_bBakeStart = false;
	CGameObject* m_pAcquireSystem = nullptr;
};

END