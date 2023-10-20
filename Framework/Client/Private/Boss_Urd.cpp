#include "pch.h"
#include "..\Public\Boss_Urd.h"

#include "GameInstance.h"
#include "PartObject.h"

CBoss_Urd::CBoss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CBoss_Urd::CBoss_Urd(const CBoss_Urd& rhs)
	: CGameObject(rhs)
{

}

HRESULT CBoss_Urd::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::BOSS;
	m_strObjectName = TEXT("Boss_Urd");

	return S_OK;
}

HRESULT CBoss_Urd::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POS, _vector(rand() % 20, 0.f, rand() % 20, 1.f));

	return S_OK;
}

void CBoss_Urd::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance)

		for (auto& pPart : m_Parts)
		{
			if (nullptr != pPart)
				pPart->Tick(fTimeDelta);
		}
}

void CBoss_Urd::LateTick(_float fTimeDelta)
{
	for (auto& pPart : m_Parts)
	{
		if (nullptr != pPart)
			pPart->LateTick(fTimeDelta);
	}
}

HRESULT CBoss_Urd::Render()
{
	return S_OK;
}

HRESULT CBoss_Urd::Ready_Components()
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

HRESULT CBoss_Urd::Ready_PlayerParts()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pParts = nullptr;

	/* For.Part_Body */
	CPartObject::PART_DESC			PartDesc_Body;
	PartDesc_Body.pParentTransform = m_pTransformCom;

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_Body"), &PartDesc_Body);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.push_back(pParts);

	/* For.Part_Weapon */
	CPartObject::PART_DESC			PartDesc_Weapon;
	PartDesc_Weapon.pParentTransform = m_pTransformCom;
	PartDesc_Weapon.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[(_uint)PARTS::BODY])->Get_SocketBonePtr("weapon_r");
	PartDesc_Weapon.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[(_uint)PARTS::BODY])->Get_SocketPivotMatrix();

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_Weapon"), &PartDesc_Weapon);
	if (nullptr == pParts)
		return E_FAIL;
	m_Parts.push_back(pParts);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBoss_Urd* CBoss_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CBoss_Urd* pInstance = new CBoss_Urd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CBoss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Urd::Clone(void* pArg)
{
	__super::Clone(pArg);

	CBoss_Urd* pInstance = new CBoss_Urd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Urd::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);
	m_Parts.clear();

	Safe_Release(m_pTransformCom);
}