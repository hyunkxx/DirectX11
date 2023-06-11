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
	enum PROB_TYPE { SWORD1, SWORD2, SWORD3, SWORD4, SWORD5, 
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
	void SetupWeaponProb(_uint iCharacter, _uint iWeaponProbID, CBone ** ppBone);

private:
	HRESULT addGameObject();

public:
	static CUIWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;

	enum { ROVER, YANGYANG, CHIXIA, CHARACTER_END };
	enum { WEAPON_PROB01, WEAPON_PROB02, WEAPON_PROB_END };
	CBone** m_pWeaponProb[CHARACTER_END][WEAPON_PROB_END];

	CModel* m_pModels[PROB_END];

private:
	PROB_TYPE m_eCurWeapon = PROB_END;

};

END