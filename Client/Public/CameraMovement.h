#pragma once

#include "GameObject.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
class CBone;

END

BEGIN(Client)

class CCameraMovement final : public CGameObject
{
public:
	enum CAM_TYPE { CAM_DYNAMIC, CAM_MAINPLAYER, CAM_BANGSUN, CAM_END };

private:
	CCameraMovement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraMovement(const CCameraMovement& rhs);
	virtual ~CCameraMovement() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	//virtual HRESULT Render();
	virtual void RenderGUI() override;

public:
	HRESULT BindTransform(CTransform* pTransform);

	HRESULT AddCamera(CAM_TYPE eCameraType, CCamera* pCamera);
	void UseCamera(CAM_TYPE eCameraType);
	void SetupBone(CAM_TYPE eCameraType, CBone* pBone);

public:
	void StartWave();
	void StartWave(CCamera::SHAKE_DESC tagShakeDesc);
	void StartVibration();
	void StartVibration(_float fRange, _float fDuration = 0.5f);

	CCamera* GetCamera(CAM_TYPE eCameraType) {
		if (!m_pCams[eCameraType])
			return nullptr;

		return m_pCams[eCameraType];
	};

	void SetCamMatrix(CAM_TYPE eCameraType, _fmatrix CamMatrix)	{
		if (!m_pCams[eCameraType])
			return;

		m_pCams[eCameraType]->SetCamMatrix(CamMatrix);
	}

	void SetCamPosition(CAM_TYPE eCameraType, _fvector vEye) {
		if (!m_pCams[eCameraType])
			return;

		m_pCams[eCameraType]->SetCamPosition(vEye);
	}


private:
	HRESULT addComponents();

public:
	static CCameraMovement* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pTargetTransform = nullptr;

private:
	CAM_TYPE m_eCurCamType = CAM_END;
	CCamera* m_pCams[CAM_END] = { nullptr, };

};

END