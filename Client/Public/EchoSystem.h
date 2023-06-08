#pragma once

#include "GameObject.h"

BEGIN(Client)

class CEchoSystem final : public CGameObject
{
public:
	typedef struct tagEcho
	{
		_bool bActive;
		_bool bBind;
		_uint iGrade;
		_uint iLevel;
		_float fAttack;
		_uint iNameTextureID;
		_uint iIconTextureID;

	}ECHO;

public:
	enum ECHO_TYPE
	{
		//����
		EC_ZIGZAG,
		EC_TICTAC,				//������
		EC_WHOOSH,				//�����
		EC_SNIPSNAP,			//�����
		
		// �ΰ���
		EC_RUPTURE_WARRIOR,		// ����
		EC_FUSHION_WARRIOR,		// �ݻ����ڰ�

		EC_IMPULSE_PREDATOR,	// ������
		EC_GLACIO_PREDATOR,		// ����
		EC_PNENUMA_PREDATOR,	// ���ʻ�

		EC_ELITE_STONE,			// ����Ʈ1
		EC_ELITE_FLUTE,			// ����Ʈ2
		EC_CROWNLESS,			// ����
		EC_EMPTY
	};

	enum ECHO_SLOT { ROVER, YANGYANG, CHIXIA, CHAR_END };
private:
	explicit CEchoSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEchoSystem(const CEchoSystem& rhs);
	virtual ~CEchoSystem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

public:
	_bool BindEcho(ECHO_SLOT eTargetCharacter, ECHO_TYPE eEchoIndex);
	void ReleaseEcho(ECHO_SLOT eTargetCharacter);

	const ECHO* GetRegisteredEcho(ECHO_SLOT eTargerCharacter) { return m_BindEcho[eTargerCharacter]; }
	ECHO_TYPE GetRegisterdEchoType(ECHO_SLOT eTargerCharacter) { return m_BindEchoType[eTargerCharacter]; }
	_float ComputeDamage(ECHO_SLOT eTargetCharacter);

	const ECHO& GetEcho(ECHO_TYPE eEchoType) const { return m_Echos[eEchoType]; }

public:
	static CEchoSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	ECHO* m_BindEcho[CHAR_END] = { nullptr, };
	ECHO  m_Echos[EC_EMPTY];

	ECHO_TYPE m_BindEchoType[CHAR_END] = { EC_EMPTY, };
};

END