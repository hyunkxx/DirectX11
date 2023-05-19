#include "stdafx.h"
#include "..\Public\PlayerCamera.h"

#include "GameMode.h"
#include "GameInstance.h"

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

	m_ShakeDesc.fPower = 2.f;
	m_ShakeDesc.fSpeed = 10.f;
	m_ShakeDesc.fDuration = 2.f;

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
	m_pPlayerTransform = static_cast<CTransform*>(pGameInstance->Find_GameObject(nCurrentLevel, TEXT("Player"))->Find_Component(TEXT("Com_Transform")));
	if (nullptr == m_pPlayerTransform)
		return;
	Safe_AddRef(m_pPlayerTransform);

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

	m_pPlayerNavigation = static_cast<CNavigation*>(pGameInstance->Find_GameObject(nCurrentLevel, TEXT("Player"))->Find_Component(TEXT("Com_Navigation")));
}

void CPlayerCamera::Tick(_double TimeDelta)
{
	if (false == m_bUse)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// CameraCurve
	if (false == m_bApplyCurve)
	{
#pragma region Input
		if (pGameInstance->InputKey(DIK_Z) == KEY_STATE::TAP ||
			pGameInstance->InputKey(DIK_LALT) == KEY_STATE::HOLD)
		{
			m_bFixMouse = !m_bFixMouse;
		}

		if (m_bFixMouse)
		{
			_long MouseMove = 0;

			// 플레이어 기준 x축 회전
			if (MouseMove = pGameInstance->InputMouseMove(Engine::MOUSE_MOVESTATE::DIMM_Y))
			{
				m_fXTargetAngle += (_float)TimeDelta * MouseMove * 4.f;

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
				m_fYTargetAngle += (_float)TimeDelta * MouseMove * 4.f;
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

				// 거리 제한
				if (m_fTargetDistance > m_fDistanceMax)
					m_fTargetDistance = m_fDistanceMax;
				else if (m_fTargetDistance < m_fDistanceMin)
					m_fTargetDistance = m_fDistanceMin;
			}
			// 현재 거리, 목표 거리 값 보간
			if (m_fCurDistance != m_fTargetDistance)
			{
				_float fGap = m_fTargetDistance - m_fCurDistance;

				if (fabs(fGap) < 0.05f)
					m_fCurDistance = m_fTargetDistance;
				else
					m_fCurDistance += fGap * (_float)TimeDelta * 20.f;
			}

			POINT	Center = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
			ClientToScreen(g_hWnd, &Center);
			SetCursorPos(Center.x, Center.y);
		}
#pragma endregion
		// 타겟을 락온했을 경우
		/*if (false == m_bLockOn)
		{

		}*/

		// At 높이
		if (m_fCurDistance < 2.f)
			m_fPinHeight = 1.55f - m_fCurDistance * 0.3f;
		else
			m_fPinHeight = 0.7f + m_fCurDistance * 0.2f;

		_vector vAxisRight = XMVector3Cross(XMLoadFloat3(&m_vDir), XMVectorSet(0.f, 1.f, 0.f, 0.f));

		_matrix matRotX = XMMatrixRotationAxis(vAxisRight, XMConvertToRadians(m_fXCurAngle));
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(m_fYCurAngle));

		_vector vCamLook = XMVector3Normalize(-XMVector3TransformNormal(XMVector3TransformNormal(XMLoadFloat3(&m_vDir), matRotX), matRotY));

		_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_fPinHeight, 0.f, 0.f);
		XMStoreFloat3(&m_vTargetPrevPos, vPlayerPos);

		_vector vPrevAt = XMLoadFloat3(&m_CameraDesc.vAt);
		_vector vCurAt = XMVectorLerp(vPrevAt, vPlayerPos, (_float)TimeDelta * 5.f);

		if (m_fMaxPosDistance < XMVectorGetX(XMVector3Length(vPlayerPos - vCurAt)))
			vCurAt = vPlayerPos + XMVector3Normalize(vCurAt - vPlayerPos) * m_fMaxPosDistance;
		else if (XMVector3NearEqual(vPrevAt, vPlayerPos, XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
			vCurAt = vPlayerPos;

		XMStoreFloat3(&m_CameraDesc.vAt, vCurAt);
		XMStoreFloat3(&m_CameraDesc.vEye, vCurAt - vCamLook * m_fCurDistance);

		m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
		m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

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
		_vector vCurAt = XMVectorLerp(vPrevAt, vPlayerPos, 0.1f);

		if (m_fMaxPosDistance < XMVectorGetX(XMVector3Length(vPlayerPos - vCurAt)))
		vCurAt = vPlayerPos + XMVector3Normalize(vCurAt - vPlayerPos) * m_fMaxPosDistance;
		else if (XMVector3NearEqual(vPrevAt, vPlayerPos, XMVectorSet(0.05f, 0.05f, 0.05f, 0.05f)))
		vCurAt = vPlayerPos;*/


		XMStoreFloat3(&m_CameraDesc.vAt, vPlayerPos);
		XMStoreFloat3(&m_CameraDesc.vEye, vPlayerPos - vCamLook * m_fCurDistance);

		m_pMainTransform->Set_State(CTransform::STATE::STATE_POSITION, XMLoadFloat3(&m_CameraDesc.vEye));
		m_pMainTransform->LookAt(XMLoadFloat3(&m_CameraDesc.vAt));

	}

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

	__super::Tick(TimeDelta);

	// 카메라 높이가 지형보다 낮을 경우 높이 재설정
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fHeight = m_pPlayerNavigation->Compute_Height(vPos);
	if (XMVectorGetY(vPos) < fHeight + 0.5f)
	{
		vPos = XMVectorSetY(vPos, fHeight + 0.5f);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
		m_pPipeLine->Set_Transform(CPipeLine::TS_VIEW, m_pMainTransform->Get_WorldMatrixInverse());
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
	ImGui::Begin("Camera Shake Setting");
	ImGui::Text("CameraShake : V Key");
	ImGui::Checkbox("Vibe/Wave Toggle", (_bool*)&m_bShakeToggle);
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " Vibration Setting");
	ImGui::DragFloat("Vibe Range", (_float*)&m_fVibeRange, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Vibe Duration", (_float*)&m_fVibeDuration, 0.1f, 0.1f, 20.f);

	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " Wave Setting ");
	ImGui::DragFloat("Wave Power", (_float*)&m_ShakeDesc.fPower, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Wave Speed", (_float*)&m_ShakeDesc.fSpeed, 0.1f, 0.1f, 20.f);
	ImGui::DragFloat("Wave Duration", (_float*)&m_ShakeDesc.fDuration, 0.1f, 0.1f, 10.f);

	ImGui::End();
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

	if (true == m_bClone)
	{
		Safe_Release(m_pPlayerTransform);
	}
}
