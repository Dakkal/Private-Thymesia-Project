#include "pch.h"
#include "..\Public\Stair.h"
#include "GameInstance.h"
#include "Navigation.h"

CStair::CStair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CStair::CStair(const CStair& rhs)
	: CLandObject(rhs)
{
}

HRESULT CStair::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("Stair");

	return S_OK;
}

HRESULT CStair::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CStair::Enter_Object()
{
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CStair::PriorityTick(_float fTimeDelta)
{
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CStair::Tick(_float fTimeDelta)
{
}

void CStair::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->IsIn_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POS), 100.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);

#ifdef _DEBUG
		if (nullptr != m_pCurNavigationCom)
			m_pRendererCom->Add_Debug(m_pCurNavigationCom);
#endif
	}


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CStair::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	

	_bool	Is_Normal, Is_ORM;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		Is_Normal = Is_ORM = true;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			Is_Normal = false;

		if(true == Is_Normal)
			m_pShaderCom->Begin(3);
		else
			m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CStair::Ready_Components()
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
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Stair"),
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stair"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Stair_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
		return E_FAIL;

	m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CStair::Bind_ShaderResources()
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

CStair* CStair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CStair* pInstance = new CStair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CStair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStair::Clone(void* pArg)
{
	__super::Clone(pArg);

	CStair* pInstance = new CStair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStair::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
