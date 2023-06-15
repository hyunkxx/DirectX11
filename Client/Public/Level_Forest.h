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
	// �ε� -> �ε� �ϱ��� ���� ����Ʈ ������ �ʱ�ȭ �� �ε� �� �����Ϳ� �°� ������.
	HRESULT Load_SpawnPoint(const _tchar* pDataFilePath);
	// �ʱ�ȭ -> �� ���� ����Ʈ �����͸� �ҷ��� ������ŭ ������ �ʱ�ȭ.
	void Clear_SpawnPoint();
	// ���° ���� ����Ʈ , � ���� �߰� ( �ִ� ���� ����Ʈ ���� �ʰ��� �߰� x , ���� ��ȣ ��� �ȵȰ� �߰��� �߰� x )
	HRESULT Add_Monstaer(_uint iSpawnPointNum, _uint iMonsterModelNum, const _tchar* pLayerTag);

	// ���� �߰��� �س��´�. -> ���̾�� �ϳ��� �θ���.
	HRESULT Add_Monstaer(_uint iMonsterModelNum, const _tchar* pLayerTag);

private:
	// ���� ����Ʈ �� ����
	_uint						m_iSpawnPointCount = { 0 };
	// ���� ����Ʈ ���� (��ġ , ����ȣ)
	vector<SPAWN_POINT>			m_SpawnPoints = {};
	// ���� ����Ʈ �ε���
	_uint						m_iSpawnPointIndex = { 0 };

public:
	static CLevel_Forest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	_bool m_bBakeStart = false;
	CGameObject* m_pAcquireSystem = nullptr;
};

END