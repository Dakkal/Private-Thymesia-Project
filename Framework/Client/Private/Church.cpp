#include "pch.h"
#include "..\Public\Church.h"
#include "GameInstance.h"
#include "Navigation.h"

CChurch::CChurch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CChurch::CChurch(const CChurch& rhs)
	: CLandObject(rhs)
{
}

HRESULT CChurch::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("Church");

	return S_OK;
}

HRESULT CChurch::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POS, _vector(-5.f, 0.f, 10.f, 1.f));

	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CChurch::PriorityTick(_float fTimeDelta)
{
	if(nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CChurch::Tick(_float fTimeDelta)
{
}

void CChurch::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	if (nullptr != m_pCurNavigationCom)
		m_pRendererCom->Add_Debug(m_pCurNavigationCom);
#endif

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CChurch::Render()
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

HRESULT CChurch::Ready_Components()
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
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Church"),
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Church"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Church_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
		return E_FAIL;

	m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CChurch::Bind_ShaderResources()
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

CChurch* CChurch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CChurch* pInstance = new CChurch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CChurch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChurch::Clone(void* pArg)
{
	__super::Clone(pArg);

	CChurch* pInstance = new CChurch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChurch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChurch::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
