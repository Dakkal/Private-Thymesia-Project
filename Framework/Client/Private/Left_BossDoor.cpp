#include "pch.h"
#include "..\Public\Left_BossDoor.h"
#include "GameInstance.h"
#include "Bounding_AABB.h"

CLeft_BossDoor::CLeft_BossDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLeft_BossDoor::CLeft_BossDoor(const CLeft_BossDoor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLeft_BossDoor::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PORP;
	m_strObjectName = TEXT("LeftBossDoor");

	return S_OK;
}

HRESULT CLeft_BossDoor::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLeft_BossDoor::Tick(_float fTimeDelta)
{
	if (true == g_OpenDoor)
	{
		m_vAngle = _float3::Lerp(m_vAngle, _float3(80.f, 0.f, 0.f), fTimeDelta);
		m_pTransformCom->Fix_Rotation(AXIS::Y, XMConvertToRadians(-m_vAngle.x));

		m_pColliderCom->Set_Active(false);
	}

	/* 계산한뒤에 월드를 업데이트 */
	if(false == g_EditMode)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CLeft_BossDoor::LateTick(_float fTimeDelta)
{
	if (false == g_EditMode)
		m_pColliderCom->LateUpdate();


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef _DEBUG
	if (false == g_EditMode)
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif

	if (true == pGameInstance->IsIn_Frustum_World(m_pTransformCom->Get_State(CTransform::STATE_POS), 5.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_NONBLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLeft_BossDoor::Render()
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

		if (true == Is_Normal)
			m_pShaderCom->Begin(3);
		else
			m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
	

	return S_OK;
}

HRESULT CLeft_BossDoor::Render_LightDepth()
{
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
		return E_FAIL;

	_matrix		ViewMatrix, ProjMatrix;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vLightPos = pGameInstance->Get_ShadowLightDesc(0)->vLightPos;
	_vector vLightAt = pGameInstance->Get_ShadowLightDesc(0)->vLightAt;

	ViewMatrix = XMMatrixLookAtLH(vLightPos, vLightAt, _vector(0.f, 1.f, 0.f, 0.f));
	ProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f);

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLeft_BossDoor::Ready_Components()
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
	//if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Left_BossDoor"),
	//	TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;
#else
#endif // !EDIT
	if (true == g_EditMode)
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Model_Left_BossDoor"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		/* Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;

		/* Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Left_BossDoor"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;


		CBounding_AABB::BOUNDING_AABB_DESC		AABBDesc = {};
		AABBDesc.vExtents = _float3(6.f, 2.f, 0.5f);
		AABBDesc.vCenter = _float3(3.f, AABBDesc.vExtents.y, 0.f);
		AABBDesc.vCollideColor = _vector(1.f, 1.f, 1.f, 1.f);
		AABBDesc.vColor = _vector(0.33f, 0.63f, 0.93f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &AABBDesc)))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CLeft_BossDoor::Bind_ShaderResources()
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

CLeft_BossDoor* CLeft_BossDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CLeft_BossDoor* pInstance = new CLeft_BossDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CLeft_BossDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLeft_BossDoor::Clone(void* pArg)
{
	__super::Clone(pArg);

	CLeft_BossDoor* pInstance = new CLeft_BossDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLeft_BossDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeft_BossDoor::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
