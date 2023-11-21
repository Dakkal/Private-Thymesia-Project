#include "pch.h"
#include "..\Public\Sub_Building.h"
#include "GameInstance.h"
#include "Navigation.h"

CSub_Building::CSub_Building(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CSub_Building::CSub_Building(const CSub_Building& rhs)
	: CLandObject(rhs)
{
}

HRESULT CSub_Building::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("SubBuilding");

	return S_OK;
}

HRESULT CSub_Building::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSub_Building::PriorityTick(_float fTimeDelta)
{
	if (nullptr != m_pCurNavigationCom)
		m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CSub_Building::Tick(_float fTimeDelta)
{
}

void CSub_Building::LateTick(_float fTimeDelta)
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

HRESULT CSub_Building::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	Is_Normal, Is_ORM;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		Is_Normal = Is_ORM = true;

		if("Mat_Glass_Dirty_02" == m_pModelCom->Get_MaterialName(i))
		{
			if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				Is_Normal = false;

			m_vColor = { 0.08f, 0.086667f, 0.1f, 1.f };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
				return E_FAIL;

			if(true == Is_Normal)
				m_pShaderCom->Begin(3);
			else
				m_pShaderCom->Begin(2);

			m_pModelCom->Render(i);

			m_vColor = { 0.f, 0.f, 0.f, 0.f };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
				Is_Normal = false;

			if (true == Is_Normal)
				m_pShaderCom->Begin(3);
			else
				m_pShaderCom->Begin(2);

			m_pModelCom->Render(i);
		}
	}

	return S_OK;
}

HRESULT CSub_Building::Ready_Components()
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
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Sub_Building"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
#else
	///* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sub_Building"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;

	///* Com_Navigation */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Sub_Building_Navigation"),
	//	TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
	//	return E_FAIL;
#endif // !NDEBUG
	///* Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sub_Building"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;

	///* Com_Navigation */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Sub_Building_Navigation"),
	//	TEXT("Com_Navigation"), (CComponent**)&m_pCurNavigationCom)))
	//	return E_FAIL;

	//m_pCurNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSub_Building::Bind_ShaderResources()
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

CSub_Building* CSub_Building::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CSub_Building* pInstance = new CSub_Building(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CSub_Building");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSub_Building::Clone(void* pArg)
{
	__super::Clone(pArg);

	CSub_Building* pInstance = new CSub_Building(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSub_Building");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSub_Building::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
