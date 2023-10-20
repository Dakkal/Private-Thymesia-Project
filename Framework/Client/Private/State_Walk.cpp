#include "pch.h"
#include "State_Walk.h"

CState_Walk::CState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Walk::Initialize()
{
	return S_OK;
}

STATE CState_Walk::Tick(const _float& fTimeDelta)
{
	if (false == m_bEnter)
	{
		m_bEnter = true;
	}

	m_eState = Key_Input(fTimeDelta);

	return m_eState;
}

STATE CState_Walk::LateTick(const _float& fTimeDelta)
{
	return m_eState;
}

void CState_Walk::Reset_State()
{
	m_bEnter = false;

}

STATE CState_Walk::Key_Input(const _float& fTimeDelta)
{



	return m_eState;
}

CState_Walk* CState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Walk* pInstance = new CState_Walk(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Walk::Free()
{
	__super::Free();
}
