#include "pch.h"
#include "..\Public\BossRoom.h"
#include "GameInstance.h"
#include "Navigation.h"

CBossRoom::CBossRoom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CBossRoom::CBossRoom(const CBossRoom& rhs)
	: CLandObject(rhs)
{
}

HRESULT CBossRoom::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("BossRoom");

	return S_OK;
}

HRESULT CBossRoom::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBossRoom::PriorityTick(_float fTimeDelta)
{
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CBossRoom::Tick(_float fTimeDelta)
{
}

void CBossRoom::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->IsIn_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POS), 50.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);

#ifdef _DEBUG
		if (nullptr != m_pCurNavigationCom)
			m_pRendererCom->Add_Debug(m_pCurNavigationCom);
#endif
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBossRoom::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBossRoom::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

#ifdef _DEBUG
	///* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_BossRoom"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;
#else
	///* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Church"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;

	///* Com_Navigation */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Church_Navigation"),
	//	TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
	//	return E_FAIL;
#endif // !NDEBUG
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossRoom"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BossRoom_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
		return E_FAIL;

	m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CBossRoom::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ViewMatrix", CPipeLine::D3DTS_VIEW)))
		return E_FAIL;

	if (FAILED(pGameInstance->Bind_TransformToShader(m_pShaderCom, "g_ProjMatrix", CPipeLine::D3DTS_PROJ)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBossRoom* CBossRoom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CBossRoom* pInstance = new CBossRoom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CBossRoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossRoom::Clone(void* pArg)
{
	__super::Clone(pArg);

	CBossRoom* pInstance = new CBossRoom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBossRoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossRoom::Free()
{
	__super::Free();


	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
