#include "..\Public\Component_Manager.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iLevelIndex)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iLevelIndex = iLevelIndex;

	m_pPrototypes = new PROTOTYPES[m_iLevelIndex];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypTag, CComponent* pPrototype)
{
	if (nullptr == m_pPrototypes ||
		nullptr != Find_Prototype(iLevelIndex, strPrototypTag))
		return E_FAIL;


	m_pPrototypes[iLevelIndex].emplace(strPrototypTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pPrototypes[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pPrototypes[iLevelIndex].clear();
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag)
{
	if (iLevelIndex >= m_iLevelIndex)
		return nullptr;

	auto	iter = m_pPrototypes[iLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iLevelIndex; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
		{
			Safe_Release(Pair.second);
		}
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);

}
