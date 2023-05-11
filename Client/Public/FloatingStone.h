#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;

END

BEGIN(Client)

class CFloatingStone final : public CGameObject
{
private:
	explicit CFloatingStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFloatingStone(const CFloatingStone& rhs);
	virtual ~CFloatingStone() = default;
	virtual void Free() override;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

private:
	void floatingPosition(_double TimeDelta);

public:
	static CFloatingStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;

private:
	CRenderer* m_pRenderer = nullptr;
	CShader* m_pShader = nullptr;
	CModel* m_pModel = nullptr;
	CModel* m_pModel_LOD = nullptr;

private:
	_float m_fTimeAcc = 0.f;
	_float m_fStartTimeAcc = 0.f;
	_float m_fStartTimeLimit = 0.f;

};

END