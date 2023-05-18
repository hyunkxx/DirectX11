#pragma once

#include "GameObject.h"
#include "Item.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CAcquireUI : public CGameObject
{
public:
	explicit CAcquireUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CAcquireUI(const CAcquireUI& rhs);
	virtual ~CAcquireUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	void EnqueueItemDesc(CItem::ITEM_DESC ItemDesc);

private:
	HRESULT addComponents();
	void	resetAcquire();
	_uint	getConvertAmountToTexture(CItem::ITEM_DESC ItemDesc, _uint iDigit);
	_uint	getConvertNumberToLength(_uint iAmount);
public:
	static CAcquireUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;

private:
	enum { ACQUIRE_MAX = 6 };
	
	_float4x4 m_ViewMatrix, m_ProjMatrix;
	ORTHO_DESC m_AcquireTextDesc;
	ORTHO_DESC m_AcquireOrthoDesc[ACQUIRE_MAX];
	ORTHO_DESC m_ItemIconDesc[ACQUIRE_MAX];
	ORTHO_DESC m_ItemTextDesc[ACQUIRE_MAX];

	// Amount ������ 4�ڸ��� ����
	ORTHO_DESC m_ItemAmountBegin[ACQUIRE_MAX];  //xǥ��
	ORTHO_DESC m_ItemAmountDesc[5][ACQUIRE_MAX];//����
private:
	vector<CItem::ITEM_DESC> m_ItemDescs;
	deque<CItem::ITEM_DESC> m_ItemDescQueue;
	
private: // ���ϴ� ȹ�� ������ ����Ʈ ǥ��
	_bool m_bAquireDisable = false;
	_float m_fActiveTimeAcc = 0.f;

	_int m_iCurrentActiveCount = 0;
	_float m_fAcquireCheckAcc = 0.f;
	const _float m_fAcquireCheckSec = 0.2f;

	_float m_fAcquireTextAlpha = 0.f;
	_float m_fAcquireAlpha[ACQUIRE_MAX] = { 0.f, };

private: // ȹ�� �ְ� ��� ǥ��
	ORTHO_DESC m_HighestAcquireOrtho;
	ORTHO_DESC m_HighestItemSlotOrtho;
	ORTHO_DESC m_HighestItemIconOrtho;
	ORTHO_DESC m_HighestItemTextOrtho;

	_bool m_bHighestCheck = false;
	_bool m_bHighestRenderBegin = false;
	_float m_fHighestTimeAcc = 0.f;
	_float m_fHighestAlpha = 0.f;
	CItem::ITEM_GRADE m_Highest = CItem::ITEM_GRADE::NORMAL;
	CItem::ITEM_DESC m_HighestItem;


};

END