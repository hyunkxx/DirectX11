#pragma once

#include "Level.h"

BEGIN(Client)

class CLevel_AnimTool final : public CLevel
{
private:
	explicit CLevel_AnimTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_AnimTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void RenderLevelUI() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);

public:
	static CLevel_AnimTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;


	std::thread thread1;
	int m_iThreadTest = 0;
};

END