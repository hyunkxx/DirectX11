#include "stdafx.h"
#include "..\Public\Item.h"

#include "GameInstance.h"

CItem::CItem(ITEM_DESC tagItemDesc)
	: m_ItemDesc(tagItemDesc)
{
}

CItem::CItem(const CItem & rhs)
	: m_ItemDesc(rhs.m_ItemDesc)
{
}