#include "..\Public\Frustrum_Cull.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustrum_Cull);

CFrustrum_Cull::CFrustrum_Cull()
	: m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CFrustrum_Cull::Initialize()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustrum_Cull::Tick()
{
	_matrix		ProjMatrixInv = m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix		ViewMatrixInv = m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	for (size_t i = 0; i < 8; i++)
	{
		m_vPoints[i] = XMVector3TransformCoord(m_vOriginalPoints[i], ProjMatrixInv);
		m_vPoints[i] = XMVector3TransformCoord(m_vPoints[i], ViewMatrixInv);
	}


	Make_Planes(m_vPoints, m_WorldPlanes);
}

void CFrustrum_Cull::Make_Planes(const _float3* pPoints, _vector* pPlanes)
{
	pPlanes[0] = XMPlaneFromPoints(pPoints[1], pPoints[5], pPoints[6]);
	pPlanes[1] = XMPlaneFromPoints(pPoints[4], pPoints[0], pPoints[3]);
	pPlanes[2] = XMPlaneFromPoints(pPoints[4], pPoints[5], pPoints[1]);
	pPlanes[3] = XMPlaneFromPoints(pPoints[3], pPoints[2], pPoints[6]);
	pPlanes[4] = XMPlaneFromPoints(pPoints[5], pPoints[4], pPoints[7]);
	pPlanes[5] = XMPlaneFromPoints(pPoints[0], pPoints[1], pPoints[2]);
}

_bool CFrustrum_Cull::IsIn_Frustum_World(_vector vWorldPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(m_WorldPlanes[i], vWorldPos)))
			return false;
	}

	return true;
}



void CFrustrum_Cull::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
