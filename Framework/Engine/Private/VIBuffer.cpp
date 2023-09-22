#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_iNumVBs(rhs.m_iNumVBs)
	, m_tRasterDesc(rhs.m_tRasterDesc)
	, m_pRasterizer(rhs.m_pRasterizer)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pRasterizer);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB,
	};
	_uint			iStrides[] = {
		m_iStride,
	};
	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVBs, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	if(nullptr != m_pRasterizer)
		m_pContext->RSSetState(m_pRasterizer);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);


	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(_Inout_ ID3D11Buffer** ppOut)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_tBufferDesc, &m_tInitialData, ppOut);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pRasterizer);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
