#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

const _float3& CTransform::Get_Scale() const
{
	return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix		StateMatrix;

	StateMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	StateMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, StateMatrix);
}

void CTransform::Set_Scale(const _float3& vScale)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
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

HRESULT CTransform::Bind_ShaderReSources(CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
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
	_float3		vScaled = Get_Scale();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;
	
	_float4 vQuaternionData;
	XMStoreFloat4(&vQuaternionData, XMVectorZero());

	if(1 == XMVectorGetX(vAxis))
		vQuaternionData = { sinf(fRadian * 0.5f), 0.f, 0.f, cosf(XMConvertToRadians(fRadian * 0.5f)) };
	else if(1 == XMVectorGetY(vAxis))
		vQuaternionData = { 0.f, sinf(fRadian * 0.5f), 0.f, cosf(XMConvertToRadians(fRadian * 0.5f)) };
	else if (1 == XMVectorGetZ(vAxis))
		vQuaternionData = { 0.f, 0.f, sinf(fRadian * 0.5f), cosf(XMConvertToRadians(fRadian * 0.5f)) };


	_matrix RotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuaternionData));


	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_float4 vQuaternionData;
	XMStoreFloat4(&vQuaternionData, XMVectorZero());

	if (1 == XMVectorGetX(vAxis))
		vQuaternionData = { sinf((m_fRotRadianPerSec * fTimeDelta) * 0.5f), 0.f, 0.f, cosf(XMConvertToRadians((m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };
	else if (1 == XMVectorGetY(vAxis))
		vQuaternionData = { 0.f, sinf((m_fRotRadianPerSec * fTimeDelta) * 0.5f), 0.f, cosf(XMConvertToRadians((m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };
	else if (1 == XMVectorGetZ(vAxis))
		vQuaternionData = { 0.f, 0.f, sinf((m_fRotRadianPerSec * fTimeDelta) * 0.5f), cosf(XMConvertToRadians((m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };


	_matrix RotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuaternionData));


	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Turn_Invert(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_float4 vQuaternionData;
	XMStoreFloat4(&vQuaternionData, XMVectorZero());

	if (1 == XMVectorGetX(vAxis))
		vQuaternionData = { sinf((-m_fRotRadianPerSec * fTimeDelta) * 0.5f), 0.f, 0.f, cosf(XMConvertToRadians((-m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };
	else if (1 == XMVectorGetY(vAxis))
		vQuaternionData = { 0.f, sinf((-m_fRotRadianPerSec * fTimeDelta) * 0.5f), 0.f, cosf(XMConvertToRadians((-m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };
	else if (1 == XMVectorGetZ(vAxis))
		vQuaternionData = { 0.f, 0.f, sinf((-m_fRotRadianPerSec * fTimeDelta) * 0.5f), cosf(XMConvertToRadians((-m_fRotRadianPerSec * fTimeDelta) * 0.5f)) };


	_matrix RotationMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&vQuaternionData));

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::LookAt(_fvector vPoint)
{
	_float3		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POS);
	_vector		vLook = XMVector3Normalize(vPoint - vPosition) * vScaled.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScaled.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Chase(_fvector vPoint, _float fTimeDelta, _float fDis)
{
	_vector		vPosition = Get_State(STATE_POS);

	_vector		vDir = vPoint - vPosition;

	if (XMVectorGetX(XMVector3Length(vDir)) >= fDis)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
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
