#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CItem : public CGameObject
{
public:
	enum ITEM_TYPE { ITEM_MATERIAL, ITEM_COOK, ITEM_EQUITMENT };

	typedef struct tagItemDesc
	{
		ITEM_TYPE			eItemType;
		_uint				iImageIndex;
		_tchar				szTag[MAX_TAG];

	}ITEM_DESC;

public:
	explicit CItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

private:
	HRESULT addComponents();

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;

private:
	ITEM_DESC m_ItemDesc;
	
};

END