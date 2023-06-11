#include "stdafx.h"
#include "..\Public\PlayerCamera.h"

#include "GameMode.h"
#include "GameInstance.h"

#include "CameraMovement.h"
#include "TerminalUI.h"
#include "UI_TapT.h"
#include "UI_Tip.h"
#include "UI_MerchantMen.h"
#include "UI_Souvenir.h"
#include "UI_Panhua.h"
#include "Character.h"

CPlayerCamera::CPlayerCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CPlayerCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerCamera::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	PLAYERCAM_DESC PlayerCamDesc = *((PLAYERCAM_DESC*)pArg);

	memcpy(&m_CameraDesc, &PlayerCamDesc.CamDesc, sizeof(CAMERA_DESC));

	m_fXTargetAngle = PlayerCamDesc.fXAngle;
	m_fXCurAngle = m_fXTargetAngle;

	m_fYTargetAngle = 0.f;
	m_fYCurAngle = m_fYTargetAngle;

	m_fTargetDistance = PlayerCamDesc.fDistance;
	m_fCurDistance = m_fTargetDistance;

	XMStoreFloat3(&m_vDir, XMVectorSet(0.f, 0.f, -1.f, 0.f));

	m_bFixMouse = true;

	m_pMainTransform = CTransform::Create(m_pDevice, m_pContext);
	m_pMainTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	// 플레이어 스테이트 세팅
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pPlayerStateClass->Set_PlayerCamera(this);

#ifdef _DEBUG
	m_ShakeDesc.fPower = 2.f;
	m_ShakeDesc.fSpeed = 10.f;
	m_ShakeDesc.fDuration = 2.f;
#endif

	//if (__super::Initialize(pArg))
	//	return E_FAIL;

	return S_OK;
}

void CPlayerCamera::Start()
{
	// 플레이어 Transform 가져옴
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	m_pPlayerTransform = m_pPlayerStateClass->Get_ActiveCharacter()->GetTransform();
	m_pPlayerNavigation = static_cast<CNavigation*>(m_pPlayerStateClass->Get_ActiveCharacter()->Find_Component(TEXT("Com_Navigation")));

	if (nullptr == m_pPlayerTransform || nullptr == m_pPlayerNavigation)
		return;

	// 초기 위치로 1회 갱신해 놓음
	_vector vAxisRight = XMVector3Cross(XMLoadFloat3(&m_vDir), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_matrix matRotX = XMMatrixRotationAxis(vAxisRight, XMConvertToRadians(m_fXCurAngle));
	_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(m_fYCurAngle));
	_vector vCamLook = XMVector3Normalize(-XMVector3TransformNormal(XMVector3TransformNormal(XMLoadFloat3(&m_vDir), matRotX), matRotY));
	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vEyePos = vPlayerPos - vCamLook * m_fCurDistance;
	XMStoreFloat3(&m_CameraDesc.vEye, vEyePos);
	XMStoreFloat3(&m_CameraDesc.vAt, vEyePos + vCamLook * m_fCurDistance);

	m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
	m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
	

	CCameraMovement* pCamMovement = nullptr;
	pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	pCamMovement->AddCamera(CCameraMovement::CAM_MAINPLAYER, this);
	pCamMovement->BindCamera(CCameraMovement::CAM_MAINPLAYER, this);
	pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);

	m_pTerminalUI = static_cast<CTerminalUI*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Terminal"));
	m_pUITap = static_cast<CUI_TapT*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_TapT"));
	m_pUITip = static_cast<CUI_Tip*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Tip"));
	m_pUIMen = static_cast<CUI_MerchantMen*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_MerchantMen"));
	m_pUISovi = static_cast<CUI_Souvenir*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Souvenir"));
	m_pUIPanhua = static_cast<CUI_Panhua*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Panhua"));
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (0.0 < m_SoftLerpDuration)
	{
		m_SoftLerpDuration -= TimeDelta;
		if (0.0 > m_SoftLerpDuration)
			m_SoftLerpDuration = 0.0;
	}

	// CameraCurve
	if (false == m_bApplyCurve)
	{

		m_bFixMouse = m_pTerminalUI->IsActive() ? false : true;
		/*if (false == m_pTerminalUI->IsActive())
		{
			if (nullptr == m_pUITap || nullptr == m_pUITip || nullptr == m_pUIMen || nullptr == m_pUISovi || m_pUIPanhua)
				return;

			m_bFixMouse = (m_pUITap->IsMouseActive() ? false : m_pUITip->IsMouseActive() ? false : m_pUIMen->IsMouseActive() ? false : m_pUISovi ->IsMouseActive() ? false : m_pUIPanhua->IsMouseActive() ? false : true);
		}
		else
		{
			if (nullptr == m_pUITap || nullptr == m_pUITip || nullptr == m_pUIMen || nullptr == m_pUISovi || m_pUIPanhua)
				return;
			m_pUITip->SetState(DISABLE);
		}*/

		if (pGameInstance->InputKey(DIK_LALT) == KEY_STATE::HOLD)
		{
			m_bFixMouse = false;
		}
		
		/* 임시 마우스 고정 혜지 용 */
		if (pGameInstance->InputKey(DIK_NUMLOCK) == KEY_STATE::TAP)
		{
			if (false == m_IsMouse_UnLock)
				m_IsMouse_UnLock = true;
			else if (true == m_IsMouse_UnLock)
				m_IsMouse_UnLock = false;
		}
		if (true == m_IsMouse_UnLock)
			m_bFixMouse = false;



		if (true == m_pPlayerStateClass->Get_Aiming())
		{
			if (false == m_bPreAiming)
			{
				//_vector vLookMinus = m_pPlayerTransform->Get_State()
				_vector vPreAt = XMLoadFloat3(&m_CameraDesc.vAt);
				_vector vPreEye = XMLoadFloat3(&m_CameraDesc.vEye);

				_vector vPreCamDir = vPreEye - vPreAt;

				_vector vDefaultDir = XMLoadFloat3(&m_vDir);

				m_fCurDistance = XMVectorGetX(XMVector3Length(vPreCamDir));

				if (0.f > XMVectorGetY(XMVector3Cross(vDefaultDir, XMVectorSetY(vPreCamDir, 0.f))))
					m_fYCurAngle = -XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetY(vPreCamDir, 0.f))))));
				else
					m_fYCurAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetY(vPreCamDir, 0.f))))));


				m_fXCurAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetX(vPreCamDir, 0.f))))));
				if (m_fXCurAngle > 90.f)
					m_fXCurAngle = 180 - m_fXCurAngle;

				m_fTargetDistance = 4.f;
				m_fXTargetAngle = m_fXCurAngle * 0.5f;
				m_fYTargetAngle = m_fYCurAngle;

				m_fOldFov = m_CameraDesc.fFovy;
				m_fTargetFov = XMConvertToRadians(45.f);
				m_SoftLerpDuration = 0.3;
			}

			if (m_bFixMouse)
			{
				_long MouseMove = 0;

				// 플레이어 기준 x축 회전
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_Y))
				{
					m_fXTargetAngle += (_float)TimeDelta * MouseMove * 4.f * 2.f;

					// x축 회전각 제한
					if (m_fXTargetAngle > m_fXAngleMax)
						m_fXTargetAngle = m_fXAngleMax;
					else if (m_fXTargetAngle < m_fXAngleMin)
						m_fXTargetAngle = m_fXAngleMin;
				}
				// 현재 각도, 목표 각도 값 보간
				if (m_fXCurAngle != m_fXTargetAngle)
				{
					_float fGap = m_fXTargetAngle - m_fXCurAngle;

					if (fabs(fGap) < 0.1f)
						m_fXCurAngle = m_fXTargetAngle;
					else
						m_fXCurAngle += fGap * (_float)TimeDelta * 20.f;

				}

				// 플레이어 기준 Y축 회전
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_X))
					m_fYTargetAngle += (_float)TimeDelta * MouseMove * 4.f * 2.f;
				// 현재 각도, 목표 각도 값 보간
				if (m_fYCurAngle != m_fYTargetAngle)
				{
					_float fGap = m_fYTargetAngle - m_fYCurAngle;

					if (fabs(fGap) < 0.1f)
						m_fYCurAngle = m_fYTargetAngle;
					else
						m_fYCurAngle += fGap * (_float)TimeDelta * 20.f;
				}

				// 줌 인/아웃
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_WHEEL))
				{
					m_fTargetDistance -= (_float)TimeDelta * MouseMove * 0.4f;
				}

				// 거리 제한
				if (m_fTargetDistance > m_fDistanceMax)
					m_fTargetDistance = m_fDistanceMax;
				else if (m_fTargetDistance < m_fDistanceMin)
					m_fTargetDistance = m_fDistanceMin;

				// 현재 거리, 목표 거리 값 보간

				if (m_fCurDistance != m_fTargetDistance)
				{
					_float fGap = m_fTargetDistance - m_fCurDistance;

					if (fabs(fGap) < 0.05f)
						m_fCurDistance = m_fTargetDistance;
					else
					{
						m_fCurDistance += fGap * (_float)TimeDelta * 20.f;
					}
				}

				POINT	Center = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
				ClientToScreen(g_hWnd, &Center);
				SetCursorPos(Center.x, Center.y);
			}

			_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);


			_vector vAxisRight = XMVector3Cross(XMLoadFloat3(&m_vDir), XMVectorSet(0.f, 1.f, 0.f, 0.f));
			_matrix matRotX = XMMatrixRotationAxis(vAxisRight, XMConvertToRadians(m_fXCurAngle));
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(m_fYCurAngle));
			_vector vCamLook = XMVector3Normalize(-XMVector3TransformNormal(XMVector3TransformNormal(XMLoadFloat3(&m_vDir), matRotX), matRotY));
			_vector vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook));


			_vector vEyePosTemp = vPlayerPos + vCamRight * (1.f) + XMVectorSet(0.f, 1.5f, 0.f, 0.f);

			
			_vector vEyePos = vEyePosTemp - vCamLook * 2.5f;
			_vector vAtPos = vEyePos + vCamLook * 4.f;
			
			_vector vCurAt, vCurEye;

			if (0.0 == m_SoftLerpDuration)
			{
				vCurAt = vAtPos;
				vCurEye = vEyePos;
				m_CameraDesc.fFovy = m_fTargetFov;
			}
			else
			{
				_float fRatio = _float(m_SoftLerpDuration / 0.5);

				_vector vPrevAt = XMLoadFloat3(&m_CameraDesc.vAt);
				_vector vPrevEye = XMLoadFloat3(&m_CameraDesc.vEye);

				vCurAt = XMVectorLerp(vPrevAt, vAtPos, fRatio);
				vCurEye = XMVectorLerp(vPrevEye, vEyePos, fRatio);
				m_CameraDesc.fFovy = m_fOldFov * fRatio + m_fTargetFov * (1 - fRatio);
			}

			XMStoreFloat3(&m_CameraDesc.vAt, vCurAt);
			XMStoreFloat3(&m_CameraDesc.vEye, vCurEye);

			m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
			m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

		}
		else if (true == m_pPlayerStateClass->Get_LockOn())
		{
			if (m_bFixMouse)
			{
				POINT	Center = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
				ClientToScreen(g_hWnd, &Center);
				SetCursorPos(Center.x, Center.y);
			}

			CCharacter* pTarget = m_pPlayerStateClass->Get_LockOnTarget();
			_vector vTargetPos = pTarget->Get_Position();
			_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vPlayerPos));
			_float fDistRate = fDistance / 25.f;

			_vector vTargetDir = vTargetPos - vPlayerPos;

			_vector vTargetDirLook = XMVector3Normalize(XMVectorSetY(vTargetDir, 0.f));
			_vector vTargetDirRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vTargetDirLook));
			//_vector vTargetDirUp = XMVector3Normalize(XMVector3Cross(vTargetDirLook, vTargetDirRight));


			_float fLookDist = -3.5f - fDistRate * 1.5f;
			_float fUpDist = 1.3f - min(max(XMVectorGetY(vTargetDir), -2.f), 0.f);
			_float fRightDist = 1.2f + fDistance * 0.12f;

			_vector vEyePos = vPlayerPos + vTargetDirLook * fLookDist + vTargetDirRight * fRightDist + XMVectorSet(0.f, fUpDist, 0.f, 0.f);

			// 거리가 0.f일 때 7:3 위치
			_float fRatio = 0.5f + fDistance * 0.02f;

			// 타겟 쪽 Y값이 너무 높다면 플레이어 y 좌표 + 1로 고정
			_vector vAtPosTemp = vPlayerPos * (1 - fRatio) + vTargetPos * fRatio - vTargetDirRight * fDistance * 0.1f;

			_float fAtY = min(XMVectorGetY(vAtPosTemp) + 1.2f - fDistRate  * 0.8f, XMVectorGetY(vEyePos) * 1.2f + fDistRate * 3.f);
			//AtY = max(XMVectorGetY(vPlayerPos) - 3.f , AtY);
			vAtPosTemp = XMVectorSetY(vAtPosTemp, fAtY);

			_vector vAtPos = vEyePos + XMVector3Normalize(vAtPosTemp - vEyePos) * 4.f;

			_vector vCurAt, vCurEye;

			_vector vPrevAt = XMLoadFloat3(&m_CameraDesc.vAt);
			vCurAt = XMVectorLerp(vPrevAt, vAtPos, (_float)TimeDelta * 5.f);
			_vector vPrevEye = XMLoadFloat3(&m_CameraDesc.vEye);
			vCurEye = XMVectorLerp(vPrevEye, vEyePos, (_float)TimeDelta * 5.f);
			
			XMStoreFloat3(&m_CameraDesc.vAt, vCurAt);
			XMStoreFloat3(&m_CameraDesc.vEye, vCurEye);
			m_CameraDesc.fFovy = XMConvertToRadians(45.f);

			m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
			m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
		}
		else
		{
			if (true == m_bPreLockOn)
			{
				//_vector vLookMinus = m_pPlayerTransform->Get_State()
				_vector vPreAt = XMLoadFloat3(&m_CameraDesc.vAt);
				_vector vPreEye = XMLoadFloat3(&m_CameraDesc.vEye);

				_vector vPreCamDir = vPreEye - vPreAt;

				_vector vDefaultDir = XMLoadFloat3(&m_vDir);

				m_fCurDistance = XMVectorGetX(XMVector3Length(vPreCamDir));

				if (0.f > XMVectorGetY(XMVector3Cross(vDefaultDir, XMVectorSetY(vPreCamDir, 0.f))))
					m_fYCurAngle = -XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetY(vPreCamDir, 0.f))))));
				else
					m_fYCurAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetY(vPreCamDir, 0.f))))));


				m_fXCurAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDefaultDir, XMVector3Normalize(XMVectorSetX(vPreCamDir, 0.f))))));
				if (m_fXCurAngle > 90.f)
					m_fXCurAngle = 180 - m_fXCurAngle;

				m_fTargetDistance = 4.f;
				m_fXTargetAngle = m_fXCurAngle;
				m_fYTargetAngle = m_fYCurAngle;

				m_SoftLerpDuration = 0.3;
			}

#pragma region Input

			if (m_bFixMouse)
			{
				_long MouseMove = 0;

				// 플레이어 기준 x축 회전
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_Y))
				{
					m_fXTargetAngle += (_float)TimeDelta * MouseMove * 4.f * 2.f;

					// x축 회전각 제한
					if (m_fXTargetAngle > m_fXAngleMax)
						m_fXTargetAngle = m_fXAngleMax;
					else if (m_fXTargetAngle < m_fXAngleMin)
						m_fXTargetAngle = m_fXAngleMin;
				}
				// 현재 각도, 목표 각도 값 보간
				if (m_fXCurAngle != m_fXTargetAngle)
				{
					_float fGap = m_fXTargetAngle - m_fXCurAngle;

					if (fabs(fGap) < 0.1f)
						m_fXCurAngle = m_fXTargetAngle;
					else
						m_fXCurAngle += fGap * (_float)TimeDelta * 20.f;

				}

				// 플레이어 기준 Y축 회전
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_X))
					m_fYTargetAngle += (_float)TimeDelta * MouseMove * 4.f * 2.f;
				// 현재 각도, 목표 각도 값 보간
				if (m_fYCurAngle != m_fYTargetAngle)
				{
					_float fGap = m_fYTargetAngle - m_fYCurAngle;

					if (fabs(fGap) < 0.1f)
						m_fYCurAngle = m_fYTargetAngle;
					else
						m_fYCurAngle += fGap * (_float)TimeDelta * 20.f;
				}

				// 줌 인/아웃
				if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_WHEEL))
				{
					m_fTargetDistance -= (_float)TimeDelta * MouseMove * 0.4f;
				}

				// 거리 제한
				if (m_fTargetDistance > m_fDistanceMax)
					m_fTargetDistance = m_fDistanceMax;
				else if (m_fTargetDistance < m_fDistanceMin)
					m_fTargetDistance = m_fDistanceMin;

				// 현재 거리, 목표 거리 값 보간

				if (m_fCurDistance != m_fTargetDistance)
				{
					_float fGap = m_fTargetDistance - m_fCurDistance;

					if (fabs(fGap) < 0.05f)
						m_fCurDistance = m_fTargetDistance;
					else
					{
						m_fCurDistance += fGap * (_float)TimeDelta * 20.f;
					}
				}



				POINT	Center = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
				ClientToScreen(g_hWnd, &Center);
				SetCursorPos(Center.x, Center.y);
			}
#pragma endregion
			// At 높이
			if (m_fCurDistance < 2.f)
				m_fPinHeight = 1.55f - m_fCurDistance * 0.3f;
			else
				m_fPinHeight = 0.7f + m_fCurDistance * 0.2f;

			_vector vAxisRight = XMVector3Cross(XMLoadFloat3(&m_vDir), XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_matrix matRotX = XMMatrixRotationAxis(vAxisRight, XMConvertToRadians(m_fXCurAngle));
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(m_fYCurAngle));

			_vector vCamLook = XMVector3Normalize(-XMVector3TransformNormal(XMVector3TransformNormal(XMLoadFloat3(&m_vDir), matRotX), matRotY));

			_vector vAtPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_fPinHeight, 0.f, 0.f);
			XMStoreFloat3(&m_vTargetPrevPos, vAtPos);

			_vector vPrevAt = XMLoadFloat3(&m_CameraDesc.vAt);
			_vector vCurAt = XMVectorLerp(vPrevAt, vAtPos, (_float)TimeDelta * 5.f);

			if (0.0 == m_SoftLerpDuration)
			{
				if (m_fMaxPosDistance < XMVectorGetX(XMVector3Length(vAtPos - vCurAt)))
					vCurAt = vAtPos + XMVector3Normalize(vCurAt - vAtPos) * m_fMaxPosDistance;
				else if (XMVector3NearEqual(vPrevAt, vAtPos, XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
					vCurAt = vAtPos;
			}

			XMStoreFloat3(&m_CameraDesc.vAt, vCurAt);
			XMStoreFloat3(&m_CameraDesc.vEye, vCurAt - vCamLook * m_fCurDistance);
			m_CameraDesc.fFovy = XMConvertToRadians(45.f);

			m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
			m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));
		}
	}
	// Camera Curve 적용 중
	else
	{
		m_FrameAcc += 60.0 * TimeDelta;
		CCameraCurve::CURVEKEY CurveKey = m_pCamCurve->Get_CurCamCurve(&m_iCurKey, m_FrameAcc);
		m_fXCurAngle = CurveKey.fXCurAngle;
		m_fYCurAngle = CurveKey.fYCurAngle;
		m_fCurDistance = CurveKey.fCurDistance;
		m_fPinHeight = CurveKey.fPinHeight;

		_vector vAxisRight = XMVector3Cross(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), XMVectorSet(0.f, 1.f, 0.f, 0.f));

		_matrix matRotX = XMMatrixRotationAxis(vAxisRight, XMConvertToRadians(m_fXCurAngle));
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(m_fYCurAngle));

		_vector vCamLook = XMVector3Normalize(-XMVector3TransformNormal(XMVector3TransformNormal(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK), matRotX), matRotY));

		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_fPinHeight, 0.f, 0.f);
		XMStoreFloat3(&m_vTargetPrevPos, vPlayerPos);
		/*
		_vector vPrevAt = XMLoadFloat3(&m_CameraDesc.vAt);
		_vector vCurAt = XMVectorLerp(vPrevAt, vAtPos, 0.1f);

		if (m_fMaxPosDistance < XMVectorGetX(XMVector3Length(vAtPos - vCurAt)))
		vCurAt = vAtPos + XMVector3Normalize(vCurAt - vAtPos) * m_fMaxPosDistance;
		else if (XMVector3NearEqual(vPrevAt, vAtPos, XMVectorSet(0.05f, 0.05f, 0.05f, 0.05f)))
		vCurAt = vAtPos;*/

		XMStoreFloat3(&m_CameraDesc.vAt, vPlayerPos);
		XMStoreFloat3(&m_CameraDesc.vEye, vPlayerPos - vCamLook * m_fCurDistance);
		m_CameraDesc.fFovy = XMConvertToRadians(45.f);

		m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
		m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

	}

	//
	m_bPreLockOn = m_pPlayerStateClass->Get_LockOn();
	m_bPreAiming = m_pPlayerStateClass->Get_Aiming();

	//여기서 월드매트릭스랑 원하는 상태를 보간

#ifdef _DEBUG
	// 카메라 쉐이크 강도 체크용
	if (pGameInstance->InputKey(DIK_V) == KEY_STATE::HOLD)
	{
		if (m_bShakeToggle)
			StartWave(m_ShakeDesc);
		else
			StartVibration(m_fVibeRange, m_fVibeDuration);
	}
#endif

	if (false == m_bUse)
		return;

	__super::Tick(TimeDelta);
	// 카메라 높이가 지형보다 낮을 경우 높이 재설정
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	if (m_pPlayerNavigation->Get_CurCellState() == CCell::NORMAL_CELL)
	{
		_float fHeight = m_pPlayerNavigation->Compute_Height(vPos);

		if (XMVectorGetY(vPos) < fHeight + 0.5f)
		{
			_vector vFixedPos = XMVectorSetY(vPos, fHeight + 0.5f);
			//vFixedPos = XMVectorLerp(vPos, vFixedPos, (_float)TimeDelta);
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vFixedPos);
			m_pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pMainTransform->Get_WorldMatrixInverse());
		}
	}

}

void CPlayerCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (true == m_bApplyCurve && nullptr != m_pCamCurve)
	{
		if (m_FrameAcc > m_pCamCurve->Get_Duration())
			End_Curve();
	}
}

void CPlayerCamera::RenderGUI()
{
#ifdef _DEBUG
	ImGui::Begin("Camera Shake Setting");
	ImGui::Text("CameraShake : V Key");

#ifdef _DEBUG
	ImGui::Checkbox("Vibe/Wave Toggle", (_bool*)&m_bShakeToggle);
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " Vibration Setting");
	ImGui::DragFloat("Vibe Range", (_float*)&m_fVibeRange, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Vibe Duration", (_float*)&m_fVibeDuration, 0.1f, 0.1f, 20.f);

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " Wave Setting ");
	ImGui::DragFloat("Wave Power", (_float*)&m_ShakeDesc.fPower, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Wave Speed", (_float*)&m_ShakeDesc.fSpeed, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Wave Duration", (_float*)&m_ShakeDesc.fDuration, 0.1f, 0.1f, 10.f);

#endif // DEBUG

	ImGui::End();
#endif
}

CPlayerCamera * CPlayerCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerCamera* pInstance = new CPlayerCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerCamera::Clone(void * pArg)
{
	CPlayerCamera* pInstance = new CPlayerCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCamera::Free()
{
	__super::Free();
}

void CPlayerCamera::AttachTargetTransform(CTransform * pTransform)
{
	if (!pTransform)
		return;

	m_pPlayerTransform = pTransform;
}

void CPlayerCamera::SoftLerp()
{

}

void CPlayerCamera::Reset_ActiveCharacter(CCharacter * pCharacter)
{
	m_pPlayerTransform = pCharacter->GetTransform();
	m_pPlayerNavigation = static_cast<CNavigation*>(m_pPlayerStateClass->Get_ActiveCharacter()->Find_Component(TEXT("Com_Navigation")));
}
