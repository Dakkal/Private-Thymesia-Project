#include "..\Public\Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix		StateMatrix;

	StateMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	StateMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, StateMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC tTransformDesc;

		memmove(&tTransformDesc, pArg, sizeof(tTransformDesc));

		m_fSpeedPerSec = tTransformDesc.fSpeedPerSec;
		m_fRotRadianPerSec = tTransformDesc.fRotRadianPerSec;
	}

	return S_OK;
}

void CTransform::Go_Forward(_float fTimeDelta)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector		vUp = Get_State(STATE_UP);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector		vUp = Get_State(STATE_UP);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector     vPosition = Get_State(STATE_POS);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Fix_Rotation(_fvector vAxis, _float fRadian)
{
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
