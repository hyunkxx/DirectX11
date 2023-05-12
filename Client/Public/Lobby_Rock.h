#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client);

class CLobby_Rock final : public CGameObject
{
public:
	enum ROCK_DIFFUSE_KINDS { RD_1, RD_2, RD_3, RD_4, RD_END };

	enum LOBBYROCK_MODELTYPE
	{
		LOBBYROCK_0,
		LOBBYROCK_1,
		LOBBYROCK_2,
		LOBBYROCK_3,
		LOBBYROCK_4,
		LOBBYROCK_5,
		LOBBYROCK_6,
		LOBBYROCK_END,
	};



protected:
	CLobby_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLobby_Rock(const CLobby_Rock& rhs);
	virtual ~CLobby_Rock() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetUp_State();

public:
	HRESULT ModelType_Detection();

private:
	_uint					m_iShaderPass_ID = { 0 };

	SMAP_OBJECT_DESC		m_SMap_Obejct_Desc = {};

	LOBBYROCK_MODELTYPE		m_eLobbyRock_ModelType = { LOBBYROCK_END };
	_uint					m_iDiffuseTex_ID = { 0 };

private:
	CRenderer*				m_pRendererCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CTexture*				m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_END] = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CLobby_Rock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END