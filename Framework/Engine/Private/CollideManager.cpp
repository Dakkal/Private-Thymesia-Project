#include "CollideManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "PartObject.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollideManager);

CCollideManager::CCollideManager()
{
}

void CCollideManager::Check_Collision(const _uint iLevel, const LAYER_TAG& _eType1, const LAYER_TAG& _eType2, _float fTimedelta)
{
	CGameObject* pObj1 = nullptr;
	CGameObject* pObj2 = nullptr;
	CCollider* pCol1 = nullptr;
	CCollider* pCol2 = nullptr;

	CHECKITER	iter;

	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	auto Obj1 = pGameInstace->Get_LayerList(iLevel, _eType1);
	auto Obj2 = pGameInstace->Get_LayerList(iLevel, _eType2);
	if (nullptr == Obj1 || nullptr == Obj2 || Obj1->empty() || Obj2->empty())
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	
	for (auto iter1 = Obj1->begin(); iter1 != Obj1->end(); ++iter1)
	{
		for (auto iter2 = Obj2->begin(); iter2 != Obj2->end(); ++iter2)
		{
			pObj1 = (*iter1);	pObj2 = (*iter2);

			if (pObj1 == pObj2 || nullptr == pObj1 || nullptr == pObj2) continue;

			pCol1 = dynamic_cast<CCollider*>(pObj1->Get_Component(TEXT("Com_Collider")));
			pCol2 = dynamic_cast<CCollider*>(pObj2->Get_Component(TEXT("Com_Collider")));

			if (nullptr == pCol1 || nullptr == pCol2) continue;
			else if (false == pCol1->Is_Active() || false == pCol2->Is_Active()) continue;

			Set_Info(iter, pCol1, pCol2);

			if (pCol1->IsCollision(pCol2)) // 충돌
			{
				if (iter->second) // 이전에도 충돌
				{
					if (false == pObj1->Is_Active() || false == pObj2->Is_Active()) // 둘 중 하나 삭제 예정
					{
						pCol1->OnCollision_Exit(pObj2, fTimedelta);
						pCol2->OnCollision_Exit(pObj1, fTimedelta);
						iter->second = false;
					}
					else // 삭제 예정 없음
					{
						pCol1->OnCollision_Stay(pObj2, fTimedelta);
						pCol2->OnCollision_Stay(pObj1, fTimedelta);
					}
				}
				else // 이번에 충돌
				{
					if (true == pObj1->Is_Active() && true == pObj2->Is_Active()) // 둘다 삭제될 예정이 아닐 때만 충돌 처리
					{
						pCol1->OnCollision_Enter(pObj2, fTimedelta);
						pCol2->OnCollision_Enter(pObj1, fTimedelta);
						iter->second = true;
					}
				}
			}
			else // 충돌 X
			{
				if (iter->second) // 이전에 충돌
				{
					pCol1->OnCollision_Exit(pObj2, fTimedelta);
					pCol2->OnCollision_Exit(pObj1, fTimedelta);
					iter->second = false;
				}
			}

			if (0 < pObj1->Get_Parts_Size() || 0 < pObj2->Get_Parts_Size())
			{
				Check_Part_Collision(pObj1, pObj2, pCol1, pCol2, fTimedelta);
			}

		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CCollideManager::Check_Part_Collision(CGameObject* _pObj1, CGameObject* _pObj2, CCollider* _pObj1Col, CCollider* _pObj2Col, _float fTimedelta)
{
	CCollider* pCol1 = nullptr;
	CCollider* pCol2 = nullptr;

	CHECKITER	iter;

	for (auto& pPart2 : _pObj2->Get_Parts())
	{
		if (_pObj1 == pPart2.second || nullptr == _pObj1 || nullptr == pPart2.second) continue;

		pCol2 = dynamic_cast<CCollider*>(pPart2.second->Get_Component(TEXT("Com_Collider")));

		if (nullptr == _pObj1Col || nullptr == pCol2) continue;
		else if (false == _pObj1Col->Is_Active() || false == pCol2->Is_Active()) continue;

		Set_Info(iter, _pObj1Col, pCol2);

		if (_pObj1Col->IsCollision(pCol2)) // 충돌
		{
			if (iter->second) // 이전에도 충돌
			{
				if (false == _pObj1->Is_Active() || false == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // 둘 중 하나 삭제 예정
				{
					_pObj1Col->OnCollision_Exit(pPart2.second, fTimedelta);
					pCol2->OnCollision_Exit(_pObj1, fTimedelta);
					iter->second = false;
				}
				else // 삭제 예정 없음
				{
					_pObj1Col->OnCollision_Stay(pPart2.second, fTimedelta);
					pCol2->OnCollision_Stay(_pObj1, fTimedelta);
				}
			}
			else // 이번에 충돌
			{
				if (true == _pObj1->Is_Active() && true == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // 둘다 삭제될 예정이 아닐 때만 충돌 처리
				{
					_pObj1Col->OnCollision_Enter(pPart2.second, fTimedelta);
					pCol2->OnCollision_Enter(_pObj1, fTimedelta);
					iter->second = true;
				}
			}
		}
		else // 충돌 X
		{
			if (iter->second) // 이전에 충돌
			{
				_pObj1Col->OnCollision_Exit(pPart2.second, fTimedelta);
				pCol2->OnCollision_Exit(_pObj1, fTimedelta);
				iter->second = false;
			}
		}
	}

	for (auto& pPart1 : _pObj1->Get_Parts())
	{
		if (_pObj2 == pPart1.second || nullptr == _pObj2 || nullptr == pPart1.second) continue;

		pCol1 = dynamic_cast<CCollider*>(pPart1.second->Get_Component(TEXT("Com_Collider")));

		if (nullptr == _pObj2Col || nullptr == pCol1) continue;
		else if (false == _pObj2Col->Is_Active() || false == pCol1->Is_Active()) continue;

		Set_Info(iter, _pObj2Col, pCol1);

		if (_pObj2Col->IsCollision(pCol1)) // 충돌
		{
			if (iter->second) // 이전에도 충돌
			{
				if (false == _pObj2->Is_Active() || false == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active()) // 둘 중 하나 삭제 예정
				{
					_pObj2Col->OnCollision_Exit(pPart1.second, fTimedelta);
					pCol1->OnCollision_Exit(_pObj2, fTimedelta);
					iter->second = false;
				}
				else // 삭제 예정 없음
				{
					_pObj2Col->OnCollision_Stay(pPart1.second, fTimedelta);
					pCol1->OnCollision_Stay(_pObj2, fTimedelta);
				}
			}
			else // 이번에 충돌
			{
				if (true == _pObj2->Is_Active() && true == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active()) // 둘다 삭제될 예정이 아닐 때만 충돌 처리
				{
					_pObj2Col->OnCollision_Enter(pPart1.second, fTimedelta);
					pCol1->OnCollision_Enter(_pObj2, fTimedelta);
					iter->second = true;
				}
			}
		}
		else // 충돌 X
		{
			if (iter->second) // 이전에 충돌
			{
				_pObj2Col->OnCollision_Exit(pPart1.second, fTimedelta);
				pCol1->OnCollision_Exit(_pObj2, fTimedelta);
				iter->second = false;
			}
		}
	}


	for (auto& pPart1 : _pObj1->Get_Parts())
	{
		for (auto& pPart2 : _pObj2->Get_Parts())
		{
			if (pPart1.second == pPart2.second || nullptr == pPart1.second || nullptr == pPart2.second) continue;

			pCol1 = dynamic_cast<CCollider*>(pPart1.second->Get_Component(TEXT("Com_Collider")));
			pCol2 = dynamic_cast<CCollider*>(pPart2.second->Get_Component(TEXT("Com_Collider")));

			if (nullptr == pCol1 || nullptr == pCol2) continue;
			else if (false == pCol1->Is_Active() || false == pCol2->Is_Active()) continue;

			Set_Info(iter, pCol1, pCol2);

			if (pCol1->IsCollision(pCol2)) // 충돌
			{
				if (iter->second) // 이전에도 충돌
				{
					if (false == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active() || 
						false == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // 둘 중 하나 삭제 예정
					{
						pCol1->OnCollision_Exit(pPart2.second, fTimedelta);
						pCol2->OnCollision_Exit(pPart1.second, fTimedelta);
						iter->second = false;
					}
					else // 삭제 예정 없음
					{
						pCol1->OnCollision_Stay(pPart2.second, fTimedelta);
						pCol2->OnCollision_Stay(pPart1.second, fTimedelta);
					}
				}
				else // 이번에 충돌
				{
					if (true == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active() && 
						true == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // 둘다 삭제될 예정이 아닐 때만 충돌 처리
					{
						pCol1->OnCollision_Enter(pPart2.second, fTimedelta);
						pCol2->OnCollision_Enter(pPart1.second, fTimedelta);
						iter->second = true;
					}
				}
			}
			else // 충돌 X
			{
				if (iter->second) // 이전에 충돌
				{
					pCol1->OnCollision_Exit(pPart2.second, fTimedelta);
					pCol2->OnCollision_Exit(pPart1.second, fTimedelta);
					iter->second = false;
				}
			}
		}
	}
}

void CCollideManager::Set_Info(CHECKITER& _iter, CCollider* _pCol1, CCollider* _pCol2)
{
	COLLIDER_ID ID;

	ID.iLeft_id = _pCol1->Get_ID();
	ID.iRight_id = _pCol2->Get_ID();

	// 이전에 충돌 검사 여부 조사
	_iter = m_mapColInfo.find(ID.ID);

	// 없다면 새로 추가
	if (m_mapColInfo.end() == _iter)
	{
		m_mapColInfo.insert({ ID.ID, false });
		_iter = m_mapColInfo.find(ID.ID);
	}
}

void CCollideManager::Free()
{
	__super::Free();
}
