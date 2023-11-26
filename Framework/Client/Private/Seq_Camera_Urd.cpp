#include "pch.h"
#include "Seq_Camera_Urd.h"

#include "GameInstance.h"
#include "BinBone.h"
#include "Bounding_AABB.h"
#include "Collider.h"

CSeq_Camera_Urd::CSeq_Camera_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{

}

CSeq_Camera_Urd::CSeq_Camera_Urd(const CSeq_Camera_Urd& rhs)
	: CPartObject(rhs)
{

}

HRESULT CSeq_Camera_Urd::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	m_eObjType = OBJECT_TYPE::PART;
	m_strObjectName = TEXT("Boss_Urd_Camera");

	return S_OK;
}

HRESULT CSeq_Camera_Urd::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	/* 부모 소켓행렬을 기준으로 자식의 상태를 제어한다.  */

	return S_OK;
}

void CSeq_Camera_Urd::Tick(_float fTimeDelta)
{
	XMMATRIX	WorldMatrix = m_pSocketBone->Get_CombinedTransform() * m_SocketPivotMatrix;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(WorldMatrix);

	memcpy(&m_vEye, m_WorldMatrix.m[3], sizeof(_vector));

	WorldMatrix = m_pSocketBoneforPivot->Get_CombinedTransform() * m_SocketPivotMatrix;

	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);

	Compute_RenderMatrix(WorldMatrix);

	memcpy(&m_vAt, m_WorldMatrix.m[3], sizeof(_vector));


	m_pTransformCom->Set_State(CTransform::STATE_POS, m_vEye);
	m_pTransformCom->LookAt(m_vAt);
}

void CSeq_Camera_Urd::LateTick(_float fTimeDelta)
{
	if (true == g_BossSeq)
	{
		CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

		pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
		pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(85.f), g_iWinSizeX / (_float)g_iWinSizeY, 0.1f, 1000.f));

		RELEASE_INSTANCE(CPipeLine);
	}
}

HRESULT CSeq_Camera_Urd::Render()
{

	return S_OK;
}

HRESULT CSeq_Camera_Urd::Ready_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSeq_Camera_Urd::Bind_ShaderResources()
{


	return S_OK;
}

CSeq_Camera_Urd* CSeq_Camera_Urd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag)
{
	CSeq_Camera_Urd* pInstance = new CSeq_Camera_Urd(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strProtoTag)))
	{
		MSG_BOX("Failed to Created : CSeq_Camera_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSeq_Camera_Urd::Clone(void* pArg)
{
	__super::Clone(pArg);

	CSeq_Camera_Urd* pInstance = new CSeq_Camera_Urd(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSeq_Camera_Urd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSeq_Camera_Urd::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
}
