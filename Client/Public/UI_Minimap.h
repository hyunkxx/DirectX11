#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)

class CUI_Minimap final : public CGameObject
{
public:
	typedef struct tagMApDesc
	{
		//Ŭ�󺯼�
		_float4			 Pos = { 0.f, 0.f, 0.f, 1.f }; // ���忡���� ������pos , �긦 �������� ���� ���ؼ� �̴ϸʿ� ���
		_float2			IconLU = { 0.f,0.f };
		_float2			IconRB = { 0.f,0.f };
		_float2			IconPointLU = { 0.f,0.f };
		_float2			IconPointRB = { 0.f,0.f };
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 30.f };
		_float		fHeight = { 30.f };
		_float		fColorA = { 0.f };
		_int		iTexNum = { 0 };
		_int		iPass = { 1 };

		_float4x4	WorldMatrix;
		_float4     ColorIcon;
	}MAPDESC;

protected:
	explicit CUI_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Minimap(const CUI_Minimap& rhs);
	virtual ~CUI_Minimap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResourcesMiniMap();
	HRESULT Setup_ShaderResourcesDefaultIcon(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesIcon(_uint Bufferindex);

public:
	
	static CUI_Minimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;





	//����Ʈ�� �׻� ��� �ִ� ����
private:
	//�̴ϸ� ���
	_bool m_bNull = { true };
	// ��ü�ʿ��� ���� ����Ϸ��� ������ uv��ǥ
	_float2			m_TerrainLU = { 0.f,0.f };
	_float2			m_TerrainRB = { 0.f,0.f };

	//�̴ϸʿ� ��µ� �ؽ�ó uv�� ��ü ���������� pos X,Z��
	_float2			m_fPointLU = { 0.f,0.f };
	_float2			m_fPointRB = { 0.f,0.f };

	

	_float			m_fXMiniMap = { -540.f };
	_float			m_fYMiniMap = { 260.f };
	_float			m_fZMiniMap = { 0.f };
	_float			m_fWidthMiniMap = { 300.f };
	_float			m_fHeightMiniMap = { 300.f };
	_float4x4		m_WorldMatrixMiniMap;
	_float4x4		m_ViewMatrix, m_ProjMatrix;

	_float4 ColorMiniMap;
	_float m_fColorAMiniMap = { 0.f };
	_float m_fColorRMiniMap = { 0.f };
	_float m_fColorGMiniMap = { 0.f };
	_float m_fColorBMiniMap = { 0.f };
	
	//����Ʈ ������(�÷��̾�ǥ��,�þ�) 
	_float2			m_DefaultIconLU[2] = { {0.f,0.f} };
	_float2			m_DefaultIconRB[2] = { {0.f,0.f} };

	_float2			m_fDefaultIconPointLU[2] = { {0.f,0.f} };
	_float2			m_fDefaultIconPointRB[2] = { {0.f,0.f} };

	
	_float			m_fXDefaultIcon[2] = { -540.f,-540.f };
	_float			m_fYDefaultIcon[2] = { 260.f ,260.f };
	_float			m_fZDefaultIcon[2] = { 0.f,0.f };
	_float			m_fWidthDefaultIcon[2] = { 30.f ,100.f };
	_float			m_fHeightDefaultIcon[2] = { 30.f,120.f };
	_float4x4		m_WorldMatrixDefaultIcon[2];

	_float4 ColorDefaultIcon[2];
	_float m_fColorADefaultIcon[2] = { 0.f,0.f };
	_float  m_MouseMoveAcc = { 0.f };
	_float  m_DegreeAcc = { 0.f };

	//������ �߰���
	

	// ������ ��ġ�� �ӽú���
	_float4  OriPos = {60.f , 0.f, 20.f, 1.f};
	_float4 fPlayerPos;
	_float4 fPrePlayerPos = { 0.f , 0.f, 0.f, 1.f };


private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CTexture*		m_pTexIcon = { nullptr };
	CTexture*		m_pTexMiniMap = { nullptr };
	CVIBuffer_Rect* m_pVIBufferMiniMap = { nullptr };
	CTexture*		m_pTexDefaultIcon = { nullptr };
	CVIBuffer_Rect*	 m_pVIBufferIcon = { nullptr };

	CVIBuffer_Rect* m_pVIBufferDefaultIcon[2] = { nullptr ,nullptr };

	
	vector<CVIBuffer_Rect*> m_BufferList;
	vector<MAPDESC*>		  m_IconDescList;
	CVIBuffer_Rect*			 m_BufferIcon = { nullptr };
};

END