#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Walk_Shield.h"
#include "Transform.h"
#include "Navigation.h"

#include "Enemy_Shield.h"

CState_Walk_Shield::CState_Walk_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Walk_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Walk_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	_float fDist = 6.f;
	if (fDist <= dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
		return STATE::RUN;

	if(true == m_bAttack)
		return STATE::ATTACK;

	if(nullptr == dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerTransform())
		return STATE::IDLE;

	eState = WalkState(fTimeDelta);

	return eState;
}

STATE CState_Walk_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	


	return eState;
}

void CState_Walk_Shield::Reset_State()
{
	if (true == m_bAttack)
	{
		m_fAttackTime = 0.f;
		m_bAttack = false;
	}
		
	m_bGoRight = false;
	m_bGoLeft = false;
}

void CState_Walk_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	_float fWalkDist = 4.f;
	if (fWalkDist < dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerDistance())
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 41, 1.2f);
	}
	else
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (false == m_bGoRight && false == m_bGoLeft)
		{
			if (true == pGameInstance->Random_Coin(0.6f))
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 45, 1.2f);
				m_bGoLeft = true;
			}
			else
			{
				m_pOwnerBodyPart->Set_AnimationIndex(true, 46, 1.2f);
				m_bGoRight = true;
			}
				
		}

		RELEASE_INSTANCE(CGameInstance);
	}
		
}

STATE CState_Walk_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Walk_Shield::WalkState(_float fTimeDelta)
{
	CTransform* pTransform = dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerTransform();
	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	if (nullptr == pTransform)
		return STATE::IDLE;

	if (false == m_pOwnerTransform->Chase(pTransform->Get_State(CTransform::STATE_POS), fTimeDelta, 4.f, pNavi))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (false == m_bGoRight && false == m_bGoLeft)
		{
			if (true == pGameInstance->Random_Coin(0.6f))
			{
				m_bGoLeft = true;
			}
			else
				m_bGoRight = true;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		m_bGoRight = false;
		m_bGoLeft = false;

		if (false == m_pOwnerBodyPart->IsAnimChange())
			m_pOwnerBodyPart->Set_AnimationIndex(true, 41, 1.2f);
	}

	if (true == m_bGoRight)
	{
		if (false == m_pOwnerBodyPart->IsAnimChange())
			m_pOwnerBodyPart->Set_AnimationIndex(true, 46, 1.2f);

		m_pOwnerTransform->Go_Right(fTimeDelta, pNavi);

		m_fAttackTime += fTimeDelta;
	}
	else if (true == m_bGoLeft)
	{
		if (false == m_pOwnerBodyPart->IsAnimChange())
			m_pOwnerBodyPart->Set_AnimationIndex(true, 45, 1.2f);

		m_pOwnerTransform->Go_Right(-fTimeDelta, pNavi);

		m_fAttackTime += fTimeDelta;
	}

	if (3.f <= m_fAttackTime)
	{
		m_bAttack = true;
	}

	return m_eState;
}

CState_Walk_Shield* CState_Walk_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Walk_Shield* pInstance = new CState_Walk_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Walk_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Walk_Shield::Free()
{
	__super::Free();
}
