#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "AsFileUtils.h"
#include "Transform.h"

_matrix CNavigation::m_NaviWorldMatrix[30] = {};
_uint	CNavigation::m_iIndex = 0;

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(CGameObject* pOwner, const CNavigation& rhs)
	: CComponent(pOwner, rhs)
	, m_iCurrentIndex(rhs.m_iCurrentIndex)
	, m_Cells(rhs.m_Cells)
	, m_iNaviIndex(rhs.m_iNaviIndex)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // !NDEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // !NDEBUG

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strNavigationDataFiles)
{
	CAsFileUtils In;
	In.Open(strNavigationDataFiles, FileMode::Read);

	_uint		iCellSize;
	_float3		vPoints[CCell::POINT_END] = {};
	
	In.Read<_uint>(iCellSize);
	for (size_t i = 0; i < iCellSize; i++)
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

	_uint		iPassageSize;
	_uint		iPassage;

	In.Read<_uint>(iPassageSize);
	for (size_t i = 0; i < iPassageSize; i++)
	{
		In.Read<_uint>(iPassage);
		m_Passages.push_back(iPassage);
	}

	if (FAILED(Set_All_CelltoPassage()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::tElements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif // !NDEBUG

	m_iNaviIndex = m_iIndex;
	++m_iIndex;

	return S_OK;
}

HRESULT CNavigation::Initialize_Alone(const _matrix& NaviMatrix)
{
	m_NaviWorldMatrix[m_iNaviIndex] = NaviMatrix;

#ifdef _DEBUG
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
	m_strNavi = pNaviDesc->strNavi;

	return S_OK;
}

void CNavigation::Update(_matrix WorldMatrix)
{
	m_NaviWorldMatrix[m_iNaviIndex] = WorldMatrix;

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Update(WorldMatrix);
	}
}

_int CNavigation::IsMove(_vector vPoint)
{
	_int iNeighborIndex = 0;

	if (true == m_Cells[m_iCurrentIndex]->IsOut(vPoint, m_NaviWorldMatrix[m_iNaviIndex], iNeighborIndex))
	{
		if (-2 == iNeighborIndex)
		{
			return -2;
		}
		else if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-2 == iNeighborIndex)
					return -2;
				if (-1 == iNeighborIndex)
					return -1;

				if (false == m_Cells[iNeighborIndex]->IsOut(vPoint, m_NaviWorldMatrix[m_iNaviIndex], iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					break;
				}
			}
			return 0;
		}
		else
			return -1;
	
	}
	else
		return 0;

}

_int CNavigation::IsIn(_vector vPoint)
{
	_int iCurIndex = -1;

	for (auto& pCell : m_Cells)
	{
		if (true == pCell->IsIn(vPoint, m_NaviWorldMatrix[m_iNaviIndex], iCurIndex))
			return m_iCurrentIndex = iCurIndex;
	}
	return iCurIndex;
}

_int CNavigation::CheckIn(_vector vPoint)
{
	_int iCurIndex = -1;

	for (auto& pCell : m_Cells)
	{
		if (true == pCell->IsIn(vPoint, m_NaviWorldMatrix[m_iNaviIndex], iCurIndex))
			return iCurIndex;
	}
	return iCurIndex;
}

HRESULT CNavigation::Set_toCell(_uint iIndex, CTransform* pTransform)
{
	if (iIndex >= m_Cells.size())
		return E_FAIL;

	m_iCurrentIndex = iIndex;
	_float3 vPos = m_Cells[iIndex]->Get_Middle_Pos();
	_vector vSetPos = _vector(vPos);
	vSetPos.w = 1;
	pTransform->Set_State(CTransform::STATE_POS, vSetPos);

	return S_OK;
}

_vector CNavigation::Get_Cell_SliderVec(_vector vLook)
{
	_vector vContactNormal = m_Cells[m_iCurrentIndex]->Get_SlideNormal();
	if (-1 == vContactNormal.w)
		return vContactNormal;

	_vector vSlider = vLook - vLook.Dot(vContactNormal) * vContactNormal;

	return vSlider;
}

HRESULT CNavigation::Add_Cell(_float3* vPoints)
{
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.push_back(pCell);

	if (FAILED(Set_Neighbors()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Delete_Last_Cell()
{
	if (0 < m_Cells.size())
	{
		Safe_Release(m_Cells.back());
		m_Cells.pop_back();
	}
	
	return S_OK;
}

_float3 CNavigation::Get_Closet_Cell_Point(_vector vPick)
{
	_float3* pWorldPick = nullptr;
	_float3	vReturnPoint;

	for (auto& pCell : m_Cells)
	{
		pWorldPick = pCell->IsClose(vPick, 0.3f);
		if (nullptr != pWorldPick)
		{
			vReturnPoint = XMVector3TransformCoord(*pWorldPick, m_NaviWorldMatrix[m_iNaviIndex].Invert());
			return vReturnPoint;
		}
	}

	vReturnPoint = XMVector3TransformCoord(vPick, m_NaviWorldMatrix[m_iNaviIndex].Invert());
	return vReturnPoint;
}

HRESULT CNavigation::Set_CelltoPassage(_uint iIndex)
{
	m_Cells[iIndex]->Set_Passage();
	m_Passages.push_back(iIndex);

	return S_OK;
}

HRESULT CNavigation::Set_All_CelltoPassage()
{
	for (auto& Passage : m_Passages)
	{
		m_Cells[Passage]->Set_Passage();
	}

	return S_OK;
}

HRESULT CNavigation::Save_Navi(const wstring& Path)
{
	CAsFileUtils Out;
	Out.Open(Path, FileMode::Write);

	Out.Write<_uint>(m_Cells.size());

	for (size_t i = 0; i < m_Cells.size(); i++)
	{
		Out.Write<_float3>(*(m_Cells[i]->Get_Point(CCell::POINT_A)));
		Out.Write<_float3>(*(m_Cells[i]->Get_Point(CCell::POINT_B)));
		Out.Write<_float3>(*(m_Cells[i]->Get_Point(CCell::POINT_C)));
	}

	Out.Write<_uint>(m_Passages.size());
	for (size_t i = 0; i < m_Passages.size(); i++)
	{
		Out.Write<_uint>(m_Passages[i]);
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_NaviWorldMatrix[m_iNaviIndex])))
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
		vColor = { 0.f, 1.f, 0.f, 0.1f };
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vColor, sizeof(_vector))))
			return E_FAIL;

		fHeight = 0.02f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(0)))
			return E_FAIL;

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}

		vColor = { 0.f, 1.f, 0.f, 1.f };
		if (FAILED(m_pShader->Bind_RawValue("g_vLineColor", &vColor, sizeof(_vector))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(1)))
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

		fHeight = 0.04f;
		if (FAILED(m_pShader->Bind_RawValue("g_fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(1)))
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

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _matrix& NaviMatrix)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Alone(NaviMatrix)))
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

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // !NDEBUG

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
}
