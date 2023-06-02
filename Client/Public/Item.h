#pragma once

#include "GameObject.h"

BEGIN(Client)

class CItem
{
public:
	enum ITEM_TYPE { ITEN_WEAPON, ITEM_COOK, ITEM_MATERIAL, ITEM_COIN };
	enum ITEM_GRADE { NORMAL, ADVANCED, RARE, UNIQUE, LEGEND };

	typedef struct tagItemDesc
	{
		_uint				iItemID;			// ������ ID
		ITEM_TYPE			eItemType;			// ������ ����
		ITEM_GRADE			eItemGrade;			// ������ ���
		_uint				iImageIndex;		// ������ ������ �ؽ���(�ε���)
		_uint				iImageTextIndex;	// ������ �̸� �ؽ���(�ε���)
		_uint				iAmount;			// ����
		_tchar				szTag[MAX_TAG];		// ������ �±�
		_int				iData[2];			// �ΰ� ����
		_bool				bAvailable;			// ��밡�� ����(�ܵ�������)

	}ITEM_DESC;
	
public:
	explicit CItem(ITEM_DESC tagItemDesc);
	explicit CItem(const CItem& rhs);
	~CItem() = default;

public: 
	//������ ���� �������̽� �ۼ�
	ITEM_DESC GetItemDesc() const {	return m_ItemDesc; }


private:
	ITEM_DESC m_ItemDesc;
	
};

END