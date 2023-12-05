#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Run_Shield.h"
#include "Transform.h"
#include "Navigation.h"

#include "Enemy_Shield.h"

CState_Run_Shield::CState_Run_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Run_Shield::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Run_Shield::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CTransform* pTransform = dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Get_PlayerTransform();
	CNavigation* pNavi = dynamic_cast<CLandObject*>(m_pRealOwner)->Get_CurNaviCom();

	if (nullptr == pTransform)
		return STATE::IDLE;

	if (false == m_pOwnerTransform->Chase(pTransform->Get_State(CTransform::STATE_POS), fTimeDelta, 6.f, pNavi))
	{
		return STATE::WALK;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (1 == m_pRealOwner->Get_Index())
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
			pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Magician_01.ogg"), CHANNELID::CHANNEL_7, 1.f);
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
			pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Magician_02.ogg"), CHANNELID::CHANNEL_8, 1.f);
	}
	else
	{
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(10))
			pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Magician_01.ogg"), CHANNELID::CHANNEL_9, 1.f);
		if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(20))
			pGameInstance->CheckPlaySoundFile(TEXT("FootStep_Magician_02.ogg"), CHANNELID::CHANNEL_10, 1.f);
	}

	

	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Run_Shield::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;




	return eState;
}

void CState_Run_Shield::Reset_State()
{
	m_pOwnerTransform->Reset_Modulate_Speed();
}

void CState_Run_Shield::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CEnemy_Shield*>(m_pRealOwner)->Set_LookPlayer(true);

	m_pOwnerBodyPart->Set_AnimationIndex(true, 29, 1.2f);

	m_pOwnerTransform->Set_Modulate_Speed(3.f);
}

STATE CState_Run_Shield::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

CState_Run_Shield* CState_Run_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Run_Shield* pInstance = new CState_Run_Shield(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Run_Shield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Run_Shield::Free()
{
	__super::Free();
}
