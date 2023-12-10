#include "pch.h"
#include "..\Public\PartUI.h"

#include "GameInstance.h"


#include "StateMachine.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

CPartUI::CPartUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CPartUI::CPartUI(const CPartUI& rhs)
	: CPartObject(rhs)
{

}

HRESULT CPartUI::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PART;
	m_strObjectName = TEXT("Npc_Kid_UI");

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 834 * 0.8f;
	m_fSizeY = 735 * 0.8f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform_Back->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform_Back->Set_State(CTransform::STATE_POS,
		_vector(m_fX - g_iWinSizeX * 0.5f , -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	m_fSizeX = 512 * 0.3f;
	m_fSizeY = 512 * 0.3f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform_Key->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform_Key->Set_State(CTransform::STATE_POS,
		_vector(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f + 120.f, 0.f, 1.f));

	m_fSizeX = 1820 * 0.25f;
	m_fSizeY = 123;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_pTransform_Line->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransform_Line->Set_State(CTransform::STATE_POS,
		_vector(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f + 10.f, 0.f, 1.f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	return S_OK;
}

HRESULT CPartUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartUI::Tick(_float fTimeDelta)
{
	
}

void CPartUI::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::RG_UI, this);
}

HRESULT CPartUI::Render()
{
	if (FAILED(Bind_ShaderResources_Back()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	if (FAILED(Bind_ShaderResources_Key()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	if (FAILED(Bind_ShaderResources_Line()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	wstring strLogo = TEXT("ROSVHFRMX 열쇠");
	wstring strTex1 = TEXT("꼬마아이가 주는 보스방을 열어주는 열쇠");
	wstring strTex2 = TEXT("원래라면 말이 많았어야 하는 아이였지만");
	wstring strTex3 = TEXT("창조주의 부족함으로 벙어리가 되고 말았다.");

	pGameInstance->Render_Font(TEXT("Font_Default"), strLogo, _float2(g_iWinSizeX * 0.5f - 100, g_iWinSizeY * 0.5f + 50.f), _vector(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.8f);
	pGameInstance->Render_Font(TEXT("Font_Default"), strTex1, _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f + 100.f), _vector(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f);
	pGameInstance->Render_Font(TEXT("Font_Default"), strTex2, _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f + 140.f), _vector(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f);
	pGameInstance->Render_Font(TEXT("Font_Default"), strTex3, _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f + 180.f), _vector(1.f, 1.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 0.5f);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPartUI::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com.Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex")
		, TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com.VIBuffer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect")
		, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform1"), (CComponent**)&m_pTransform_Key)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform2"), (CComponent**)&m_pTransform_Back)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform3"), (CComponent**)&m_pTransform_Line)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Key"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTexture_Key))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Back"),
		TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTexture_Back))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Line"),
		TEXT("Com_Texture3"), reinterpret_cast<CComponent**>(&m_pTexture_Line))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartUI::Bind_ShaderResources_Key()
{
	if (FAILED(m_pTransform_Key->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture_Key->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartUI::Bind_ShaderResources_Back()
{
	if (FAILED(m_pTransform_Back->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture_Back->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartUI::Bind_ShaderResources_Line()
{
	if (FAILED(m_pTransform_Line->Bind_ShaderResources(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTexture_Line->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CPartUI* CPartUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CPartUI* pInstance = new CPartUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CPartUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPartUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransform_Back);
	Safe_Release(m_pTransform_Key);
	Safe_Release(m_pTransform_Line);
	Safe_Release(m_pTexture_Back);
	Safe_Release(m_pTexture_Key);
	Safe_Release(m_pTexture_Line);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
