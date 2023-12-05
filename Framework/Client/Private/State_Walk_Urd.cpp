#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Walk_Urd.h"
#include "Transform.h"
#include "Navigation.h"

#include "Boss_Urd.h"

CState_Walk_Urd::CState_Walk_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Walk_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Walk_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_pRealOwner->Is_Hit())
		return STATE::HIT;

	if (true == m_bAttack)
	{
		_float fMinSkillDist = 4.f; _float fMaxSkillDist = 10.f;

		if (fMinSkillDist <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			fMaxSkillDist >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance() &&
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillCnt() >= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_SkillActive())
		{
			return STATE::SKILL;
		}

		_float fMinRushDist = 4.f;
		if (fMinRushDist < dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance())
			return STATE::RUN;


		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (true == pGameInstance->Random_Coin(0.8f))
		{
			RELEASE_INSTANCE(CGameInstance);

			return STATE::ATTACK;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);

			return STATE::AVOID;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
		
	if (nullptr == dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform())
		return STATE::IDLE;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(5))
		pGameInstance->CheckPlaySoundFile(TEXT("FootStep_HighHeel_01.ogg"), CHANNELID::CHANNEL_13, 1.f);
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(25))
		pGameInstance->CheckPlaySoundFile(TEXT("FootStep_HighHeel_02.ogg"), CHANNELID::CHANNEL_13, 1.f);

	RELEASE_INSTANCE(CGameInstance);

	eState = WalkState(fTimeDelta);

	return eState;
}

STATE CState_Walk_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;




	return eState;
}

void CState_Walk_Urd::Reset_State()
{
	if (true == m_bAttack)
	{
		m_fAttackTime = 0.f;
		m_bAttack = false;
	}

	m_bGoRight = false;
	m_bGoLeft = false;
}

void CState_Walk_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->Random_Coin(0.6f))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 48, 1.2f);
		m_bGoLeft = true;
	}
	else
	{
		m_pOwnerBodyPart->Set_AnimationIndex(true, 49, 1.2f);
		m_bGoRight = true;
	}

	RELEASE_INSTANCE(CGameInstance);

}

STATE CState_Walk_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

STATE CState_Walk_Urd::WalkState(_float fTimeDelta)
{
	CTransform* pTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	if (true == m_bGoRight)
	{
		m_pOwnerTransform->Go_Right(fTimeDelta, pNavi);

		m_fAttackTime += fTimeDelta;
	}
	else if (true == m_bGoLeft)
	{
		m_pOwnerTransform->Go_Right(-fTimeDelta, pNavi);

		m_fAttackTime += fTimeDelta;
	}

	if (1.5f <= m_fAttackTime)
	{
		m_bAttack = true;
	}


	return m_eState;
}

CState_Walk_Urd* CState_Walk_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Walk_Urd* pInstance = new CState_Walk_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Walk_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Walk_Urd::Free()
{
	__super::Free();
}
