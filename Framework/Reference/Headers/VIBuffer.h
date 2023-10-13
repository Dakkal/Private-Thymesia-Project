#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract: public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

	vector<_float3>& Get_Buffer_Pos() { return m_BufferPoses; }
	vector<_ulong>& Get_Indicies() { return m_Indicies; }

public:
	HRESULT			Set_RasterState(_bool eWireFrame = false);

protected:
	ID3D11Buffer*		m_pVB = { nullptr };
	ID3D11Buffer*		m_pIB = { nullptr };

	D3D11_BUFFER_DESC			m_tBufferDesc;
	D3D11_SUBRESOURCE_DATA		m_tInitialData;
	_uint						m_iStride = { 0 };
	_uint						m_iNumVertices = { 0 };
	_uint						m_iIndexStride = { 0 };
	_uint						m_iNumIndices = { 0 };
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;
	_uint						m_iNumVBs = { 0 };

	ID3D11RasterizerState*		m_pRasterState = { nullptr };
	D3D11_RASTERIZER_DESC		m_tRasterDesc;

	vector<_float3> m_BufferPoses;
	vector<_ulong> m_Indicies;

protected:
	HRESULT Create_Buffer(_Inout_ ID3D11Buffer * *ppOut);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END