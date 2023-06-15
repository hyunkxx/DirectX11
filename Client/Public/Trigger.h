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
	// Ʈ���� ���� ���� �ߵ� ���� ���� ( Ű�Է� )
	void Trigger_Condition(); 

	// �ߵ����� ������ Ʈ���� �������� ó�� ����. -> ó�� �ѵڿ� m_IsTrigger false �� �ʱ�ȭ.
	void Trigger_Process(); 


public: // Ʈ���� �� ó�� �Լ�
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
	// Ʈ���� ���� �ȿ� ���� ���� ���.
	_bool						m_IsInTrigger = { false };
	_bool						m_IsTrigger = { false };

	_float						m_fDistance = { 0.0f };

	_vector						m_vPlayerPos = { 0.0f, 0.0f, 0.0f, 0.0f };
	_vector						m_vTriggerPos = { 0.0f, 0.0f, 0.0f, 0.0f };

	// �ѹ��� Ʈ���Ÿ� �ߵ��ؾ� �� ��.
	_bool						m_IsOnlyOneTrigger = { false };

public: /* For. Spawn Trigger */
	// ���� ����Ʈ ���� + ����Ŭ ������ ( �ִ밳�� ���޽� 0���� �ʱ�ȭ )
	void Add_SpawnPoint();

	void Clear_SpawnPoint();
	void Clear_MonsterSpawnControl();

	void Monster_Spawn();
	void Check_ActiveMonster();

private:
	// �ִ� ���� ����Ʈ ����
	_uint						m_iSpawnPointCount = { 0 };
	vector<SPAWN_POINT>			m_SpawnPoints = {};

	// ���� ����Ʈ �ε���
	_uint						m_iSpawnPointIndex = { 0 };
	// ���� ���� ����.
	_bool						m_IsSpawnMonster = { false };
;

	// �������� ���� ���� �ε��� -> Max �� �Ǹ� ���� �ߴ�.
	_uint						m_iSpawnMonsterIndex = { 0 };
	// �� �������������� �� ���� ���� ������
	//const _uint&				m_iMaxSpawnMonsterIndex = { 8 };
	const _uint&				m_iMaxSpawnMonsterIndex = { 4 };

	// ���� ��ȯ�� ������ üũ
	_uint						m_iCurrentSpawnMonsterCount = { 0 };
	// ���� ���� ������ ���� ( �ѹ��� ���� ������ �� )
	const _uint&				m_iMonsterSpawnLimitCount = { 4 };


	// ���� ã�ƿ� ���̾�
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
