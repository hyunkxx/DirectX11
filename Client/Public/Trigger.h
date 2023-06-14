#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

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
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;

public:
	TRIGGER_TYPE Get_TriggerType() const { return m_eTriggerType; }
	_bool IsIn_Trigger() const { return m_IsInTrigger; }

	TRIGGER_ID Get_TriggerID() const { return m_eTriggerID; }

	TRIGGER_DESC Get_TriggerDesc() const { return m_TriggerDesc; }

public:
	void SetUp_State();

private:
	TRIGGER_DESC				m_TriggerDesc = {};
	TRIGGER_TYPE				m_eTriggerType = { TRIGGER_TYPE::TYPE_END };

	TRIGGER_ID					m_eTriggerID = { TRIGGER_ID::ID_END };

	// 트리거 범위 안에 접근 했을 경우.
	_bool						m_IsInTrigger = { false };
	
	_float						m_fDistance = { 0.0f };

	_tchar						m_szDataFilePath[MAX_PATH] = { L"" };

private:
	CRenderer*					m_pRendererCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };


private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
