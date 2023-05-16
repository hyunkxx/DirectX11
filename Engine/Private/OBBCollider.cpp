#include "..\Public\OBBCollider.h"
#include "DebugDraw.h"
#include "GameObject.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "PipeLine.h"

COBBCollider::COBBCollider(ID3D11Device * device, ID3D11DeviceContext * context)
	: CCollider(device, context)
{
}

COBBCollider::COBBCollider(const COBBCollider & rhs)
	: CCollider(rhs)
{
	_obb = nullptr;
	_obbOriginal = nullptr;
}

HRESULT COBBCollider::InitializePrototype(ID3D11Device * device, ID3D11DeviceContext* context)
{
	m_eCollType = COLL_TYPE::COLL_OBB;

	_batch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	_effect = new BasicEffect(m_pDevice);
	_effect->SetVertexColorEnabled(true);

	const void* shaderByteCode = nullptr;
	size_t shaderByteColdeLength = 0;

	_effect->GetVertexShaderBytecode(&shaderByteCode, &shaderByteColdeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, shaderByteColdeLength, &_inputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT COBBCollider::Initialize(void * arg)
{
	if (nullptr != arg)
	{
		CollDesc.owner = static_cast<COLLIDER_DESC*>(arg)->owner;
		CollDesc.vCenter = static_cast<COLLIDER_DESC*>(arg)->vCenter;
		CollDesc.vExtents = static_cast<COLLIDER_DESC*>(arg)->vExtents;
		CollDesc.vRotation = static_cast<COLLIDER_DESC*>(arg)->vRotation;
	}

	SetOwner(CollDesc.owner);

	if (!CollDesc.owner)
	{
		MSG_BOX("CSphere.Owner == nullptr");
		return E_FAIL;
	}

	_matrix scaleMatrix, rotationMatrix, translationMatrix;
	scaleMatrix = XMMatrixScaling(CollDesc.vExtents.x, CollDesc.vExtents.y, CollDesc.vExtents.z);
	rotationMatrix = XMMatrixRotationX(CollDesc.vRotation.x) * XMMatrixRotationY(CollDesc.vRotation.y) * XMMatrixRotationZ(CollDesc.vRotation.z);
	translationMatrix = XMMatrixTranslation(CollDesc.vCenter.x, CollDesc.vCenter.y, CollDesc.vCenter.z);

	_matrix transformMatrix = XMMatrixIdentity();
	
	_obbOriginal = _obb = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f), _float4(0.f, 0.f, 0.f, 1.f));
	transformMatrix = rotationMatrix * translationMatrix;
	_obb->Transform(*_obb, transformMatrix);
	XMStoreFloat3(&_obb->Extents, XMLoadFloat3(&_obb->Extents) * XMLoadFloat3(&CollDesc.vExtents));
	_obbOriginal = new BoundingOrientedBox(*_obb);

	return S_OK;
}

void COBBCollider::Update(_matrix transformMatrix)
{
	if (!m_isActive)
		return;

	transformMatrix.r[0] = XMVector3Normalize(transformMatrix.r[0]) * (CollDesc.vExtents.x * 2.f);
	transformMatrix.r[1] = XMVector3Normalize(transformMatrix.r[1]) * (CollDesc.vExtents.y * 2.f);
	transformMatrix.r[2] = XMVector3Normalize(transformMatrix.r[2]) * (CollDesc.vExtents.z * 2.f);

	m_isColl = false;
	_obbOriginal->Transform(*_obb, transformMatrix);
}

_bool COBBCollider::Collision(CCollider * targetCollider)
{
	if (!m_isActive)
		return false;

	_bool ret = false;

	if (COLL_SPHERE == targetCollider->GetType())
		ret = _obb->Intersects(*(static_cast<CSphereCollider*>(targetCollider)->_sphere));
	else if (COLL_OBB == targetCollider->GetType())
		ret = _obb->Intersects(*(static_cast<COBBCollider*>(targetCollider)->_obb));
	else if (COLL_AABB == targetCollider->GetType())
		ret = _obb->Intersects(*(static_cast<COBBCollider*>(targetCollider)->_obb));

	return ret;
}

HRESULT COBBCollider::Render()
{
	if (!m_isActive)
		return E_FAIL;

	m_pContext->IASetInputLayout(_inputLayout);
	CPipeLine* pipeline = CPipeLine::GetInstance();

	_effect->SetWorld(XMMatrixIdentity());
	_effect->SetView(pipeline->Get_Transform_Matrix(CPipeLine::TS_VIEW));
	_effect->SetProjection(pipeline->Get_Transform_Matrix(CPipeLine::TS_PROJ));
	
	_effect->Apply(m_pContext);

	_batch->Begin();
	_vector color = m_isColl == true ? XMVectorSet(1.0f, 0.f, 0.f, 1.f) : XMLoadFloat4(&m_vColor);
	
	DX::Draw(_batch, *_obb, color);

	_batch->End();

	return S_OK;
}

void COBBCollider::SetExtents(_float3 vExtents)
{
	CollDesc.vExtents = vExtents;

	_matrix scaleMatrix, rotationMatrix, translationMatrix;
	scaleMatrix = XMMatrixScaling(CollDesc.vExtents.x, CollDesc.vExtents.y, CollDesc.vExtents.z);
	rotationMatrix = XMMatrixRotationX(CollDesc.vRotation.x) * XMMatrixRotationY(CollDesc.vRotation.y) * XMMatrixRotationZ(CollDesc.vRotation.z);
	translationMatrix = XMMatrixTranslation(CollDesc.vCenter.x, CollDesc.vCenter.y, CollDesc.vCenter.z);

	_matrix transformMatrix = XMMatrixIdentity();

	_obb->Transform(*_obb, transformMatrix);
	transformMatrix = rotationMatrix * translationMatrix;
	_obb->Transform(*_obb, transformMatrix);
	XMStoreFloat3(&_obb->Extents, XMLoadFloat3(&_obb->Extents) * XMLoadFloat3(&CollDesc.vExtents));
	_obbOriginal = _obb;
}

void COBBCollider::SetRotation(_float3 vRotation)
{
	CollDesc.vRotation = vRotation;
	
	_matrix scaleMatrix, rotationMatrix, translationMatrix;
	scaleMatrix = XMMatrixScaling(CollDesc.vExtents.x, CollDesc.vExtents.y, CollDesc.vExtents.z);
	rotationMatrix = XMMatrixRotationX(CollDesc.vRotation.x) * XMMatrixRotationY(CollDesc.vRotation.y) * XMMatrixRotationZ(CollDesc.vRotation.z);
	translationMatrix = XMMatrixTranslation(CollDesc.vCenter.x, CollDesc.vCenter.y, CollDesc.vCenter.z);

	_matrix transformMatrix = XMMatrixIdentity();
	_obb->Transform(*_obb, transformMatrix);

	transformMatrix = rotationMatrix * translationMatrix;
	_obb->Transform(*_obb, transformMatrix);
	XMStoreFloat3(&_obb->Extents, XMLoadFloat3(&_obb->Extents) * XMLoadFloat3(&CollDesc.vExtents));
	_obbOriginal = _obb;

}

_float3 COBBCollider::GetCorners()
{
	_float3 fConers;
	_obb->GetCorners(&fConers);
	return fConers;
}

COBBCollider * COBBCollider::Create(ID3D11Device * device, ID3D11DeviceContext * context)
{
	COBBCollider* instance = new COBBCollider(device, context);
	if (FAILED(instance->InitializePrototype(device, context)))
	{
		MSG_BOX("failed to created : COBBCollider");
		Safe_Release(instance);
	}

	return instance;
}

CComponent * COBBCollider::Clone(void * arg)
{
	COBBCollider* instance = new COBBCollider(*this);
	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("failed to cloned : COBBCollider");
		Safe_Release(instance);
	}

	return instance;
}

void COBBCollider::Free()
{
	__super::Free();

	Safe_Delete(_obb);
	Safe_Delete(_obbOriginal);
}
