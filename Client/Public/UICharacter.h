#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;

class CModel_Anim;
class CModel_VTF;

END

BEGIN(Client)

class UICharacter final : public CGameObject
{
private:
	explicit UICharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit UICharacter(const UICharacter& rhs);
	virtual ~UICharacter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

private:
	void initAnimation();
	void updateAnimationState(_double TimeDelta);

	HRESULT addComponents();
	HRESULT setupShaderResource();

public:
	static UICharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;
	CModel_VTF*	m_pModel = nullptr;
	CModel_Anim* m_pAnimSetBase = nullptr;
	CModel_Anim* m_pAnimSetRibbon = nullptr;

};

END