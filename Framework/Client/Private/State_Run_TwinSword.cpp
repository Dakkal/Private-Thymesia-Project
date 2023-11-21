#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Run_TwinSword.h"
#include "Transform.h"
#include "Navigation.h"

#include "Enemy_TwinSword.h"

CState_Run_TwinSword::CState_Run_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Run_TwinSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Run_TwinSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CTransform* pTransform = dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Get_PlayerTransform();
	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	if (nullptr == pTransform)
		return STATE::IDLE;

	if (false == m_pOwnerTransform->Chase(pTransform->Get_State(CTransform::STATE_POS), fTimeDelta, 6.f, pNavi))
	{
		return STATE::WALK;
	}

	return eState;
}

STATE CState_Run_TwinSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;




	return eState;
}

void CState_Run_TwinSword::Reset_State()
{
	m_pOwnerTransform->Reset_Modulate_Speed();
}

void CState_Run_TwinSword::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_TwinSword*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 37, 1.f);

	m_pOwnerTransform->Set_Modulate_Speed(3.f);
}

STATE CState_Run_TwinSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Run_TwinSword* CState_Run_TwinSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Run_TwinSword* pInstance = new CState_Run_TwinSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Run_TwinSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Run_TwinSword::Free()
{
	__super::Free();
}
