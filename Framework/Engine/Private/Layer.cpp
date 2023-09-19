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

CGameObject* CLayer::Get_FirstObject()
{
	if (nullptr == m_listGameObject.front())
		return nullptr;
	else
		return m_listGameObject.front();
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
