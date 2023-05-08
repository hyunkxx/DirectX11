#pragma once
#include "Client_Defines.h"
#include "Camera.h"
#include "CameraCurve.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CPlayerCamera final : public CCamera
{
public:
	typedef struct tagPlayerCameraDesc
	{
		CCamera::CAMERA_DESC CamDesc;
		CTransform* pPlayerTransform;
		_float  fXAngle;
		_float	fDistance;
		_float3 vDir;
	}PLAYERCAM_DESC;

private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual ~CPlayerCamera() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	//virtual HRESULT Render();

	void Start_Curve(class CCameraCurve* pCurve)
	{
		m_bApplyCurve = true;
		m_iCurKey = 0;
		m_FrameAcc = 0.0;
		m_pCamCurve = pCurve;
	}

	void End_Curve()
	{
		m_bApplyCurve = false;
		//m_iCurKey = 0;
		//m_TimeAcc = 0.0;
		m_pCamCurve = nullptr;

		m_fXTargetAngle = m_fXCurAngle;
		m_fYTargetAngle = m_fYCurAngle;
		m_fTargetDistance = m_fCurDistance;

		XMStoreFloat3(&m_vDir, m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));

	}

private:
	_bool		m_bFixMouse = { false };
	CTransform* m_pPlayerTransform = { nullptr };

	_float3 m_vTargetPrevPos = {};
	_float m_fMaxPosDistance = { 1.5f };

	_float m_fXTargetAngle = { 0.f };
	_float m_fYTargetAngle = { 0.f };
	_float m_fTargetDistance = { 0.f };

	_float m_fXCurAngle = { 0.f };
	_float m_fYCurAngle = { 0.f };
	_float m_fCurDistance = { 0.f };
	_float m_fPinHeight = { 0.7f };

	_float3 m_vDir = {};

	// 카메라 커브용 변수
	class CCameraCurve* m_pCamCurve = { nullptr };
	_uint m_iCurKey = { 0 };
	_double m_FrameAcc = { 0.0 };

	// 상태 분기용 변수
	_bool m_bApplyCurve = { false };
	_bool m_bLockOn = { false };

	const _float m_fDistanceMin = { 2.f };
	const _float m_fDistanceMax = { 7.f };

	const _float m_fXAngleMin = { -20.f };
	const _float m_fXAngleMax = { 80.f };


public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
};

END