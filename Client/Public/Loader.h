#pragma once

#include "Base.h"

BEGIN(Client)

class CLoader : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL_ID eNextLevel);
	HRESULT Load_Level_Logo();
	HRESULT Load_Level_GamePlay();
	HRESULT Load_Level_City();
	HRESULT Load_Level_Forest();
	HRESULT Load_Level_Crown();
	
	HRESULT Load_Level_AnimTool();
	HRESULT Load_Level_Test();


private:
	HRESULT LoadCharacters(_uint iLevel);
	HRESULT LoadCharacterProps(_uint iLevel);

private:
	HRESULT Load_Prototype_GameObject();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel);
	virtual void Free() override;

public:
	CRITICAL_SECTION* Get_CriticalSectionPtr() { return &m_hCriticalSection; }
	LEVEL_ID Get_NextLevel() const { return m_eNextLevel; }
	wstring GetLoadingStateText() const { return m_szLoadingStateText; }
	_bool IsFinished() { m_Wait = 0; return m_isFinish; }

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
							
private:
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_hCriticalSection;
	LEVEL_ID				m_eNextLevel;

	_bool					m_isFinish = { false };
	wstring					m_szLoadingStateText;

private:
	class CAppManager*		m_pApp = nullptr;
	_uint					m_Wait = 0;
	_uint					m_Start = 0;

private:
	HRESULT Load_MapResource_Logo();
	HRESULT Load_MapResource_GamePlay();
	HRESULT Load_MapResource_City();
	HRESULT Load_MapResource_Forest();
	HRESULT Load_MapResource_Crown();

};

END