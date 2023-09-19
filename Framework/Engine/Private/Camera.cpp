#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)pArg;

	m_vEye = pCameraDesc->vEye;
	m_vAt = pCameraDesc->vAt;
	m_fFovy = pCameraDesc->fFovy;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;

	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (FAILED(m_pTransform->Initialize(pArg)))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POS, m_vEye);
	m_pTransform->LookAt(m_vAt);

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::LateTick(_float fTimeDelta)
{
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pPipeLine);
}
