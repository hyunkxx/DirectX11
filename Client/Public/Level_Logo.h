#pragma once

#include "Level.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Character(const _tchar* pLayerTag);
	HRESULT Ready_StaticGameObject(const _tchar* pLayerTag);
private:
	HRESULT Load_LobbyRock(const _tchar* pLobbyRockFilePath, const _tchar * pLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

	CGameObject* m_pIntroCam = nullptr;
};

END