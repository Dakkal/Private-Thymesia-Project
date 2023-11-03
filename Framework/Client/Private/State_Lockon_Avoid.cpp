#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Lockon_Avoid.h"

CState_Lockon_Avoid::CState_Lockon_Avoid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Lockon_Avoid::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Lockon_Avoid::Tick(const _float& fTimeDelta)
{
	CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
	if (nullptr == pTarget)
		return STATE::IDLE;

	CComponent* pCom = pTarget->Get_Component(TEXT("Com_Transform"));
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

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pOwnerBodyPart->IsAnimationEnd())
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_IDLE;
	}
	else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
			pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_WALK;
		}
	}
	else if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(40))
	{
		if (pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_PARRY;
		}
		if (true == m_bIsAttack)
		{
			RELEASE_INSTANCE(CGameInstance);
			return STATE::LOCK_ATTACK;
		}

	}

	RELEASE_INSTANCE(CGameInstance);

	return m_eState;
}

STATE CState_Lockon_Avoid::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bIsAttack = true;
	}

	RELEASE_INSTANCE(CGameInstance);


	return eState;
}

void CState_Lockon_Avoid::Enter_State()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 61, 2.f);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 60, 2.f);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 62, 2.f);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 63, 2.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CState_Lockon_Avoid::Reset_State()
{
	m_bIsAttack = false;
}

STATE CState_Lockon_Avoid::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Lockon_Avoid* CState_Lockon_Avoid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Lockon_Avoid* pInstance = new CState_Lockon_Avoid(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Lockon_Avoid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Lockon_Avoid::Free()
{
	__super::Free();
}