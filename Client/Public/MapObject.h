#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel_Instance;
class CTexture;
END

BEGIN(Client)

class CMapObject : public CGameObject
{
public:
	enum MAPOBJECT_TYPEID { ID_TREE, ID_ROCK, ID_FLOOR, ID_STAIRS, ID_GRASS, ID_GRASS_MASK,
		ID_VIN, ID_VEG, ID_SHR, ID_STRUCTURE, ID_PIL, ID_STATUE, ID_NOIROCK, ID_TOF_GRASS, ID_END };

	enum ROCK_DIFFUSE_KINDS { RD_1, RD_2, RD_3, RD_4, RD_END };

	enum GRASS_MASK_KINDS { GM_1, GM_2, GM_3, GM_4, GM_5, GM_6, GM_END };

private:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

public:
	HRESULT Render_Rock();

	HRESULT Render_SubEditionColor_Mask();

	HRESULT Render_Default();
	HRESULT Render_Default_SelfShadow();

public:
	void SetUp_LevelFilePath(_uint iLevelID, const _tchar* pFilePath, _Out_ _tchar& szResultFilePath);

private:
	_tchar						m_szEdition_FilePath[MAX_PATH] = { L"" };

public:
	HRESULT Load_Edition();

	void Load_EditionID(HANDLE& _hFile, DWORD& _dwByte);

	void Load_EditionColor(HANDLE& _hFile, DWORD& _dwByte);
	void Load_DiffuseTexID(HANDLE& _hFile, DWORD& _dwByte);
	void Load_SubEditionColor_Mask(HANDLE& _hFile, DWORD& _dwByte);

private:
	SMAP_OBJECT_EDITION_DESC	m_EditionDesc = {};

	_uint						m_iShaderPassID = { 0 };

	_bool						m_IsUse_EdtionColor = { false };

private:
	CRenderer*					m_pRendererCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CModel_Instance*			m_pModelCom = { nullptr };

	CTexture*					m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_END] = { nullptr };

	CTexture*					m_pGrassMaskTexture[GRASS_MASK_KINDS::GM_END] = { nullptr };
	
private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();
	HRESULT	SetUp_ShadowShaderResources();

public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
