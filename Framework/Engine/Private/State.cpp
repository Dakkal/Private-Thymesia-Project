#include "State.h"
#include "StateMachine.h"

CState::CState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: m_pDevice(pDevice), m_pContext(pContext), m_pStateOwner(pOwner), m_eState(eState)
{
	m_pRealOwner = pOwner->Get_Owner();
}

void CState::Free()
{
	__super::Free();
}
