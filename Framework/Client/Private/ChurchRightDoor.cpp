#include "pch.h"
#include "..\Public\ChurchRightDoor.h"
#include "GameInstance.h"

CChurchRightDoor::CChurchRightDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CChurchRightDoor::CChurchRightDoor(const CChurchRightDoor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CChurchRightDoor::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("ChurchRightDoor");

	return S_OK;
}

HRESULT CChurchRightDoor::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CChurchRightDoor::Tick(_float fTimeDelta)
{

}

void CChurchRightDoor::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (true == pGameInstance->IsIn_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POS), 3.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CChurchRightDoor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Bind_MaterialTexture(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}



	return S_OK;
}

HRESULT CChurchRightDoor::Ready_Components()
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
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_ChurchRightDoor"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;
#else
#endif // !EDIT
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChurchRightDoor"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChurchRightDoor::Bind_ShaderResources()
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

CChurchRightDoor* CChurchRightDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CChurchRightDoor* pInstance = new CChurchRightDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CChurchRightDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChurchRightDoor::Clone(void* pArg)
{
	__super::Clone(pArg);

	CChurchRightDoor* pInstance = new CChurchRightDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CChurchRightDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChurchRightDoor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
