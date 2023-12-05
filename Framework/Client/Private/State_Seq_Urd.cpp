#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Seq_Urd.h"
#include "Transform.h"
#include "LandObject.h"

#include "Boss_Urd.h"

CState_Seq_Urd::CState_Seq_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Seq_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Seq_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(1))
	{
		if (false == g_BossSeq)
		{
			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

			pGameInstace->StopSoundAll();

			RELEASE_INSTANCE(CGameInstance);

			g_BossSeq = true;
		}
	}

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(248))
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

		if (false == m_bPlayOnce1)
		{
			pGameInstace->CheckPlaySoundFile(TEXT("Urd_FightStart.ogg"), CHANNELID::CHANNEL_20, 1.f);

			m_bPlayOnce1 = true;
		}
		

		RELEASE_INSTANCE(CGameInstance);
	}

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(545))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_Seq1_State(false);
	}

	if (true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(850))
	{
		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

		if (false == m_bPlayOnce2)
		{
			pGameInstace->CheckPlaySoundFile(TEXT("CV_Urd_CutScene02.ogg"), CHANNELID::CHANNEL_22, 1.f);

			m_bPlayOnce2 = true;
		}
		

		RELEASE_INSTANCE(CGameInstance);
	}


	if (true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		g_BossSeq = false;

		CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

		pGameInstace->PlayBGM(TEXT("urd_music_2_A.ogg"), 0.3f);

		RELEASE_INSTANCE(CGameInstance);


		return STATE::IDLE;
	}

	return eState;
}

STATE CState_Seq_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Seq_Urd::Reset_State()
{
	m_bEnter = false;
}

void CState_Seq_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 24, 3.6f);
}

STATE CState_Seq_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Seq_Urd* CState_Seq_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Seq_Urd* pInstance = new CState_Seq_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Seq_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Seq_Urd::Free()
{
	__super::Free();
}
