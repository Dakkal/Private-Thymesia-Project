#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Lockon_Parry.h"
#include "Input_Device.h"

CState_Lockon_Parry::CState_Lockon_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Lockon_Parry::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Lockon_Parry::Tick(const _float& fTimeDelta)
{
	CGameObject* pTarget = dynamic_cast<CPlayer*>(m_pRealOwner)->Get_TargetEnemy();
	if (nullptr == pTarget)
		return STATE::IDLE;

	if (true == m_pRealOwner->Is_Hit())
	{
		if (true == dynamic_cast<CPlayer*>(m_pRealOwner)->Is_Parry())
		{
			return STATE::LOCK_PARRY_SUCCESS;
		}
		else
			return STATE::HIT;
	}

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


	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_bIdle && m_pOwnerBodyPart->Is_AnimCurKeyFrame(120))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_IDLE;
	}
	else if (true == m_bAttack && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_ATTACK;
	}
	else if (true == m_bAvoid && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_AVOID;
	}
	else if (true == m_bWalk && false == m_bIdle && false == m_IsKeepParry && true == m_pOwnerBodyPart->Is_AnimOverKeyFrame(35))
	{
		RELEASE_INSTANCE(CGameInstance);
		return STATE::LOCK_WALK;
	}
	else if (true == m_bParry2 && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 45, 3.f);
		m_IsKeepParry = false;
	}
	else if (true == m_bParryRe && true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 3.f, false, 10);
		m_IsKeepParry = false;
	}
	else
		eState = m_eState;

	if (5 <= m_pOwnerBodyPart->Get_CurKeyFrameNumb() &&
		25 >= m_pOwnerBodyPart->Get_CurKeyFrameNumb())
	{
		dynamic_cast<CPlayer*>(m_pRealOwner)->Set_Parry(true);
	}
	else
	{
		dynamic_cast<CPlayer*>(m_pRealOwner)->Set_Parry(false);
	}


	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

STATE CState_Lockon_Parry::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIMouseState(CInput_Device::MOUSEKEY_STATE::LBUTTON) & 0x80)
	{
		m_bAttack = true;
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_bAttack = false;
		m_bAvoid = true;
		m_bWalk = false;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_D) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_S) & 0x80 ||
		pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_bAttack = false;
		m_bAvoid = false;
		m_bWalk = true;
		m_bIdle = false;
		m_IsKeepParry = false;
	}
	else
	{
		m_bAttack = false;
		m_bAvoid = false;
		m_bWalk = false;
		m_bIdle = true;
		m_IsKeepParry = false;
	}

	if (true == m_bParry1 && 44 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80 ||
		true == m_bParryRe && 44 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		m_bParry2 = true;
		m_bParry1 = false;
		m_bParryRe = false;
		m_IsKeepParry = true;
	}
	else if (true == m_bParry2 && 45 == m_pOwnerBodyPart->Get_AnimationIndex() &&
		pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
	{
		m_bParry2 = false;
		m_bParryRe = true;
		m_IsKeepParry = true;
	}



	RELEASE_INSTANCE(CGameInstance);

	return eState;
}

void CState_Lockon_Parry::Enter_State()
{
	m_pRealOwner->Set_Move(false);

	m_pOwnerBodyPart->Set_AnimationIndex(false, 44, 3.f);
	m_bParry1 = true;
}

void CState_Lockon_Parry::Reset_State()
{
	m_bAttack = false;
	m_bAvoid = false;
	m_bWalk = false;
	m_bIdle = true;
	m_IsKeepParry = false;
	m_bParry1 = false;
	m_bParry2 = false;
	m_bParryRe = false;
}

STATE CState_Lockon_Parry::Key_Input(const _float& fTimeDelta)
{
	return STATE();
}


CState_Lockon_Parry* CState_Lockon_Parry::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Lockon_Parry* pInstance = new CState_Lockon_Parry(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Lockon_Parry");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Lockon_Parry::Free()
{
	__super::Free();
}