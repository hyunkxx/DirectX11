#pragma once

#include "Base.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)

class CEfffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEfffect_Manager)

private:
	explicit CEfffect_Manager();
	virtual ~CEfffect_Manager() = default;


public:
	HRESULT				Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void				Tick(_double TimeDelta);
	void				LateTick(_double TimeDelta);
	HRESULT				Render();
	void				RenderGUI();

private:
	void				Effect_List();
	void				Parts_List();
	void				Create_Effect();

	void				Play_Effect();

	void				Copy_Effect();
private:
	_float4x4			m_WorldMatrix;
	_bool				m_bLoadEffect = { false };
	_bool				m_bLoadEffect2 = { false };

	_int				m_iNumInstance = { 1 };

	_int				m_PushID = { 1000 };
	_int				m_iMapCount = { 0 };

#define PUSHID ImGui::PushID(m_PushID++);
#define POPID ImGui::PopID();

	_float				m_fLifeTime = { 1.f };
	_float				m_fTimeAcc = { 0.f };
	_bool				m_bPlay = { false };

	char				m_LifeTimeTex[MAX_PATH] = { "" };
	char				m_EffectAccTex[MAX_PATH] = { "" };

	char				m_EffectName[MAX_PATH] = {""};
private:
	CEffect*			m_pEffect = { nullptr };
	
	typedef unordered_map<char*, CEffect*> EFFECTS;
	EFFECTS*  m_pEffects = { nullptr };
	EFFECTS	m_LoadEffectList;
	CEffect* m_pEffectPlayer = { nullptr };

	char		m_Effect_ListName[100][MAX_PATH];
	char		m_ListCur_Select[MAX_PATH] = "";
	char		m_ListPrev_Select[MAX_PATH] = "";

	_int		m_iListCount = { 0 };


	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_float3	m_vStartPos = { 0.f , 0.f ,0.f };
	_float3	m_vEndPos = { 0.f , 0.f ,0.f };


	_bool	m_bRender = { false };
private:
	void Load_EffectList(const _tchar* pPathName);

public:
	virtual void Free()override;
};

END