#include "..\Public\EffectManager.h"
#include "EffectObject.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
{
}

HRESULT CEffectManager::Add_Effect(const wstring strEffectsName, CEffectObject* pGameObject)
{
	auto iter = Find_Effects(strEffectsName);
	if (nullptr == iter)
	{
		list<CEffectObject*> plistEffects;
		plistEffects.push_back(pGameObject);

		m_Effects.emplace(strEffectsName, plistEffects);
	}
	else
	{
		iter->push_back(pGameObject);
	}

	return S_OK;
}

CEffectObject* CEffectManager::Get_Effect(const wstring strEffectsName)
{
	auto iter = Find_Effects(strEffectsName);
	if (nullptr == iter)
	{
		return nullptr;
	}
	else
	{
		for (auto& pEffect : *iter)
		{
			
		}
	}
}

list<CEffectObject*>* CEffectManager::Find_Effects(const wstring strEffectsName)
{
	auto iter = m_Effects.find(strEffectsName);

	if (iter == m_Effects.end())
		return nullptr;

	return &iter->second;
}

void CEffectManager::Free()
{
	__super::Free();

	for (auto& map_iter : m_Effects)
	{
		for (auto& list_iter : map_iter.second)
		{
			Safe_Release(list_iter);
		}
		map_iter.second.clear();
	}
	m_Effects.clear();

}
