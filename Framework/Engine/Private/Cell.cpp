#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Transform.h"

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
		m_vNormals[i] = _float3(vLines[i].z * -1.f, 0.f, vLines[i].x);
		m_vNormals[i].Normalize();
	}

#ifdef _DEBUG
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

void CCell::Set_Passage()
{
	if (-1 == m_iNeighborIndicies[LINE_AB])
		m_iNeighborIndicies[LINE_AB] = -2;
	if (-1 == m_iNeighborIndicies[LINE_BC])
		m_iNeighborIndicies[LINE_BC] = -2;
	if (-1 == m_iNeighborIndicies[LINE_CA])
		m_iNeighborIndicies[LINE_CA] = -2;
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
		vSour.Normalize();
		vDest.Normalize();

		if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			if (-1 == m_iNeighborIndicies[i])
			{
				m_vSlideNormal = vDest;
			}
			else
			{
				m_vSlideNormal = { -1.f, -1.f, -1.f, -1.f };
			}

			pNeighborIndex = m_iNeighborIndicies[i];
			return true;
		}
	}

	return false;
}

_bool CCell::IsIn(_vector vPoint, _matrix WorldMatrix, _int& pCurIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_float3 WorldPoint = m_vPoints_World[i];
		_vector Point = vPoint;
		Point.y = 0; WorldPoint.y = 0;

		_vector vSour = Point - WorldPoint;
		_vector vDest = XMVector3TransformNormal(m_vNormals[i], WorldMatrix);
		vSour.Normalize();
		vDest.Normalize();

		if (0 < vSour.Dot(vDest))
		{
			return false;
		}
	}

	pCurIndex = m_iIndex;
	return true;
}

_float3* CCell::IsClose(_vector vPoint, _float CompareLength)
{
	for (size_t i = 0; i < POINT_END; i++)
	{
		_float fDeltaX = vPoint.x - m_vPoints_World[i].x;
		_float fDeltaY = vPoint.y - m_vPoints_World[i].y;
		_float fDeltaZ = vPoint.z - m_vPoints_World[i].z;

		_float fDist = sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY + fDeltaZ * fDeltaZ);

		if (CompareLength >= fDist)
		{
			return &m_vPoints_World[i];
		}
			
	}

	return nullptr;
}

_float3 CCell::Get_Middle_Pos()
{
	_float3 midPoint;
	midPoint.x = (m_vPoints_World[POINT_A].x + m_vPoints_World[POINT_B].x + m_vPoints_World[POINT_C].x) / 3.f;
	midPoint.y = (m_vPoints_World[POINT_A].y + m_vPoints_World[POINT_B].y + m_vPoints_World[POINT_C].y) / 3.f;
	midPoint.z = (m_vPoints_World[POINT_A].z + m_vPoints_World[POINT_B].z + m_vPoints_World[POINT_C].z) / 3.f;

	return midPoint;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	return m_pVIBuffer->Render();
}
#endif // !NDEBUG

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

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // !NDEBUG


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
