#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };

	_uint						m_iStride = { 0 };				//���� �ϳ��� ����Ʈ ũ��
	_uint						m_iVertexCount = { 0 };			//�⺻����(�ﰢ��)�� ���� ����
	_uint						m_iFaceIndexSize = { 0 };		//�⺻����(�ﰢ��)�� �ε��� ������ 16 or 32
	_uint						m_iFaceCount = { 0 };			//�׷����� �⺻����(�ﰢ��) ����
	_uint						m_iFaceIndexCount = { 0 };		//�׷����� �⺻����(�ﰢ��) �ε��� ����
	_uint						m_iVertexBuffersCount = { 0 };	//���� ���� ����
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;

	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

};

END