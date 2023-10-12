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
	, m_pRasterState(rhs.m_pRasterState)
	, m_tRasterDesc(rhs.m_tRasterDesc)
	, m_Indicies(rhs.m_Indicies)
	, m_pBufferPos(rhs.m_pBufferPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pRasterState);
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

	if(nullptr != m_pRasterState)
		m_pContext->RSSetState(m_pRasterState);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);


	return S_OK;
}

HRESULT CVIBuffer::Set_RasterState(_bool eWireFrame)
{
	if (nullptr != m_pRasterState)
		Safe_Release(m_pRasterState);

	if (true == eWireFrame)
	{
		ZeroMemory(&m_tRasterDesc, sizeof(D3D11_RASTERIZER_DESC));
		m_tRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		m_tRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		if (FAILED(m_pDevice->CreateRasterizerState(&m_tRasterDesc, &m_pRasterState)))
			return E_FAIL;
	}
	if (false == eWireFrame)
	{
		ZeroMemory(&m_tRasterDesc, sizeof(D3D11_RASTERIZER_DESC));
		m_tRasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		m_tRasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		if (FAILED(m_pDevice->CreateRasterizerState(&m_tRasterDesc, &m_pRasterState)))
			return E_FAIL;
	}

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

	if (nullptr != m_pBufferPos)
		Safe_Delete_Array(m_pBufferPos);

	Safe_Release(m_pRasterState);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
