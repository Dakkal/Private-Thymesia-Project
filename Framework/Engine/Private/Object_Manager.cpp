#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iLevelIndex)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iLevelIndex];

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_mapPrototypes.insert({ strPrototypeTag, pPrototype });

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].insert({ strLayerTag, pLayer });
	}
	else
		pLayer->Add_GameObject(pGameObject);


	return S_OK;
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelIndex; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Tick(fTimeDelta);
		}
	}

}

void CObject_Manager::LateTick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iLevelIndex; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->LateTick(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();

}

CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& ObjName, _uint iIndex)
{
	auto iter = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr != iter)
		return iter->Find_GameObject(ObjName, iIndex);
}

CGameObject* CObject_Manager::Last_GameObject(_uint iLevelIndex, const wstring& strLayerTag)
{
	auto iter = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == iter)
		return nullptr;

	
	return iter->Last_GameObject();
}

HRESULT CObject_Manager::Delete_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& ObjName, _uint iIndex)
{
	auto iter = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == iter)
		return E_FAIL;

	for (auto mapiter = m_mapPrototypes.begin(); mapiter != m_mapPrototypes.end(); mapiter++)
	{
		if (ObjName == mapiter->second->Get_Name())
		{
			mapiter->second->Decrease_CloneIndex();
			break;
		}
	}
	return iter->Delete_GameObject(ObjName, iIndex);
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto iter = m_mapPrototypes.find(strPrototypeTag);

	if (iter == m_mapPrototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iLevelIndex)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iLevelIndex; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_mapPrototypes)
		Safe_Release(Pair.second);

	m_mapPrototypes.clear();
}
