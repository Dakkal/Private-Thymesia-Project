#include "pch.h"
#include "..\Public\Boss_Urd.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "StateMachine.h"

#include "Collider.h"
#include "Bounding_Sphere.h"

CBoss_Urd::CBoss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{

}

CBoss_Urd::CBoss_Urd(const CBoss_Urd& rhs)
	: CLandObject(rhs)
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

	return S_OK;
}

void CBoss_Urd::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->Tick(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	RELEASE_INSTANCE(CGameInstance)
}

void CBoss_Urd::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_Parts)
	{
		if (nullptr != iter.second)
			iter.second->LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CBoss_Urd::Render()
{
#ifdef _DEBUG
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Render();

	m_pColliderCom->Render();
#endif // DEBUG

	return S_OK;
}

HRESULT CBoss_Urd::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotRadianPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation* pNavigation = { nullptr };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Church_Navigation"),
		TEXT("Com_Navigation1"), (CComponent**)&pNavigation)))
		return E_FAIL;
	m_NavigationComs.push_back(pNavigation);

	m_pCurNavigationCom = pNavigation;
	m_pCurNavigationCom->Set_toCell(6, m_pTransformCom);

	/* Com_Collider_Sphere */
	CBounding_Sphere::BOUNDING_SPHERE_DESC	SphereDesc = {};
	SphereDesc.fRadius = 3.f;
	SphereDesc.vCenter = _float3(0.f, 0.01f, 0.f);
	SphereDesc.vCollideColor = _vector(1.f, 0.f, 0.f, 1.f);
	SphereDesc.vColor = _vector(1.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &SphereDesc)))
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
	dynamic_cast<CPartObject*>(pParts)->Set_Owner(this);
	m_Parts.emplace(CGameObject::PARTS::BODY, pParts);

	/* For.Part_Weapon */
	CPartObject::PART_DESC			PartDesc_Weapon;
	PartDesc_Weapon.pParentTransform = m_pTransformCom;
	PartDesc_Weapon.pSocketBone = dynamic_cast<CPartObject*>(m_Parts[CPartObject::PARTS::BODY])->Get_SocketBonePtr("weapon_r");
	PartDesc_Weapon.SocketPivot = dynamic_cast<CPartObject*>(m_Parts[CPartObject::PARTS::BODY])->Get_SocketPivotMatrix();

	pParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Boss_Urd_Weapon"), &PartDesc_Weapon);
	if (nullptr == pParts)
		return E_FAIL;
	dynamic_cast<CPartObject*>(pParts)->Set_Owner(this);
	m_Parts.emplace(CGameObject::PARTS::WEAPON_R, pParts);

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

	for (auto& iter : m_Parts)
		Safe_Release(iter.second);
	m_Parts.clear();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pStateMachineCom);
}