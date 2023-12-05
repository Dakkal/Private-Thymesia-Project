#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Dead_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Dead_Halberd::CState_Dead_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Dead_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Dead_Halberd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pOwnerBodyPart->IsAnimationEnd())
		m_pRealOwner->Set_Dead(true);

	return eState;
}

STATE CState_Dead_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Dead_Halberd::Reset_State()
{
	m_bEnter = false;
}

void CState_Dead_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(false);
	
	dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 29, 1.2f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundFile(TEXT("NM_V_Halberds_LieDie.ogg"), CHANNELID::CHANNEL_5, 0.8f);

	RELEASE_INSTANCE(CGameInstance);

}

STATE CState_Dead_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Dead_Halberd* CState_Dead_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Dead_Halberd* pInstance = new CState_Dead_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Dead_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Dead_Halberd::Free()
{
	__super::Free();
}
