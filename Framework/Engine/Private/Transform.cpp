#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "GameObject.h"
#include "LandObject.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(CGameObject* pOwner, const CTransform& rhs)
	: CComponent(pOwner, rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_State(STATE eState)
{
	_vector vRow;
	memmove(&vRow, m_WorldMatrix.m[eState], sizeof _vector);

	return vRow;
}

_vector CTransform::Get_Scale()
{
	return _vector(Get_State(STATE_RIGHT).Length(),
		Get_State(STATE_UP).Length(),
		Get_State(STATE_LOOK).Length(), 0);
}

void CTransform::Set_State(STATE eState, _vector vState)
{
	memmove(m_WorldMatrix.m[eState], &vState, sizeof _vector);
}

void CTransform::Set_Look(_vector vChangeLook)
{
	_vector		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POS);
	_vector		vLook = XMVector3Normalize(vChangeLook) * vScaled.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(_vector(0.f, 1.f, 0.f, 0.f), vLook)) * vScaled.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Set_Scale(const _float3& vScale)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp  = Get_State(STATE_UP);
	_vector		vLook  = Get_State(STATE_LOOK);

	vRight.Normalize();
	vUp.Normalize();
	vLook.Normalize();

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::Set_WorldMatrix(_matrix matWorld)
{
	m_WorldMatrix = matWorld;
}

HRESULT CTransform::Initialize_Prototype()
{
	m_WorldMatrix = XMMatrixIdentity();

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC tTransformDesc;

		memmove(&tTransformDesc, pArg, sizeof(tTransformDesc));

		m_TrasformDesc.fSpeedPerSec = tTransformDesc.fSpeedPerSec;
		m_TrasformDesc.fRotRadianPerSec = tTransformDesc.fRotRadianPerSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResources(CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Go_Dir(_vector vDir, _float fTimeDelta, CNavigation* pNavi)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POS);

	vDir.Normalize();
	vPosition += vDir * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPosition);

		if (0 == iMove)
		{
			Set_State(STATE_POS, vPosition);
		}
		else if (-2 == iMove)
		{
			if (true == dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPosition))
			{
				Set_State(STATE_POS, vPosition);
			}
		}
	}
	else
		Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Forward(_float fTimeDelta, CNavigation* pNavi)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POS);

	vLook.Normalize();
	vPosition += vLook * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPosition);

		if (0 == iMove)
		{
			Set_State(STATE_POS, vPosition);
		}
		else if (-2 == iMove)
		{
			if (true == dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPosition))
			{
				Set_State(STATE_POS, vPosition);
			}
		}
		else if (-1 == iMove)
		{
			_vector vSlider = pNavi->Get_Cell_SliderVec(vLook);

			if (-1 != vSlider.w)
				Go_Dir(vSlider, fTimeDelta, pNavi);
		}

	}
	else
		Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavi)
{
	_vector		vLook = Get_State(STATE_LOOK);
	_vector		vPosition = Get_State(STATE_POS);

	vLook.Normalize();
	vPosition -= vLook * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPosition);

		if (0 == iMove)
		{
			Set_State(STATE_POS, vPosition);
		}
		else if (-2 == iMove)
		{
			if (true == dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPosition))
			{
				Set_State(STATE_POS, vPosition);
			}
		}
		

	}
	else
		Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector		vUp = Get_State(STATE_UP);
	_vector		vPosition = Get_State(STATE_POS);

	vUp.Normalize();
	vPosition += vUp * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector		vUp = Get_State(STATE_UP);
	_vector		vPosition = Get_State(STATE_POS);

	vUp.Normalize();
	vPosition -= vUp * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavi)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vPosition = Get_State(STATE_POS);

	vRight.Normalize();
	vPosition -= vRight * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPosition);

		if (0 == iMove)
		{
			Set_State(STATE_POS, vPosition);
		}
		else if (-2 == iMove)
		{
			if (true == dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPosition))
			{
				Set_State(STATE_POS, vPosition);
			}
		}

	}
	else
		Set_State(STATE_POS, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavi)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vPosition = Get_State(STATE_POS);

	vRight.Normalize();
	vPosition += vRight * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPosition);

		if (0 == iMove)
		{
			Set_State(STATE_POS, vPosition);
		}
		else if (-2 == iMove)
		{
			if (true == dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPosition))
			{
				Set_State(STATE_POS, vPosition);
			}
		}

	}
	else
		Set_State(STATE_POS, vPosition);
}

void CTransform::Fix_Rotation(_vector vAxis, _float fRadian)
{
	_vector		vScaled = Get_Scale();

	_vector		vRight = _vector(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = _vector(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = _vector(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_vector		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
	_matrix		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

	vRight = XMVector4Transform(vRight, QuaternionMatrix);
	vUp = XMVector4Transform(vUp, QuaternionMatrix);
	vLook = XMVector4Transform(vLook, QuaternionMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Rotation(_vector vAxis, _float fRadian)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_vector		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
	_matrix		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

	vRight = XMVector4Transform(vRight, QuaternionMatrix);
	vUp = XMVector4Transform(vUp, QuaternionMatrix);
	vLook = XMVector4Transform(vLook, QuaternionMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Turn(_vector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TrasformDesc.fRotRadianPerSec * fTimeDelta);
	_vector		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
	_matrix		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

	vRight = XMVector4Transform(vRight, QuaternionMatrix);
	vUp = XMVector4Transform(vUp, QuaternionMatrix);
	vLook = XMVector4Transform(vLook, QuaternionMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::LookAt(_vector vPoint)
{
	_vector		vScaled = Get_Scale();

	_vector		vPosition = Get_State(STATE_POS);
	_vector		vLook = XMVector3Normalize(vPoint - vPosition) * vScaled.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(_vector(0.f, 1.f, 0.f, 0.f), vLook)) * vScaled.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScaled.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Chase(_vector vPoint, _float fTimeDelta, _float fDis)
{
	_vector		vPosition = Get_State(STATE_POS);

	_vector		vDir = vPoint - vPosition;

	if (vDir.Length() > fDis)
		vPosition += XMVector3Normalize(vDir) * m_TrasformDesc.fSpeedPerSec * fTimeDelta;

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

CComponent* CTransform::Clone(CGameObject* pOwner, void* pArg)
{
	CTransform* pInstance = new CTransform(pOwner, *this);

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
