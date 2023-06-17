#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "PlayerState.h"
#include "Layer.h"

BEGIN(Engine)
class CEffect;
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
		ID_INTERACT_COOKNPC,
		ID_INTERACT_SHOP_1,
		ID_INTERACT_SHOP_2,

		ID_END
	};

	enum SPAWN_WAVE {
		WAVE_1,
		WAVE_2,
		WAVE_3,
		WAVE_END
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
	void Distance_Check();

	void Floating(_double TimeDelta);

private:
	_float4x4					m_WorldMatrix_Origin = {};
	_float3						m_vOrigin_Pos = { 0.0f, 0.0f, 0.0f };

	_float4x4					m_WorldMatrix_Potal = {};

	_float						m_fFloatingTimeAcc = { 0.0 };

	_float						m_fFloating_Power = { 0.0f };
	_float						m_fFloating_Speed = { 0.0f };

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
	void Trigger_Interact_CookNpc();
	void Trigger_Interact_Shop_1();
	void Trigger_Interact_Shop_2();

	void ShowEffect(_double TimeDelta);

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

	// 트리거 이펙트 보여주고 작동시키기.
	_bool						m_IsTriggerEffect = { false };
	_double						m_EffectTimeAcc = { 0.0 };
	_double						m_EffectTime = { 0.0 };
	_bool						m_OnlyOnePlay = { false };

	_double						m_FadeStartTime = { 0.0 };
	_double						m_FadeTime = { 0.0 };
	_bool						m_IsFade = { false };


public: /* For. Spawn Trigger */
	// 스폰 포인트 증가 + 사이클 돌리기 ( 최대개수 도달시 0으로 초기화 )
	void Add_SpawnPoint();

	void Clear_SpawnPoint();
	void Clear_MonsterSpawnControl();

	void Wave_Start();
	void Monster_Spawn();

private:
	// 최대 스폰 포인트 개수
	_uint						m_iSpawnPointCount = { 0 };
	vector<SPAWN_POINT>			m_SpawnPoints = {};

	// 스폰 포인트 인덱스
	_uint						m_iSpawnPointIndex = { 0 };
	// 몬스터 스폰 시작.
	_bool						m_IsSpawnMonster = { false };
	
	// 웨이브가 진행중임을 체크
	_bool						m_IsWave = { false };
	// 현재 웨이브가 끝났는지를 체크.
	_bool						m_IsWave_End = { false };
	// 현재 웨이브.
	_uint						m_iCurrent_Wave = { 0 };
	// 최대 웨이브
	_uint						m_iLimitWave = { 0 };

public:
	HRESULT Link_WaveMonster(SPAWN_WAVE eWave, CCharacter* pCharacter);
	void ClearLink_Monster();

private:
	vector<CCharacter*>			m_pMonsters[SPAWN_WAVE::WAVE_END] = {};

public:
	HRESULT Load_SpawnPoint();

private:
	CPlayerState*				m_pPlayerState = { nullptr };

	CEffect*					m_pPotal = { nullptr };
	CEffect*					m_pPotalEffect = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
