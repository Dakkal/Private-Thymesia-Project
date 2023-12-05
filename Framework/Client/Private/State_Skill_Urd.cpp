#include "pch.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Player.h"
#include "StateMachine.h"
#include "State_Skill_Urd.h"
#include "Transform.h"
#include "LandObject.h"
#include "PartObject.h"
#include "Throw_Weapon_Urd.h"

#include "Boss_Urd.h"

CState_Skill_Urd::CState_Skill_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
	: CState(pDevice, pContext, pOwner, eState)
{
}

HRESULT CState_Skill_Urd::Initialize()
{
	__super::Initialize();

	return S_OK;
}

STATE CState_Skill_Urd::Tick(const _float& fTimeDelta)
{
	STATE eState = m_eState;

	if (true == m_bSkil_Front)
		Skill_Front();
	if (true == m_bSkil_LeftRight)
		Skill_LeftRight();


	if (true == m_pOwnerBodyPart->IsAnimationEnd())
	{
		
		return STATE::IDLE;
	}

	return eState;
}

STATE CState_Skill_Urd::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_eState;



	return eState;
}

void CState_Skill_Urd::Reset_State()
{
	if (true == m_bLeft)
		m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L)->Set_Active(false);
	else if (true == m_bRight)
		m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_R)->Set_Active(true);

	m_pOwnerBodyPart->Set_Anim_TargetPos(XMVectorZero());
	m_bSkil_LeftRight = false ;
	m_bSkil_Front = false;

	m_bLeft = false;
	m_bRight = false;
	m_bCreateProjectile = false;
}

void CState_Skill_Urd::Enter_State()
{
	m_pRealOwner->Set_Move(true);

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(true);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->PlaySoundFile(TEXT("Urd_Skill01.ogg"), CHANNELID::CHANNEL_13, 0.5f);

	if (6.f <= dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerDistance())
	{
		if (true == pGameInstance->Random_Coin(0.7f))
		{
			m_bSkil_Front = true;

			m_pOwnerBodyPart->Set_AnimationIndex(false, 26, 1.5f);

		}
		else
		{
			m_bSkil_LeftRight = true;

			if (true == pGameInstance->Random_Coin(0.5f))
			{
				m_pOwnerBodyPart->Set_AnimationIndex(false, 28, 1.8f);
				m_bLeft = true;
			}
			else
			{
				m_pOwnerBodyPart->Set_AnimationIndex(false, 29, 1.8f);
				m_bRight = true;
			}
		}
	}
	else
	{
		m_bSkil_LeftRight = true;

		if (true == pGameInstance->Random_Coin(0.5f))
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 28, 1.8f);
			m_bLeft = true;
		}
		else
		{
			m_pOwnerBodyPart->Set_AnimationIndex(false, 29, 1.8f);
			m_bRight = true;
		}
	}

	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Reset_SkillCnt();

	_uint iRandom = pGameInstance->Random_Int(0, 3);
	dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_SkillActive(iRandom);

	RELEASE_INSTANCE(CGameInstance);	
}

STATE CState_Skill_Urd::Key_Input(const _float& fTimeDelta)
{
	return m_eState;
}

void CState_Skill_Urd::Skill_LeftRight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(35) && 28 == m_pOwnerBodyPart->Get_AnimationIndex())
	{
		m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L)->Set_Active(true);
	}

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(50))
	{
		_matrix ProjectileMatrix;
		CTransform* pPlayerTransform;

		if (true == m_bLeft && false == m_bCreateProjectile)
		{
			pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction10.ogg"), CHANNELID::CHANNEL_14, 1.f);

			m_bCreateProjectile = true;

			ProjectileMatrix = dynamic_cast<CPartObject*>(m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L))->Get_Part_WorldMatrix();
			pPlayerTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();


			CThrow_Weapon_Urd* pThrow_Weapon = CThrow_Weapon_Urd::Create(m_pDevice, m_pContext, ProjectileMatrix, pPlayerTransform);
			pGameInstance->Direct_Add_GameObject(LEVEL_GAMEPLAY, LAYER_PROJECTILE, pThrow_Weapon);
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_Projectiles()->push_back(pThrow_Weapon);

			m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L)->Set_Active(false);

		}
		else if (true == m_bRight && false == m_bCreateProjectile)
		{
			pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction10.ogg"), CHANNELID::CHANNEL_14, 1.f);

			m_bCreateProjectile = true;

			ProjectileMatrix = dynamic_cast<CPartObject*>(m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_R))->Get_Part_WorldMatrix();
			pPlayerTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();

			CThrow_Weapon_Urd* pThrow_Weapon = CThrow_Weapon_Urd::Create(m_pDevice, m_pContext, ProjectileMatrix, pPlayerTransform);
			pGameInstance->Direct_Add_GameObject(LEVEL_GAMEPLAY, LAYER_PROJECTILE, pThrow_Weapon);
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_Projectiles()->push_back(pThrow_Weapon);

			m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_R)->Set_Active(false);
		}


	}

	RELEASE_INSTANCE(CGameInstance);
}

void CState_Skill_Urd::Skill_Front()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	
	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(30))
	{
		CTransform* pTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();
		if (nullptr != pTransform)
			m_pOwnerBodyPart->Set_Anim_TargetPos(pTransform->Get_State(CTransform::STATE_POS));

		m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L)->Set_Active(true);

		m_pRealOwner->Set_Attack(true);

		pGameInstance->PlaySoundFile(TEXT("CV_Urd_AtkAction10.ogg"), CHANNELID::CHANNEL_14, 1.f);
	}

	if (true == m_pOwnerBodyPart->Is_AnimCurKeyFrame(60))
	{
		dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Set_LookPlayer(false);

		_matrix ProjectileMatrix;
		CTransform* pPlayerTransform;

		if (false == m_bCreateProjectile)
		{
			m_bCreateProjectile = true;
			m_pRealOwner->Set_Attack(false);

			ProjectileMatrix = dynamic_cast<CPartObject*>(m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L))->Get_Part_WorldMatrix();
			pPlayerTransform = dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_PlayerTransform();


			CThrow_Weapon_Urd* pThrow_Weapon = CThrow_Weapon_Urd::Create(m_pDevice, m_pContext, ProjectileMatrix);
			pGameInstance->Direct_Add_GameObject(LEVEL_GAMEPLAY, LAYER_PROJECTILE, pThrow_Weapon);
			dynamic_cast<CBoss_Urd*>(m_pRealOwner)->Get_Projectiles()->push_back(pThrow_Weapon);

			m_pRealOwner->Get_Index_Parts(CGameObject::PARTS::WEAPON_L)->Set_Active(false);

		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

CState_Skill_Urd* CState_Skill_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState)
{
	CState_Skill_Urd* pInstance = new CState_Skill_Urd(pDevice, pContext, pOwner, eState);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CState_Skill_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Skill_Urd::Free()
{
	__super::Free();
}
