#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_listGameObject.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_listGameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}

}

void CLayer::LateTick(_float fTimeDelta)
{
	for (auto& pGameObject : m_listGameObject)
	{
		if (nullptr != pGameObject)
			pGameObject->LateTick(fTimeDelta);
	}

}

CGameObject* CLayer::Last_GameObject()
{
	return m_listGameObject.back();
}

CGameObject* CLayer::Find_GameObject(const wstring& ObjName, _uint iCloneIndex)
{
	for (auto& pGameObject : m_listGameObject)
	{
		if (nullptr != pGameObject)
		{
			if (ObjName == pGameObject->Get_Name() &&
				iCloneIndex == pGameObject->Get_Index())
				return pGameObject;
		}
		else
			return nullptr;
	}
}

HRESULT CLayer::Delete_GameObject(const wstring& ObjName, _uint iCloneIndex)
{
	for (auto iter = m_listGameObject.begin(); iter != m_listGameObject.end(); ++iter)
	{

		if (ObjName == (*iter)->Get_Name() &&
			iCloneIndex == (*iter)->Get_Index())
		{
			Safe_Release(*iter);
			m_listGameObject.erase(iter);
			
			return S_OK;
		}
	}
}

HRESULT CLayer::Delete_Layer()
{
	for (auto& pGameObject : m_listGameObject)
	{
		Safe_Release(pGameObject);
	}
	m_listGameObject.clear();

	return S_OK;
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_listGameObject)
		Safe_Release(pGameObject);


	m_listGameObject.clear();
}
