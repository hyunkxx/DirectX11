#pragma once

#include "GameObject.h"
#include "ItemDB.h"

BEGIN(Engine)

END

BEGIN(Client)


class CInventory final : public CGameObject
{
public:
	enum INVEN_TYPE  { INVEN_WEAPON, INVEN_COOK,	, INVEN_COIN, INVEN_MAX };
public:
	explicit CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CInventory(const CInventory& rhs);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual void RenderGUI();

	/* �ʿ��� ���
	�߰�
	* Ÿ�Ժ��� �߰�
	* ������ �ִ뺸�������� �Ѿ�� �ٸ����Կ� �����ϰ� ���� ������
	* ������ �ִ밹���� �� ä��� �������� ȹ���� �� ����
	* ���� 5���� �ر� ����

	����
	* �ش� ������ �������� ����
	*/

public:
	_bool DeleteCoin(_uint iPayment);
	_uint GetCoin() const { return m_iCoin; }
	CItem::ITEM_DESC& GetSlotData(INVEN_TYPE eInvenType, _uint iSlotIndex) { return m_Items[eInvenType][iSlotIndex]; };

	void PushItemDesc(CItem::ITEM_DESC tagItemDesc);
	void AddItem(_uint iItemID, _uint iAmount);
	void DiscardItem(INVEN_TYPE eInvenType, _uint iSlotIndex, _uint iAmount = 1);
	_bool EraseItem(INVEN_TYPE eInvenType, _uint iItemID, _uint iAmount);

	// �˻��� �������� ��ü ���� (���� ��� x)
	_uint GetTotalAmount(INVEN_TYPE eInvenType, _uint iItemID);

private:
	_bool addWeapon(CItem::ITEM_DESC tagItemDesc);
	_bool addItem(INVEN_TYPE eInvenType, CItem::ITEM_DESC tagItemDesc);
	_bool addCoin(CItem::ITEM_DESC tagItemDesc);

public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	enum { MAX_AMOUNT = 50 };
	_uint m_iSlotLimit[INVEN_MAX];

	_uint m_iCoin = 100000; // �⺻ ���� ��ȭ
	vector<CItem::ITEM_DESC> m_Items[INVEN_MAX];

};

END