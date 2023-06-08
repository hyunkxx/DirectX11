#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_Player final : public CEffect
{
protected:
	CEffect_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Player(const CEffect_Player& rhs);
	virtual ~CEffect_Player() = default;

public:
	virtual	HRESULT				Initialize_Prototype(const char* FilePath , const list<EFFECT_DESC*>& pEffectDescList);
	virtual	HRESULT				Initialize(void* pArg)override;
	virtual void				Tick(_double TimeDelta)override;
	virtual void				LateTick(_double TimeDelta)override;
	virtual HRESULT				Render()override;
	virtual void				RenderGUI() override;

	virtual void				Play_Effect(_float4x4* pWorldMatrix, _bool bTracking = false)override;
	virtual EFFECT_DESC			Get_Effect_Desc()override { return m_EffectDesc; }

	list<EFFECT_DESC*>*			Get_Effects();

	_float						Get_LifeTime() { return m_LifeTime; }
public:
	void	Add_Effect(CEffect* pEffect);

private:
	vector<CEffect*>			m_EffectList;

	_float						m_LifeTime = { 0.f };
	_float						m_LifeAcc = { 0.f };

public:
	static CEffect_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , const char* FilePath  ,const list<EFFECT_DESC*>& pEffectDescList);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free()override;

};

END
