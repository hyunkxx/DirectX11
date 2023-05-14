#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scale()
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	_vector vUp = Get_State(STATE::STATE_UP);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	return _float3(
		XMVectorGetX(XMVector3Length(vRight)),
		XMVectorGetX(XMVector3Length(vUp)),
		XMVectorGetX(XMVector3Length(vLook)));
}

void CTransform::Set_Scale(_float3 fScale)
{
	_vector vRight = XMVector3Normalize(Get_State(STATE::STATE_RIGHT));
	_vector vUp = XMVector3Normalize(Get_State(STATE::STATE_UP));
	_vector vLook = XMVector3Normalize(Get_State(STATE::STATE_LOOK));

	if (fScale.x <= 0.005f)
		fScale.x = 0.005f;

	if (fScale.y <= 0.005f)
		fScale.y = 0.005f;

	if (fScale.z <= 0.005f)
		fScale.z = 0.005f;

	_matrix matScale = XMMatrixScaling(fScale.x, fScale.y, fScale.z);
	vRight = XMVector3TransformNormal(vRight, matScale);
	vUp = XMVector3TransformNormal(vUp, matScale);
	vLook = XMVector3TransformNormal(vLook, matScale);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_float4 vStateFloat4;
	XMStoreFloat4(&vStateFloat4, vState);

	memcpy(((_float4*)&m_WorldMatrix + eState), &vStateFloat4, sizeof(_float3));
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	ZeroMemory(&m_TransformDesc, sizeof TRANSFORM_DESC);

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORM_DESC));

	return S_OK;
}

void CTransform::MoveForward(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveBackward(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	vPosition += -XMVector3Normalize(vLook) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveRight(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vRight = Get_State(STATE::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveLeft(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vRight = Get_State(STATE::STATE_RIGHT);

	vPosition += -XMVector3Normalize(vRight) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::Move_Anim(_float3 * vMove, _uint iPostitionState, CNavigation * pNavigation, _float3* pTopPosition)
{
	_vector vPos = Get_State(CTransform::STATE_POSITION);
	_vector vRight = Get_State(CTransform::STATE_RIGHT);
	_vector vUp = Get_State(CTransform::STATE_UP);
	_vector vLook = Get_State(CTransform::STATE_LOOK);

	//_vector vMoveXZ = XMVector3Normalize(vRight) * vMove->x - XMVector3Normalize(vLook) * vMove->y;
	_vector vMoveY = XMVector3Normalize(vUp) * vMove->z;
	_vector vMovement = XMVector3Normalize(vRight) * vMove->x - XMVector3Normalize(vLook) * vMove->y + vMoveY;

	if (nullptr == pNavigation)
	{
		Set_State(STATE_POSITION, vPos + vMovement);
		return;
	}

	_float3 vSlideOut = _float3(0.f, 0.f, 0.f);

	_uint iResult = UINT_MAX;
	// 2 == Climb
	if (nullptr != pTopPosition && 2 == iPostitionState)
	{
		_vector vHeadPosition = XMVector3TransformCoord(XMVector3TransformCoord(XMLoadFloat3(pTopPosition), XMMatrixRotationY(180.f)), XMLoadFloat4x4(&m_WorldMatrix));
		iResult = pNavigation->Climb_OnNavigation(vPos, vHeadPosition, vMovement, &vSlideOut);
	}
	else
		iResult = pNavigation->Move_OnNavigation(vPos, vMovement, &vSlideOut);

	if (UINT_MAX == iResult)
		MSG_BOX("BUG!");

	if (CNavigation::NAVI_OK == iResult)
		Set_State(STATE_POSITION, vPos + vMovement);
	else if (CNavigation::NAVI_SLIDE == iResult)
	{
		_vector vSlideXZ = XMLoadFloat3(&vSlideOut);
		
		/* iPositionState == PS_AIR*/
		if (1 == iPostitionState)
		{
			vSlideXZ = XMVectorSetY(vSlideXZ, 0.f);
			vSlideXZ += vMoveY;
		}
			
		Set_State(STATE_POSITION, vPos + vSlideXZ);
	}
}

void CTransform::Push_Position(_fvector vPush)
{
	Set_State(STATE_POSITION, Get_State(STATE_POSITION) + vPush);
}

void CTransform::SetRotationXYZ(_float3 fRadian)
{
	_vector vRight, vUp, vLook;
	_float3 vScale = Get_Scale();

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix matrixRotX, matrixRotY, matrixRotZ, matrixRotXYZ;
	matrixRotX = XMMatrixRotationX(XMConvertToRadians(fRadian.x));
	matrixRotY = XMMatrixRotationY(XMConvertToRadians(fRadian.y));
	matrixRotZ = XMMatrixRotationZ(XMConvertToRadians(fRadian.z));

	matrixRotXYZ = matrixRotX * matrixRotY * matrixRotZ;

	vRight = XMVector3TransformNormal(vRight, matrixRotXYZ);
	vUp = XMVector3TransformNormal(vUp, matrixRotXYZ);
	vLook = XMVector3TransformNormal(vLook, matrixRotXYZ);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::SetRotation(_fvector vAxis, _float fRadian)
{
	_vector vRight, vUp, vLook;
	_float3 vScale = Get_Scale();

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::STATE_RIGHT, vRight);
	Set_State(STATE::STATE_UP, vUp);
	Set_State(STATE::STATE_LOOK, vLook);

	////��Ŀ��� ���Ȱ��� �̾Ƴ���. �������� �����ؾ��� ������
	//_vector vAxisX, vAxisZ;

	//vAxisZ = vLook;
	//vAxisZ = XMVector3Normalize(XMVectorSet(XMVectorGetX(vAxisZ), 0.f, XMVectorGetZ(vAxisZ), 0.f));

	//vAxisX = vRight;
	//vAxisX = XMVector3Normalize(XMVectorSet(XMVectorGetX(vAxisX), 0.f, XMVectorGetZ(vAxisX), 0.f));

	////X
	//_float fValue = XMVectorGetX(XMVector3Cross(vLook, vAxisZ));
	//if (fValue >= 0.f)
	//	m_fAngle.x = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), vAxisZ))));
	//else
	//	m_fAngle.x = 180.f + XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), vAxisZ))));
	////Y
	//fValue = XMVectorGetY(XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vAxisZ));
	//if (fValue >= 0.f)
	//	m_fAngle.y = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vAxisZ))));
	//else
	//	m_fAngle.y = 180.f + XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vAxisZ))));
	////Z
	//fValue = XMVectorGetZ(XMVector3Cross(vRight, vAxisX));
	//if (fValue >= 0.f)
	//	m_fAngle.z = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vRight), vAxisX))));
	//else
	//	m_fAngle.z = 180.f + XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vRight), vAxisX))));
}

void CTransform::Rotate(_fvector vAxis, _double TimeDelta)
{
	_vector vRight, vUp, vLook;
	_float3 vScale = Get_Scale();

	vRight = Get_State(STATE::STATE_RIGHT);
	vUp = Get_State(STATE::STATE_UP);
	vLook = Get_State(STATE::STATE_LOOK);

	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationSpeed * (_float)TimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::STATE_RIGHT, vRight);
	Set_State(STATE::STATE_UP, vUp);
	Set_State(STATE::STATE_LOOK, vLook);
}

void CTransform::Set_LookDir(_fvector vTargetDir)
{
	_vector vLook = XMVector3Normalize(vTargetDir);
	_vector vRight;
	if (vTargetDir.m128_f32[0] == 0.f &&
		vTargetDir.m128_f32[2] == 0.f &&
		vTargetDir.m128_f32[3] == 0.f &&
		vTargetDir.m128_f32[1] != 0.f)
	{
		vRight = XMVector3Normalize(XMVectorSet(vTargetDir.m128_f32[1], 0.f, 0.f, 0.f));
	}
	else
		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	_float3 vScale = Get_Scale();
	Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	Set_State(CTransform::STATE_UP, vUp * vScale.y);
	Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = vTargetPos - vPosition;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_State(STATE::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

_float CTransform::Compute_Distance(_fvector vTargetPos)
{
	return XMVectorGetX(XMVector3Length(vTargetPos - Get_State(STATE::STATE_POSITION)));
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDistance)
{
	LookAt(vTargetPos);

	if (fLimitDistance <= Compute_Distance(vTargetPos))
		MoveForward(TimeDelta);
}

HRESULT CTransform::Setup_ShaderResource(CShader* pShaderCom, const char* pConstantName)
{
	return pShaderCom->SetMatrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
