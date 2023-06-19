#pragma once

#include "Camera.h"
#include "CameraCurve.h"
#include "ActionCam.h"
#include "PlayerState.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
END


BEGIN(Client)

class CPlayerCamera final
	: public CCamera
	, public IAttachTargetTransform
{
public:
	typedef struct tagPlayerCameraDesc
	{
		CCamera::CAMERA_DESC CamDesc;
		CTransform* pPlayerTransform;
		_bool*	pLockOn;
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
	virtual void RenderGUI() override;
	virtual void AttachTargetTransform(CTransform * pTransform) override;

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

	void SoftLerp();

	void Reset_ActiveCharacter(class CCharacter* pCharacter);
	void Set_Behind();
private:
	_bool		m_bFixMouse = { false };
	CTransform* m_pPlayerTransform = { nullptr };
	CNavigation* m_pPlayerNavigation = { nullptr };

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

	// 전 프레임에 타겟 카메라 였는지
	_bool	m_bPreLockOn = { false };
	_bool	m_bPreAiming = { false };
	_double	m_SoftLerpDuration = { 0.0 };
	_float3 m_SoftLerpAt = {};
	_float3 m_SoftLerpEye = {};
	_float	m_fOldFov = {};
	_float	m_fTargetFov = {};


	// 카메라 커브용 변수
	class CCameraCurve* m_pCamCurve = { nullptr };
	_uint m_iCurKey = { 0 };
	_double m_FrameAcc = { 0.0 };

	// 상태 분기용 변수
	_bool m_bApplyCurve = { false };

	const _float m_fDistanceMin = { 1.f };
	const _float m_fDistanceMax = { 7.f };

	const _float m_fXAngleMin = { -20.f };
	const _float m_fXAngleMax = { 80.f };

	CPlayerState* m_pPlayerStateClass;
	

#ifdef _DEBUG
private:
	_bool m_bShakeToggle = false;
	CCamera::SHAKE_DESC m_ShakeDesc;
	_float m_fVibeRange = 5.f;
	_float m_fVibeDuration = 0.8f;

private: /* 임시 마우스 고정 헤지용 변수 */
	_bool m_IsMouse_UnLock = { false };
#endif

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	class CTerminalUI* m_pTerminalUI = nullptr;
};

END