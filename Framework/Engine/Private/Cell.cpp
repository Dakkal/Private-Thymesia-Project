#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints_Origin, pPoints, sizeof(_float3) * POINT_END);

	_float3		vLines[LINE_END];

	vLines[LINE_AB] = m_vPoints_Origin[POINT_B] - m_vPoints_Origin[POINT_A];
	vLines[LINE_BC] = m_vPoints_Origin[POINT_C] - m_vPoints_Origin[POINT_B];
	vLines[LINE_CA] = m_vPoints_Origin[POINT_A] - m_vPoints_Origin[POINT_C];

	for (size_t i = 0; i < LINE_END; i++)
	{
		m_vNormals[i] = _float3(vLines[i].z * -1, 0.f, vLines[i].x);
		m_vNormals[i].Normalize();
	}

#ifndef NDEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints_Origin);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif // !NDEBUG


	return S_OK;
}

void CCell::Update(_matrix WorldMatrix)
{
	for (size_t i = 0; i < POINT_END; i++)
	{
		m_vPoints_World[i] = XMVector3TransformCoord(m_vPoints_Origin[i], WorldMatrix);
	}
}

_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
{
	if (m_vPoints_Origin[POINT_A] == *pSourPoint)
	{
		if (m_vPoints_Origin[POINT_B] == *pDestPoint)
			return true;

		if (m_vPoints_Origin[POINT_C] == *pDestPoint)
			return true;
	}
	if (m_vPoints_Origin[POINT_B] == *pSourPoint)
	{
		if (m_vPoints_Origin[POINT_A] == *pDestPoint)
			return true;

		if (m_vPoints_Origin[POINT_C] == *pDestPoint)
			return true;
	}
	if (m_vPoints_Origin[POINT_C] == *pSourPoint)
	{
		if (m_vPoints_Origin[POINT_A] == *pDestPoint)
			return true;

		if (m_vPoints_Origin[POINT_B] == *pDestPoint)
			return true;
	}

	return false;
}

_bool CCell::IsOut(_vector vPoint, _matrix WorldMatrix, _int& pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector vSour = vPoint - m_vPoints_World[i];
		_vector vDest = XMVector3TransformNormal(m_vNormals[i], WorldMatrix);

		if (0 < vSour.Dot(vDest))
		{
			pNeighborIndex = m_iNeighborIndicies[i];
			return true;
		}
	}

	return false;
}

HRESULT CCell::Render()
{
	return m_pVIBuffer->Render();
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifndef NDEBUG
	Safe_Release(m_pVIBuffer);
#endif // !NDEBUG


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
