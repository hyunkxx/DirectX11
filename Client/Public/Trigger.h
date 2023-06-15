#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "PlayerState.h"
#include "Layer.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CTexture;

class CVIBuffer_PointBox;
END

BEGIN(Client)

class CTrigger : public CGameObject
{
public:
	enum TRIGGER_TYPE { TYPE_PORTAL, TYPE_SPAWN, TYPE_INTERACT, TYPE_END };

	enum TRIGGER_ID { 
		ID_POTAL_CITY,
		ID_POTAL_FOREST,
		ID_POTAL_CROWN,

		ID_SPAWN_FOREST_0,
		ID_SPAWN_FOREST_1,
		ID_SPAWN_FOREST_2,
		ID_SPAWN_FOREST_3,

		ID_SPAWN_CROWN,

		ID_INTERACT_COOK,

		ID_END
	};

private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;

public:
	TRIGGER_TYPE Get_TriggerType() const { return m_eTriggerType; }
	TRIGGER_ID Get_TriggerID() const { return m_eTriggerID; }
	TRIGGER_DESC Get_TriggerDesc() const { return m_TriggerDesc; }

	_bool IsIn_Trigger() const { return m_IsInTrigger; }
	_bool Is_Trigger() const { return m_IsTrigger; }

public:
	void SetUp_State();

public:
	// 트리거 종류 별로 발동 조건 구분 ( 키입력 )
	void Trigger_Condition(); 

	// 발동조건 충족후 트리거 종류별로 처리 구분. -> 처리 한뒤에 m_IsTrigger false 로 초기화.
	void Trigger_Process(); 


public: // 트리거 별 처리 함수
	void Trigger_Potal_City();
	void Trigger_Potal_Forest();
	void Trigger_Potal_Crown();

	void Trigger_Spawn_Forest_0();
	void Trigger_Spawn_Forest_1();
	void Trigger_Spawn_Forest_2();
	void Trigger_Spawn_Forest_3();

	void Trigger_Spawn_Crown();

	void Trigger_Interact_Cook();

private:
	TRIGGER_DESC				m_TriggerDesc = {};
	TRIGGER_TYPE				m_eTriggerType = { TRIGGER_TYPE::TYPE_END };

	TRIGGER_ID					m_eTriggerID = { TRIGGER_ID::ID_END };

private:
	// 트리거 범위 안에 접근 했을 경우.
	_bool						m_IsInTrigger = { false };
	_bool						m_IsTrigger = { false };

	_float						m_fDistance = { 0.0f };

	_vector						m_vPlayerPos = { 0.0f, 0.0f, 0.0f, 0.0f };
	_vector						m_vTriggerPos = { 0.0f, 0.0f, 0.0f, 0.0f };

	// 한번만 트리거를 발동해야 할 때.
	_bool						m_IsOnlyOneTrigger = { false };

public: /* For. Spawn Trigger */
	// 스폰 포인트 증가 + 사이클 돌리기 ( 최대개수 도달시 0으로 초기화 )
	void Add_SpawnPoint();

	void Clear_SpawnPoint();
	void Clear_MonsterSpawnControl();

	void Monster_Spawn();
	void Check_ActiveMonster();

private:
	// 최대 스폰 포인트 개수
	_uint						m_iSpawnPointCount = { 0 };
	vector<SPAWN_POINT>			m_SpawnPoints = {};

	// 스폰 포인트 인덱스
	_uint						m_iSpawnPointIndex = { 0 };
	// 몬스터 스폰 시작.
	_bool						m_IsSpawnMonster = { false };
;

	// 진행중인 몬스터 스폰 인덱스 -> Max 가 되면 스폰 중단.
	_uint						m_iSpawnMonsterIndex = { 0 };
	// 한 스폰지점에서의 총 스폰 몬스터 마리수
	//const _uint&				m_iMaxSpawnMonsterIndex = { 8 };
	const _uint&				m_iMaxSpawnMonsterIndex = { 4 };

	// 현재 소환된 마리수 체크
	_uint						m_iCurrentSpawnMonsterCount = { 0 };
	// 몬스터 스폰 마리수 제한 ( 한번에 스폰 가능한 수 )
	const _uint&				m_iMonsterSpawnLimitCount = { 4 };


	// 몬스터 찾아올 레이어
	CLayer*						m_pLayer = { nullptr };

public:
	HRESULT Load_SpawnPoint();

private:
	CRenderer*					m_pRendererCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };

	CPlayerState*				m_pPlayerState = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
