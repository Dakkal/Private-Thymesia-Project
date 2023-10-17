#include "..\Public\Cell.h"

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

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_float3		vLines[LINE_END];

	vLines[LINE_AB] = m_vPoints[POINT_B] - m_vPoints[POINT_A];
	vLines[LINE_BC] = m_vPoints[POINT_C] - m_vPoints[POINT_B];
	vLines[LINE_CA] = m_vPoints[POINT_A] - m_vPoints[POINT_C];

	for (size_t i = 0; i < LINE_END; i++)
	{
		m_vNormals[i] = _float3(vLines[i].z * -1, 0.f, vLines[i].x);
		m_vNormals[i].Normalize();
	}


	return S_OK;
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

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
