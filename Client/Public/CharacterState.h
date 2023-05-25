#pragma once

#include "GameObject.h"

BEGIN(Client)

class CCharacterState final : public CGameObject
{
private:
	enum { MAX_LEVEL = 50 };

public:
	enum CHARACTER_SLOT { CHARACTOR_MAIN, CHARACTOR_TWO, CHARACTOR_TRD, CHARACTER_END };

	// ĳ���� �⺻ ����
	typedef struct tagCharacterState
	{
		_uint  iLevel;
		_float fCurExp;
		_float fMaxExp;

		_float fCurHP;
		_float fMaxHP;

		_float fAttack;		//�⺻ ���ݷ�
		_float fAttackEx;	//�߰� ���ݷ�

		_float fDefense;	//�⺻ ����
		_float fDefenseEx;	//�߰� ����

		_float fCriticalRate;
		_float fCriticalDamage;
	}CHARACTER_STATE;

private:
	CCharacterState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterState(const CCharacterState& rhs);
	virtual ~CCharacterState() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual void RenderGUI();

public:
	void AddExp(CHARACTER_SLOT eCharater, _float fExp);

private:
	void levelUp(CHARACTER_SLOT eCharater);

public:
	static CCharacterState* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

private:
	CHARACTER_STATE m_CharacterState[CHARACTER_END];

};

END