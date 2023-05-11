#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CShader;
class CModel;
END

BEGIN(Client);

class CSky final : public CGameObject
{
public:
	enum SKY_TYPE { DEFAULT, LOBBY };

protected:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual	HRESULT Initialize_Prototype(SKY_TYPE eSkyType);
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Correction_Y_SkyPos();

private:
	_uint					m_iShaderPass_ID = { 0 };

	_float4					m_vCamPos = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float4					m_vOriginCamPos = { 0.0f, 0.0f, 0.0f, 0.0f };

	_float					m_fCorrectionY = { 0.0f };

private:
	SKY_TYPE				m_eType = SKY_TYPE::DEFAULT;
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, SKY_TYPE eType = SKY_TYPE::DEFAULT);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END