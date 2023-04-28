#pragma once

#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CTransform;
class CRenderer;
class CShader;

class CCollider;
class CSphereCollider;
class COBBCollider;
END

BEGIN(Client)

class CCharacter
	: public CGameObject
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
{
public:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	HRESULT Setup_ShaderResource();
	HRESULT Add_Components();
	
public:
	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;

	//TestCode
	int m_iObjectCount = 0;

	CCollider* m_pMainCollider = nullptr;

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;
};

END