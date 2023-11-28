#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Idle_Shield.h"
#include "Transform.h"

#include "Enemy_Shield.h"

CState_Idle_Shield::CState_Idle_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Idle_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Idle_Shield::Tick(const _float& fTimeDelta)
{
	if (true == m_pOwnerBodyPart->IsAnimChange())
		return m_eState;

	STATE eState = m_eState;

	if (false == dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Is_LookPlayer())
	{
		_float fLookDist = 15.f;
		if (fLookDist >= dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
			dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);
	}

	_float fActDist = 10.f;
	if (fActDist >= dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
	{
		m_fIdleTime += fTimeDelta;

		if (0.1f <= m_fIdleTime)
		{
			_float fDist = 6.f;
			if (fDist <= dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
				return STATE::RUN;

			_float fWalkDist = 4.f;
			if (fWalkDist < dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
				return STATE::WALK;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				RELEASE_INSTANCE(CGameInstance);
					return STATE::WALK;
			}
		}
	}

	return eState;
}

STATE CState_Idle_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Idle_Shield::Reset_State()
{
	m_fIdleTime = 0.f;
}

void CState_Idle_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	_float fDist = 15.f;

	if (fDist >= dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);
	else
		dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(false);


	m_pOwnerBodyPart->Set_AnimationIndex(true, 24, 1.2f);
}

STATE CState_Idle_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Idle_Shield* CState_Idle_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Idle_Shield* pInstance = new CState_Idle_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Idle_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Idle_Shield::Free()
{
	__super::Free();
}
