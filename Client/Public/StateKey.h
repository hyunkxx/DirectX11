#pragma once

#include "Base.h"
#include "Client_Defines.h"


BEGIN(Client)

class CStateKey abstract : public CBase
{
public:
	typedef enum KeyType {
		TYPE_EFFECT,
		TYPE_PARTS,
		TYPE_PRIORITY,
		TYPE_DISSOLVE,
		TYPE_OBB,
		TYPE_MISSILE,
		TYPE_SOUND,
		TYPE_END
	}TYPE;

	struct BaseData
	{
		// 필수 요소
		_int iType;
		_float ShotFrame;
		// 이하 일부에서만 쓰이는 선택요소
		_int	iInt0;
		_int	iInt1;
		_int	iInt2;
		_float fFloat;
		char szTag[MAX_PATH];
	};
protected:
	CStateKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStateKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Check(_double FrameAcc, class CCharacter* pCharacter);
	virtual void Shot(class CCharacter* pCharacter) PURE;

	void Reset() { m_bReady = true; }

	_bool* Get_Ready()
	{
		return &m_bReady;
	}

//#ifdef _DEBUG
	BaseData* Get_Data()
	{
		return &m_tData;
	}

	void Set_Data(BaseData* pData)
	{
		memcpy(&m_tData, pData, sizeof m_tData);
	}
//#endif

	static const char* szStateKeyTag[TYPE_END];

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	_bool	m_bReady = true;
	_double m_ShotFrame = 0.0;
//#ifdef _DEBUG
	BaseData m_tData;
//#endif
public:
	virtual void Free() override;
};

END