#include "..\Public\CollisionManager.h"
#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager()
{
}

HRESULT CCollisionManager::ReserveCollisionLayer(_uint iLayerCount)
{
	m_iCollisionsCount = iLayerCount;

	m_Collisions = new vector<class CCollider*>[m_iCollisionsCount];

	for (_uint i = 0; i < m_iCollisionsCount; ++i)
	{
		m_Collisions[i].reserve(300);
	}

	return S_OK;
}

HRESULT CCollisionManager::AddCollider(CCollider* collider, _uint iLayerIndex)
{
	if (iLayerIndex >= m_iCollisionsCount)
	{
		MSG_BOX("CollisionManager : AddCollider() -> Layer �ʰ�");
		return E_FAIL;
	}

	if (nullptr == collider)
	{
		MSG_BOX("CollisionManager : AddCollider() -> collider is null");
		return E_FAIL;
	}

	if (collider->IsActive())
		m_Collisions[iLayerIndex].emplace_back(collider);

	return S_OK;
}

void CCollisionManager::PhysicsUpdate()
{
	for (_uint iLayer = 0; iLayer < m_iCollisionsCount ; ++iLayer)
	{
		for (_uint i = 0; i < m_Collisions[iLayer].size(); ++i)
		{
			for (_uint j = i; j <  m_Collisions[iLayer].size(); ++j)
			{
				if (nullptr == m_Collisions[iLayer][i] || nullptr == m_Collisions[iLayer][j])
					continue;

				CGameObject* srcObject = m_Collisions[iLayer][i]->GetOwner();
				CGameObject* destObject = m_Collisions[iLayer][j]->GetOwner();
				
				//gameObject �ڽ��� �ݶ��̴����� �浹 ����
				if (srcObject == destObject)
					continue;

				// �ڽŰ� ���� ������Ʈ�� �����ϰ� �浹ó���� �ؾ��� ���
				if (!m_Collisions[iLayer][i]->SameObjectDetection() &&
					!m_Collisions[iLayer][j]->SameObjectDetection())
				{
					//���� ���� ������Ʈ�� �ٽ� �ݺ�
					if (srcObject->GetID() == destObject->GetID())
						continue;
				}
				
				//�浹����
				if (m_Collisions[iLayer][i]->Collision(m_Collisions[iLayer][j]))
				{
					m_Collisions[iLayer][i]->SetCollision(true);
					m_Collisions[iLayer][j]->SetCollision(true);

					//�����浹 Enter
					if (!m_Collisions[iLayer][i]->IsHitCollider(m_Collisions[iLayer][j]))
					{
						IOnCollisionEnter* src = dynamic_cast<IOnCollisionEnter*>(srcObject);
						IOnCollisionEnter* dest = dynamic_cast<IOnCollisionEnter*>(destObject);

						if (src)
							src->OnCollisionEnter(m_Collisions[iLayer][i], m_Collisions[iLayer][j]);
						if (dest)
							dest->OnCollisionEnter(m_Collisions[iLayer][j], m_Collisions[iLayer][i]);

						//�浹���� ����Ʈ�� �߰�
						m_Collisions[iLayer][i]->AddHitCollider(m_Collisions[iLayer][j]);
						m_Collisions[iLayer][j]->AddHitCollider(m_Collisions[iLayer][i]);

					}
					//�浹�� Stay
					else
					{
						IOnCollisionStay* src = dynamic_cast<IOnCollisionStay*>(srcObject);
						IOnCollisionStay* dest = dynamic_cast<IOnCollisionStay*>(destObject);

						if (src)
							src->OnCollisionStay(m_Collisions[iLayer][i], m_Collisions[iLayer][j]);
						if (dest)
							dest->OnCollisionStay(m_Collisions[iLayer][j], m_Collisions[iLayer][i]);
					}
				}
				//�浹���� ����
				else
				{
					//�����浹 �߰� �����浹���� ���� Exit
					if (m_Collisions[iLayer][i]->IsHitCollider(m_Collisions[iLayer][j]))
					{
						IOnCollisionExit* src = dynamic_cast<IOnCollisionExit*>(srcObject);
						IOnCollisionExit* dest = dynamic_cast<IOnCollisionExit*>(destObject);

						if (src)
							src->OnCollisionExit(m_Collisions[iLayer][i], m_Collisions[iLayer][j]);
						if (dest)
							dest->OnCollisionExit(m_Collisions[iLayer][j], m_Collisions[iLayer][i]);

						//�浹���� ����Ʈ���� ����
						m_Collisions[iLayer][i]->EraseHitCollider(m_Collisions[iLayer][j]);
						m_Collisions[iLayer][j]->EraseHitCollider(m_Collisions[iLayer][i]);
					}
				}
			}
		}
	}
 
}

void CCollisionManager::Render()
{
	if (!m_bDebugRender)
	{
		Clear();
		return;
	}

	for (_uint iLayer = 0; iLayer < m_iCollisionsCount; ++iLayer)
	{
		for (auto& coll : m_Collisions[iLayer])
		{
			if (coll->IsVisible())
				coll->Render();
		}
	}

	Clear();
}

int CCollisionManager::GetHasCollisionCount(_uint iLayerIndex)
{
	return (int)m_Collisions[iLayerIndex].size();
}

void CCollisionManager::Clear()
{
	for (_uint iLayer = 0; iLayer < m_iCollisionsCount; ++iLayer)
	{
		m_Collisions[iLayer].clear();
	}
}

void CCollisionManager::Free()
{
	for (_uint iLayer = 0; iLayer < m_iCollisionsCount; ++iLayer)
		m_Collisions[iLayer].clear();

	Safe_Delete_Array(m_Collisions);
}
