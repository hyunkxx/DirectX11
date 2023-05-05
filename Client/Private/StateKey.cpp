#include "stdafx.h"
#include "..\Public\StateKey.h"

const char* CStateKey::szStateKeyTag[TYPE_END] =
{
	"EffectKey",
	"PartsKey",
	"PriorityKey",
	"DissolveKey",
	"OBBKey",
	"MissileKey",
	"SoundKey",
};

CStateKey::CStateKey(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CStateKey::Initialize(BaseData* pData)
{
	m_ShotFrame = pData->ShotFrame;
	m_bReady = true;

#ifdef _DEBUG
	memcpy(&m_tData, pData, sizeof BaseData);
#endif

	return S_OK;
}

void CStateKey::Check(_double FrameAcc, CCharacter * pCharacter)
{
	if (true == m_bReady)
	{
		if (FrameAcc > m_ShotFrame)
		{
			// 자식 Key의 Override된 Shot함수를 애니메이션 실행한 객체를 대상으로 호출
			Shot(pCharacter);
			m_bReady = false;
		}
	}
}

void CStateKey::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
