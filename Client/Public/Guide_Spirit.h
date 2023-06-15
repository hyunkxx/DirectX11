#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CEffect;
END

BEGIN(Client)

class CGuide_Spirit final : public CGameObject
{
public:
	enum SPIRIT_STATE
	{
		STATE_WAIT , STATE_GUIDE , STATE_RECALL , STATE_STOP , STATE_END
	};

private:
	CGuide_Spirit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGuide_Spirit(const CGuide_Spirit& rhs);
	virtual ~CGuide_Spirit() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	void	Wait_Tick(_double TimeDelta);
	void	Guide_Tick(_double TimeDelta);
	void	ReCall_Tick(_double TimeDelta);
	void	Stop_Tick(_double TimeDelta);

	void	Particle(double TimeDelta);

private:
	CEffect*	m_pEffect = { nullptr };
	
	class CPlayerState* m_pPlayerState = { nullptr };
	class CCharacter* m_pTarget = { nullptr };
	
	_float3	m_vPosition;
	_float3 m_vDestination;

	_float  m_fSpeed = { 5.f };

	SPIRIT_STATE m_eSpiritState = { STATE_WAIT };
	_int		 m_iDestinationCount = { 0 };
	_float		 m_fStopCount = { 0.f };
	_float		 m_fParticle_Count = { 0.f };
	_bool		 m_bBoxAppear = { false };

	CGameObject* m_pMyBox = { nullptr };
	_tchar		 m_BoxPath[MAX_PATH] = { L"" };

	vector<_float3*>* m_pRouteList = { nullptr };

protected:
	HRESULT Add_Components();

public:
	static CGuide_Spirit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};



END