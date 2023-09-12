#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurLevel)
		return;

	m_pCurLevel->Tick(fTimeDelta);
}

void CLevel_Manager::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pCurLevel)
		return;

	m_pCurLevel->LateTick(fTimeDelta);
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	if (nullptr != m_pCurLevel)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Clear(m_iCurLevelIndex);

		Safe_Release(pGameInstance);
	}

	Safe_Release(m_pCurLevel);

	m_pCurLevel = pNewLevel;

	m_iCurLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pCurLevel);
}
