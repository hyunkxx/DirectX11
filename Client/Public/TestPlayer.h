#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CModel;
END

BEGIN(Client)

class CTestPlayer : public CGameObject
{
public:
	enum AnimSet
	{
		ANIMSET_BASE,
		ANIMSET_WEAPON1,
		ANIMSET_WEAPON2,
		ANIMSET_WEAPON3,
		ANIMSET_END
	};

	enum SkelParts
	{
		SKEL_BODY,
		SKEL_FACE,
		SKEL_EYE,
		SKEL_EYEBROW,
		SKEL_EYELASH,
		SKEL_SUPPRESSOR,
		SKEL_END
	};

	enum HairParts
	{
		HAIR_ALL,
		HAIR_FRONT,
		HAIR_BACK,
		HAIR_END
	};

	enum StaticParts
	{
		STATIC_HEADWEAR,
		STATIC_END
	};

	enum Shader
	{
		SHADER_MODEL,
		SHADER_MODELANIM,
		SHADER_END
	};

private:
	CTestPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestPlayer(const CTestPlayer& rhs);
	virtual ~CTestPlayer() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom[SHADER_END] = { nullptr };

	// Models
private:
	CModel_Anim*		m_pAnimSet = { nullptr };
	CModel_Anim*		m_pSkelParts[SKEL_END] = { nullptr, };
	CModel_Anim*		m_pHairParts[HAIR_END] = { nullptr, };
	CModel*				m_pStaticParts[STATIC_END] = { nullptr, };

private:
	_uint				m_iAnimSetID = { 0 };
	_uint				m_iSkelAnimID = { 0 };
	_uint				m_iHairAnimID = { 0 };

private:
	HRESULT Add_Components();
	void Init_Models();
	void SetUp_SkelAnimation();
	void SetUp_HairAnimation();
	void Tick_State(_double TimeDelta);

	HRESULT	SetUp_ShaderResources();

public:
	static CTestPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CGameObject* pStaticObject = nullptr;
};

END