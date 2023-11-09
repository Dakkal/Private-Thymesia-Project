#include "State.h"
#include "StateMachine.h"
#include "PartObject.h"
#include "Transform.h"

CState::CState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: m_pDevice(pDevice), m_pContext(pContext), m_pStateOwner(pOwner), m_eState(eState)
{
	m_pRealOwner = pOwner->Get_Owner();
}

HRESULT CState::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pRealOwner->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = m_pStateOwner->Get_Owner()->Get_Index_Parts(CGameObject::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);

	return S_OK;
}

void CState::Free()
{
	__super::Free();
}
