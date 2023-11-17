#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Halberd.h"
#include "Transform.h"

#include "Enemy_Halberd.h"

CState_Idle_Halberd::CState_Idle_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Halberd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Halberd::Tick(const _float& fTimeDelta)
{
	if (true == m_pOwnerBodyPart->IsAnimChange())
		return m_eState;

	STATE eState = m_eState;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	m_fIdleTime += fTimeDelta;

	if (0.1f <= m_fIdleTime)
	{
		_float fDist = 6.f;
		if (fDist <= dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Get_PlayerDistance())
			return STATE::RUN;

		_float fWalkDist = 4.f;
		if (fWalkDist < dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Get_PlayerDistance())
			return STATE::WALK;
		else
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (true == pGameInstance->Random_Coin(0.8f))
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::WALK;
			}
			else
			{
				RELEASE_INSTANCE(CGameInstance);
				return STATE::AVOID;
			}
		}
	}

	return eState;
}

STATE CState_Idle_Halberd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Idle_Halberd::Reset_State()
{
	m_fIdleTime = 0.f;
}

void CState_Idle_Halberd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	_float fDist = 20.f;

	if (fDist >= dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Get_PlayerDistance())
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(true);
	else
		dynamic_cast<CEnemy_Halberd*>(m_pRealOwner)->Set_LookPlayer(false);


	m_pOwnerBodyPart->Set_AnimationIndex(true, 51, 1.2f);
}

STATE CState_Idle_Halberd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Halberd* CState_Idle_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Halberd* pInstance = new CState_Idle_Halberd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Halberd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Halberd::Free()
{
	__super::Free();
}
