#pragma once

#include "Base.h"
#include "Client_Defines.h"


BEGIN(Client)

class CStateKey abstract : public CBase
{
public:
	typedef enum KeyType {
		TYPE_EFFECT,
		TYPE_DISSOLVE,
		TYPE_SOUND,
		TYPE_WEAPON,
		TYPE_MISSILE,
		TYPE_PRIORITY,
		TYPE_END
	}TYPE;

	struct SaveData
	{
		// 필수 요소
		_int iType;
		_int iIndex;
		_float ShotFrame;
		// 이하 일부에서만 쓰이는 선택요소
		_int	iInt;
		_float fFloat;
		char szTag[MAX_PATH];
		_bool	bBool;
	};
protected:
	CStateKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStateKey() = default;

public:
	virtual HRESULT Initialize(_double ShotFrame);
	virtual void Check(_double FrameAcc, class CCharacter* pCharacter);
	virtual void Shot(class CCharacter* pCharacter) PURE;

	void Reset() { m_bReady = true; }

	SaveData* Get_Data()
	{
		return &m_tData;
	}

	void Set_Data(SaveData* pData)
	{
		memcpy(&m_tData, pData, sizeof m_tData);
	}

	_bool* Get_Ready()
	{
		return &m_bReady;
	}


	static const char* szStateKeyTag[TYPE_END];


protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	SaveData m_tData;
	_bool	m_bReady = true;
	_double m_ShotFrame = 0.0;

public:
	virtual void Free() override;
};

END