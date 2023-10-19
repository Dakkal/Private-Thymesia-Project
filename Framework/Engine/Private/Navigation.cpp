#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "AsFileUtils.h"

_matrix CNavigation::m_NaviWorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(CGameObject* pOwner, const CNavigation& rhs)
	: CComponent(pOwner, rhs)
	, m_iCurrentIndex(rhs.m_iCurrentIndex)
	, m_Cells(rhs.m_Cells)
#ifndef NDEBUG
	, m_pShader(rhs.m_pShader)
#endif // !NDEBUG
{
#ifndef NDEBUG
	Safe_AddRef(m_pShader);
#endif // !NDEBUG

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFiles)
{
	CAsFileUtils In;
	In.Open(strNavigationDataFiles, FileMode::Read);

	_uint		iSize;
	_float3		vPoints[CCell::POINT_END] = {};
	
	In.Read<_uint>(iSize);

	for (size_t i = 0; i < iSize; i+=3)
	{
		In.Read<_float3>(vPoints[CCell::POINT_A]);
		In.Read<_float3>(vPoints[CCell::POINT_B]);
		In.Read<_float3>(vPoints[CCell::POINT_C]);

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	if (FAILED(Set_Neighbors()))
		return E_FAIL;

#ifndef NDEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::tElements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif // !NDEBUG

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVI_DESC* pNaviDesc = (NAVI_DESC*)pArg;

	/*  이 네비게이션을 이용하고자하는 객체가 어떤 셀에 있는지 저장한다. */
	m_iCurrentIndex = pNaviDesc->iCurIndex;

	return S_OK;
}

void CNavigation::Update(_matrix WorldMatrix)
{
	m_NaviWorldMatrix = WorldMatrix;

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Update(WorldMatrix);
	}
}

_bool CNavigation::IsMove(_vector vPoint)
{
	_int iNeighborIndex = 0;

	if (true == m_Cells[m_iCurrentIndex]->IsOut(vPoint, m_NaviWorldMatrix, iNeighborIndex))
	{
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (false == m_Cells[iNeighborIndex]->IsOut(vPoint, m_NaviWorldMatrix, iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					break;
				}
			}
			return true;
		}
		else
			false;
	
	}
	else
		return true;
}

#ifndef NDEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_NaviWorldMatrix)))
		return E_FAIL;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix matView = pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;

	_matrix matProj = pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	RELEASE_INSTANCE(CPipeLine);

	_vector vColor;
	_float	fHeight;

	if (-1 == m_iCurrentIndex)
	{
		vColor = { 0.f, 1.f, 0.f, 1.f };
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vColor, sizeof(_vector))))
			return E_FAIL;

		fHeight = 0.1f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
	{
		vColor = { 1.f, 0.f, 0.f, 1.f };
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vColor, sizeof(_vector))))
			return E_FAIL;

		fHeight = 0.2f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		m_Cells[m_iCurrentIndex]->Render();
	}

	return S_OK;
}
#endif // !NDEBUG

HRESULT CNavigation::Set_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}
			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}

		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strNavigationDataFiles)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFiles)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(CGameObject* pOwner, void* pArg)
{
	CNavigation* pInstance = new CNavigation(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

#ifndef NDEBUG
	Safe_Release(m_pShader);
#endif // !NDEBUG

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
}
