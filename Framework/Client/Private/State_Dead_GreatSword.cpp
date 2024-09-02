#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Dead_GreatSword.h"
#include "Transform.h"

#include "Enemy_GreatSword.h"

CState_Dead_GreatSword::CState_Dead_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Dead_GreatSword::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Dead_GreatSword::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
		m_pRealOwner->Set_Dead(true);

	return eState;
}

STATE CState_Dead_GreatSword::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Dead_GreatSword::Reset_State()
{
	m_bEnter = false;
}

void CState_Dead_GreatSword::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	m_pRealOwner->Get_Component(TEXT("Com_Colider"))->Set_Active(false);
	
	dynamic_cast<CEnemy_GreatSword*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 24, 1.2f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundFile(TEXT("NM_V_GreatSword_HurtXL.ogg"), CHANNELID::CHANNEL_3, 0.8f);
		
	RELEASE_INSTANCE(CGameInstance);
}

STATE CState_Dead_GreatSword::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Dead_GreatSword* CState_Dead_GreatSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Dead_GreatSword* pInstance = new CState_Dead_GreatSword(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Dead_GreatSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Dead_GreatSword::Free()
{
	__super::Free();
}
