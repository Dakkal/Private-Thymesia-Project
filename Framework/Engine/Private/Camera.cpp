#include "..\Public\Camera.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
	, m_ViewMatrix(rhs.m_ViewMatrix)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::LateTick(_float fTimeDelta)
{
}

void CCamera::Free()
{
	__super::Free();
}
