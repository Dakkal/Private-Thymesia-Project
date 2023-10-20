#include "pch.h"
#include "State_Idle.h"

CState_Idle::CState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle::Initialize()
{
	return S_OK;
}

STATE CState_Idle::Tick(const _float& fTimeDelta)
{
	if (false == m_bEnter)
	{
		m_bEnter = true;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Idle::LateTick(const _float& fTimeDelta)
{
	return m_eState;
}

void CState_Idle::Reset_State()
{
	m_bEnter = false;

}

STATE CState_Idle::Key_Input(const _float& fTimeDelta)
{



	return m_eState;
}

CState_Idle* CState_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle* pInstance = new CState_Idle(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle::Free()
{
	__super::Free();
}
