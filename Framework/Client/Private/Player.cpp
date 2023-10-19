#include "pch.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PartObject.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PLAYER;
	m_strObjectName = TEXT("Player");

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		//m_pTransformCom->Go_Backward(fTimeDelta);
		--iIndex;
		if (0 > iIndex)
			iIndex = 0;

		dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Set_AnimationIndex(true, iIndex);
	}

	if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
	{
		//m_pTransformCom->Go_Forward(fTimeDelta);
		++iIndex;
		if (iIndex > dynamic_cast<CBinModel*>(m_PlayerParts[(_uint)PARTS::BODY]->Get_Component(TEXT("Com_Model")))->Get_NumAnim())
			iIndex = dynamic_cast<CBinModel*>(m_PlayerParts[(_uint)PARTS::BODY]->Get_Component(TEXT("Com_Model")))->Get_NumAnim();

		dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Set_AnimationIndex(true, iIndex);
	}
	
	RELEASE_INSTANCE(CGameInstance);
	
	for (auto& pPart : m_PlayerParts)
	{
		if (nullptr != pPart)
			pPart->Tick(fTimeDelta);
	}
}

void CPlayer::LateTick(_float fTimeDelta)
{
	for (auto& pPart : m_PlayerParts)
	{
		if (nullptr != pPart)
			pPart->LateTick(fTimeDelta);
	}
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pPlayerParts = nullptr;

	/* For.Part_Body */
	CPartObject::PART_DESC			PartDesc_Body;
	PartDesc_Body.pParentTransform = m_pTransformCom;

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Body"), &PartDesc_Body);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_PlayerParts.push_back(pPlayerParts);

	/* For.Part_Weapon */
	CPartObject::PART_DESC			PartDesc_Weapon_Saber;
	PartDesc_Weapon_Saber.pParentTransform = m_pTransformCom;
	PartDesc_Weapon_Saber.pSocketBone = dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Get_SocketBonePtr("weapon_r");
	PartDesc_Weapon_Saber.SocketPivot = dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Get_SocketPivotMatrix();

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Weapon_Saber"), &PartDesc_Weapon_Saber);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_PlayerParts.push_back(pPlayerParts);

	CPartObject::PART_DESC			PartDesc_Weapon_Dagger;
	PartDesc_Weapon_Dagger.pParentTransform = m_pTransformCom;
	PartDesc_Weapon_Dagger.pSocketBone = dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Get_SocketBonePtr("weapon_l");
	PartDesc_Weapon_Dagger.SocketPivot = dynamic_cast<CPartObject*>(m_PlayerParts[(_uint)PARTS::BODY])->Get_SocketPivotMatrix();

	pPlayerParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Player_Weapon_Dagger"), &PartDesc_Weapon_Dagger);
	if (nullptr == pPlayerParts)
		return E_FAIL;
	m_PlayerParts.push_back(pPlayerParts);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);
	m_PlayerParts.clear();

	Safe_Release(m_pTransformCom);
}