#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CBone;

END

BEGIN(Client)

class CUIWeapon final : public CGameObject 
{
public:
	enum PROP_TYPE { SWORD1, SWORD2, SWORD3, SWORD4, SWORD5,
		SWORD1_SUB, SWORD2_SUB, SWORD3_SUB, SWORD4_SUB, SWORD5_SUB,
		GUN1, GUN2, GUN3, GUN4, GUN5, PROB_END };

private:
	explicit CUIWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUIWeapon(const CUIWeapon& rhs);
	virtual ~CUIWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void SetupWeaponProp(_uint iCharacter, _uint iWeaponPropID, CBone * pBone);
	void SetupParentTransform(CTransform* pTransform) { m_pParentTransform = pTransform; };

	void SetCurPlayer(_uint iCurPlayer) { m_iCurPlayer = iCurPlayer; };
	void SetWeaponState(_bool value) { m_bOnHand = value; }
	void SetDissolveAmount(_float fValue) { m_fDissolveAmount = fValue; }
private:
	HRESULT addComponents();
	HRESULT setupShaderResources();

	HRESULT renderWeapon();

public:
	static CUIWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;

private:
	class CPlayerState* m_pPlayerState = nullptr;
	CTransform* m_pParentTransform = nullptr;

private:

	enum { ROVER, YANGYANG, CHIXIA, CHARACTER_END };
	enum { BONE_PROP01, BONE_PROP02, BONE_PROP03, BONE_PROP04, BONE_PROP_END};
	CBone* m_pPropBone[4];
	CModel* m_pModels[15];

	_uint m_iWeaponModel = 0;
		
private:
	_uint m_iCurPlayer = 0;
	_float4x4 m_WorldMatrix[4];

	_bool m_bOnHand = false;
	_bool m_bDessolve = false;
	_float m_fDissolveAmount = 0.f;
};

END