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
	
	for (auto& iter1 : *Obj1)
	{
		for (auto& iter2 : *Obj2)
		{
		
			if (iter1 == iter2 || nullptr == iter1 || nullptr == iter2) continue;

			pCol1 = dynamic_cast<CCollider*>(iter1->Get_Component(TEXT("Com_Collider")));
			pCol2 = dynamic_cast<CCollider*>(iter2->Get_Component(TEXT("Com_Collider")));

			if (nullptr == pCol1 || nullptr == pCol2) continue;

			Set_Info(iter, pCol1, pCol2);
			if (false == pCol1->Is_Active() || false == pCol2->Is_Active())
			{
				if (iter->second) // �������� �浹
				{
					pCol1->OnCollision_Exit(iter2, fTimedelta);
					pCol2->OnCollision_Exit(iter1, fTimedelta);
					iter->second = false;

					if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
					{
						Active_Part_Colide(iter1, iter2, false);
						Check_Part_Collision(iter1, iter2, pCol1, pCol2, fTimedelta);
					}
				}
				continue;
			}
			

			if (pCol1->IsCollision(pCol2)) // �浹
			{
				if (iter->second) // �������� �浹
				{
					if (false == iter1->Is_Active() || false == iter2->Is_Active()) // �� �� �ϳ� ���� ����
					{
						pCol1->OnCollision_Exit(iter2, fTimedelta);
						pCol2->OnCollision_Exit(iter1, fTimedelta);
						iter->second = false;

						if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
							Active_Part_Colide(iter1, iter2, false);
					}
					else // ���� ���� ����
					{
						pCol1->OnCollision_Stay(iter2, fTimedelta);
						pCol2->OnCollision_Stay(iter1, fTimedelta);

						if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
							Active_Part_Colide(iter1, iter2, true);
					}
				}
				else // �̹��� �浹
				{
					if (true == iter1->Is_Active() && true == iter2->Is_Active()) // �Ѵ� ������ ������ �ƴ� ���� �浹 ó��
					{
						pCol1->OnCollision_Enter(iter2, fTimedelta);
						pCol2->OnCollision_Enter(iter1, fTimedelta);
						iter->second = true;

						if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
							Active_Part_Colide(iter1, iter2, true);
					}
				}

				if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
				{
					Check_Part_Collision(iter1, iter2, pCol1, pCol2, fTimedelta);
				}
			}
			else // �浹 X
			{
				if (iter->second) // ������ �浹
				{
					pCol1->OnCollision_Exit(iter2, fTimedelta);
					pCol2->OnCollision_Exit(iter1, fTimedelta);
					iter->second = false;

					if (0 < iter1->Get_Parts_Size() || 0 < iter2->Get_Parts_Size())
					{
						Active_Part_Colide(iter1, iter2, false);
						Check_Part_Collision(iter1, iter2, pCol1, pCol2, fTimedelta);
					}
				}
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

	for (auto& pPart1 : _pObj1->Get_Parts())
	{
		for (auto& pPart2 : _pObj2->Get_Parts())
		{
			if (pPart1.second == pPart2.second || nullptr == pPart1.second || nullptr == pPart2.second) continue;

			pCol1 = dynamic_cast<CCollider*>(pPart1.second->Get_Component(TEXT("Com_Collider")));
			pCol2 = dynamic_cast<CCollider*>(pPart2.second->Get_Component(TEXT("Com_Collider")));

			if (nullptr == pCol1 || nullptr == pCol2) continue;

			Set_Info(iter, pCol1, pCol2);
			if (false == pCol1->Is_Active() || false == pCol2->Is_Active())
			{
				if (iter->second) // �������� �浹
				{
					pCol1->OnCollision_Exit(pPart2.second, fTimedelta);
					pCol2->OnCollision_Exit(pPart1.second, fTimedelta);
					iter->second = false;
				}
				continue;
			}
			
			if (pCol1->IsCollision(pCol2)) // �浹
			{
				if (iter->second) // �������� �浹
				{
					if (false == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active() || 
						false == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // �� �� �ϳ� ���� ����
					{
						pCol1->OnCollision_Exit(pPart2.second, fTimedelta);
						pCol2->OnCollision_Exit(pPart1.second, fTimedelta);
						iter->second = false;
					}
					else // ���� ���� ����
					{
						pCol1->OnCollision_Stay(pPart2.second, fTimedelta);
						pCol2->OnCollision_Stay(pPart1.second, fTimedelta);
					}
				}
				else // �̹��� �浹
				{
					if (true == dynamic_cast<CPartObject*>(pPart1.second)->Get_PartOwner()->Is_Active() && 
						true == dynamic_cast<CPartObject*>(pPart2.second)->Get_PartOwner()->Is_Active()) // �Ѵ� ������ ������ �ƴ� ���� �浹 ó��
					{
						pCol1->OnCollision_Enter(pPart2.second, fTimedelta);
						pCol2->OnCollision_Enter(pPart1.second, fTimedelta);
						iter->second = true;
					}
				}
			}
			else // �浹 X
			{
				if (iter->second) // ������ �浹
				{
					pCol1->OnCollision_Exit(pPart2.second, fTimedelta);
					pCol2->OnCollision_Exit(pPart1.second, fTimedelta);
					iter->second = false;
				}
			}
		}
	}
}

void CCollideManager::Active_Part_Colide(CGameObject* _pObj1, CGameObject* _pObj2, _bool IsColide)
{
	CComponent* pCol1 = nullptr;
	CComponent* pCol2 = nullptr;

	for (auto& pPart1 : _pObj1->Get_Parts())
	{
		if (nullptr == pPart1.second) continue;

		if (pCol1 = pPart1.second->Get_Component(TEXT("Com_Collider")))
		{
			pCol1->Set_Active(IsColide);
		}
	}
	for (auto& pPart2 : _pObj2->Get_Parts())
	{
		if (nullptr == pPart2.second) continue;

		if (pCol2 = pPart2.second->Get_Component(TEXT("Com_Collider")))
		{
			pCol2->Set_Active(IsColide);
		}
	}
}

void CCollideManager::Set_Info(CHECKITER& _iter, CCollider* _pCol1, CCollider* _pCol2)
{
	COLLIDER_ID ID;

	ID.iLeft_id = _pCol1->Get_ID();
	ID.iRight_id = _pCol2->Get_ID();

	// ������ �浹 �˻� ���� ����
	_iter = m_mapColInfo.find(ID.ID);

	// ���ٸ� ���� �߰�
	if (m_mapColInfo.end() == _iter)
	{
		m_mapColInfo.emplace(ID.ID, false);
		_iter = m_mapColInfo.find(ID.ID);
	}
}

void CCollideManager::Free()
{
	__super::Free();
}
