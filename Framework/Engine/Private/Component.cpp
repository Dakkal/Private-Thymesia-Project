#include "..\Public\Component.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_IsCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(CGameObject* pOwner, const CComponent& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_IsCloned(true)
	, m_pOwner(pOwner)
{
	//Safe_AddRef(m_pOwner);

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	__super::Free();

	/*if(m_IsCloned)
		Safe_Release(m_pOwner);*/

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
