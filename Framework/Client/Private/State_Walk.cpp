#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Walk.h"

CState_Walk::CState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Walk::Initialize()
{
	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pStateOwner->Get_Owner()->Get_Component(TEXT("Com_Transform")));
	CGameObject* pOwnerBodyPart = dynamic_cast<CPlayer*>(m_pStateOwner->Get_Owner())->Get_Parts(CPlayer::PARTS::BODY);
	m_pOwnerBodyPart = dynamic_cast<CPartObject*>(pOwnerBodyPart);

	return S_OK;
}

STATE CState_Walk::Tick(const _float& fTimeDelta)
{
	STATE eState = Key_Input(fTimeDelta);

	return eState;
}

STATE CState_Walk::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	return eState;
}

void CState_Walk::Enter_State()
{
	m_pOwnerBodyPart->Set_AnimationIndex(true, 126);
}

void CState_Walk::Reset_State()
{
	m_bEnter = false;
}

STATE CState_Walk::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pOwnerTransform->Go_Forward(fTimeDelta);
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 && pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pOwnerTransform->Go_Forward(fTimeDelta);
		m_pOwnerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pOwnerTransform->Go_Forward(fTimeDelta);
		RELEASE_INSTANCE(CGameInstance);
		return m_eState;
	}
	
	RELEASE_INSTANCE(CGameInstance);


	return STATE::IDLE;
}

CState_Walk* CState_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Walk* pInstance = new CState_Walk(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Walk::Free()
{
	__super::Free();
}
