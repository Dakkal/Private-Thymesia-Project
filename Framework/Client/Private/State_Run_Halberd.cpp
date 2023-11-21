#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Run_Halberd.h"
#include "Transform.h"
#include "Navigation.h"

#include "Enemy_Halberd.h"

CState_Run_Halberd::CState_Run_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Run_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Run_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CTransform* pTransform = dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Get_PlayerTransform();
	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	if (nullptr == pTransform)
		return STATE::IDLE;

	if (false == m_pOwnerTransform->Chase(pTransform->Get_State(CTransform::STATE_POS), fTimeDelta, 6.f, pNavi))
	{
		return STATE::WALK;
	}

	return eState;
}

STATE CState_Run_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;




	return eState;
}

void CState_Run_Halberd::Reset_State()
{
	m_pOwnerTransform->Reset_Modulate_Speed();
}

void CState_Run_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 54, 1.2f);

	m_pOwnerTransform->Set_Modulate_Speed(3.f);
}

STATE CState_Run_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Run_Halberd* CState_Run_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Run_Halberd* pInstance = new CState_Run_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Run_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Run_Halberd::Free()
{
	__super::Free();
}
