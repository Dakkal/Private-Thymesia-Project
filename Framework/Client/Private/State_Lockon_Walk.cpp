#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Lockon_Walk.h"
#include "Input_Device.h"
#include "Player.h"

CState_Lockon_Walk::CState_Lockon_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Lockon_Walk::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Lockon_Walk::Tick(const _float& fTimeDelta)
{
	CComponent* pCom = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy()->Get_Component(TEXT("Com_Transform"));
	CTransform* pTargetTransform = dynamic_cast<CTransform*>(pCom);

	_vector vLook = m_pOwnerTransform->Get_State(CTransform::STATE_LOOK);
	_vector vDir = pTargetTransform->Get_State(CTransform::STATE_POS) - m_pOwnerTransform->Get_State(CTransform::STATE_POS);
	vDir.Normalize();
	vDir.y = 0.f;

	_float fRotMax = 66.f;
	_float fRotNormal = 20.f;

	_vector RotDir;

	if (3.f < acosf(vDir.Dot(vLook)))
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotMax);
	}
	else
	{
		RotDir = _vector::Lerp(vLook, vDir, fRotNormal * fTimeDelta);
	}
	m_pOwnerTransform->Set_Look(RotDir);


	STATE eState = Key_Input(fTimeDelta);

	return eState;
}

STATE CState_Lockon_Walk::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;


	return eState;
}

void CState_Lockon_Walk::Reset_State()
{
	m_bEnter = false;
}

void CState_Lockon_Walk::Enter_State()
{
	dynamic_cast<CPlayer*>(m_pRealOwner)->Search_TargetEnemy();



	m_pOwnerBodyPart->Set_AnimationIndex(true, 90, 1.2f);
}

STATE CState_Lockon_Walk::Key_Input(const _float& fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::WHEELBUTTON) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::IDLE;
	}
	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::AVOID;
	}
	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::ATTACK;
	}
	if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::PARRY;
	}
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::WALK;
	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

CState_Lockon_Walk* CState_Lockon_Walk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Lockon_Walk* pInstance = new CState_Lockon_Walk(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Lockon_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Lockon_Walk::Free()
{
	__super::Free();
}