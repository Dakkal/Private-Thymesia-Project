#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Excute.h"
#include "Input_Device.h"

#include "Seq_Camera_Player.h"

CState_Excute::CState_Excute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Excute::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Excute::Tick(const _float& fTimeDelta)
{
	if (false == g_PlayerSeq)
		g_PlayerSeq = true;

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(100))
	{
		m_pOwnerBodyPart->Set_CurAnimSpeed(0.5f);

	}
	else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(115))
	{
		m_pOwnerBodyPart->Set_CurAnimSpeed(1.8f);
	}


	if (true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		g_PlayerSeq = false;
		return STATE::IDLE;
	}


	return m_eState;
}

STATE CState_Excute::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Excute::Reset_State()
{
	m_bEnter = false;
}

void CState_Excute::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CPlayer*>(m_pRealOwner)->Reset_TargetEnemy();

	m_pOwnerBodyPart->Set_AnimationIndex(false, 176, 1.8f);


	dynamic_cast<CPlayer*>(m_pRealOwner)->Set_Excute(false);
}

STATE CState_Excute::Key_Input(const _float& fTimeDelta)
{
	

	return m_eState;
}

CState_Excute* CState_Excute::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Excute* pInstance = new CState_Excute(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Excute");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Excute::Free()
{
	__super::Free();
}
