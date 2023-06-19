#pragma once

#include "GameObject.h"
#include "Character.h"

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
		//Á¤·É
		EC_ZIGZAG,
		EC_TICTAC,				//µ¿±ÛÀÌ
		EC_WHOOSH,				//³ì»ö´«
		EC_SNIPSNAP,			//³ë¶õ´«
		
		// ÀÎ°£Çü
		EC_RUPTURE_WARRIOR,		// ³´¸Ç
		EC_FUSHION_WARRIOR,		// ±Ý»ö½ÊÀÚ°¡

		EC_IMPULSE_PREDATOR,	// ²¿±òÄÜ
		EC_GLACIO_PREDATOR,		// ¸ðÀÚ
		EC_PNENUMA_PREDATOR,	// ¾çÂÊ»Ô

		EC_ELITE_STONE,			// ¿¤¸®Æ®1
		EC_ELITE_FLUTE,			// ¿¤¸®Æ®2
		EC_CROWNLESS,			// º¸½º
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
	_uint GetActiveEchoCount() const { return m_iActivateEcho; }
	void SetActiveEcho(ECHO_TYPE eEchoType) { m_Echos[eEchoType].bActive = true; m_iActivateEcho++; }

	_bool BindEcho(ECHO_SLOT eTargetCharacter, ECHO_TYPE eEchoIndex);
	void ReleaseEcho(ECHO_SLOT eTargetCharacter);

	const ECHO* GetRegisteredEcho(ECHO_SLOT eTargerCharacter) { return m_BindEcho[eTargerCharacter]; }
	ECHO_TYPE GetRegisterdEchoType(ECHO_SLOT eTargerCharacter) { return m_BindEchoType[eTargerCharacter]; }
	_float ComputeDamage(ECHO_SLOT eTargetCharacter);

	const ECHO& GetEcho(ECHO_TYPE eEchoType) const { return m_Echos[eEchoType]; }

	_uint GetEchoLevel(ECHO_TYPE eEchoType);
	_bool Levelup(ECHO_TYPE eEchoType);

	void Register_EchoPtr(ECHO_TYPE eEchoType, CCharacter* pEcho)
	{
		m_pEchoPtr[eEchoType] = pEcho;
	}

	void Shot_Echo(ECHO_SLOT eSlot, CTransform* pTransform, CCharacter* pTarget, _uint iNaviCellID)
	{
		if (ROVER == eSlot)
		{
			if(m_BindEcho[0])
				m_pEchoPtr[m_BindEchoType[0]]->Shot_Echo(pTransform, pTarget, iNaviCellID);
		}
		else if (YANGYANG == eSlot)
		{
			if (m_BindEcho[1])
				m_pEchoPtr[m_BindEchoType[1]]->Shot_Echo(pTransform, pTarget, iNaviCellID);
		}
		else if (CHIXIA == eSlot)
		{
			if (m_BindEcho[2])
				m_pEchoPtr[m_BindEchoType[2]]->Shot_Echo(pTransform, pTarget, iNaviCellID);
		}
		
	}

public:
	static CEchoSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint m_iActivateEcho = 0;
	ECHO* m_BindEcho[CHAR_END] = { nullptr, };
	ECHO  m_Echos[EC_EMPTY];

	ECHO_TYPE m_BindEchoType[CHAR_END] = { EC_EMPTY, };
	
	CCharacter* m_pEchoPtr[EC_EMPTY] = { nullptr, };
};

END