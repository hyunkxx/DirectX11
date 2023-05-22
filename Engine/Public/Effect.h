#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CEffect abstract : public CGameObject
{
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual void Play_Effect(_float4x4* pWorldMatrix , _bool bTracking = false) = 0;
	virtual EFFECT_DESC Get_Effect_Desc() = 0;

public:
	_float Get_CamDistance() const {
		return m_fCameraDistance;
	}

	_bool Get_Finish() { return m_bFinish; }
	void Set_Edit();

protected:
	void Compute_CamDistance(_fvector vWorldPos);

public:
	const _bool	Get_UpdateOK() const { return m_bEffectUpdate; }
	void	Set_UpdateOK(_bool bUpdateOK) { m_bEffectUpdate = bUpdateOK; }
protected:
	EFFECT_DESC m_EffectDesc;

	_bool	m_bEffectUpdate = { false };
	_float	m_fCameraDistance = { 0.f };
	_bool	m_bEDit = { false };
	_bool	m_bFinish = { true };

	char		m_FileName[BUFFER_END][1000][MAX_PATH];
	_int		m_iFileCount[BUFFER_END] = { 0 , 0 };

	_float4x4					m_ParentsMatrix;
	_float4x4*					m_pParentsMatrix = { nullptr };

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void Free()override;
};

END