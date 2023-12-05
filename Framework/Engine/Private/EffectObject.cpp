#include "..\Public\EffectObject.h"
#include "GameInstance.h"
#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Rect_Instance.h"

CEffectObject::CEffectObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectObject::Initialize_Prototype(const wstring& strProtoTag)
{
	__super::Initialize_Prototype(strProtoTag);

	return S_OK;
}

HRESULT CEffectObject::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	EFFECT_DESC* pPartDesc = (EFFECT_DESC*)pArg;

	m_vTargetPos = pPartDesc->vTargetPos;
	m_vRotFactorX = pPartDesc->vRotFactorX;
	m_vRotFactorY = pPartDesc->vRotFactorY;
	m_vRotFactorZ = pPartDesc->vRotFactorZ;
	m_eEffectType = pPartDesc->eEffectType;
	m_pTargetTransform = pPartDesc->pTargetTransform;

	return S_OK;
}

void CEffectObject::PriorityTick(_float fTimeDelta)
{
}

void CEffectObject::Tick(_float fTimeDelta)
{
}

void CEffectObject::LateTick(_float fTimeDelta)
{
}

HRESULT CEffectObject::Render()
{
	return S_OK;
}

HRESULT CEffectObject::Use_Effect()
{
	return S_OK;
}

const _bool& CEffectObject::Is_Update()
{
	if (TYPE::POINT == m_eEffectType)
	{
		return m_pVIBufferCom->Is_Update();
	}
	else
	{
		return m_pVIRectBufferCom->Is_Update();
	}

	
}

HRESULT CEffectObject::Ready_Components()
{
	return S_OK;
}

HRESULT CEffectObject::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrices("g_RotMatricesX", m_matRotMatrices[0], m_iMaxNumInstance)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_RotMatricesY", m_matRotMatrices[1], m_iMaxNumInstance)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_RotMatricesZ", m_matRotMatrices[2], m_iMaxNumInstance)))
		return E_FAIL;

	return S_OK;
}

void CEffectObject::Create_RandomRotMatrix(_float2 vAxisX, _float2 vAxisY, _float2 vAxisZ)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float fRotX = pGameInstance->Random_Float(vAxisX.x, vAxisX.y);

		_matrix		RotationMatrix = XMMatrixRotationAxis(AXIS::X, XMConvertToRadians(fRotX));
		_vector		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
		_matrix		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

		m_matRotMatrices[0][i] = QuaternionMatrix;

		_float fRotY = pGameInstance->Random_Float(vAxisY.x, vAxisY.y);

		RotationMatrix = XMMatrixRotationAxis(AXIS::Y, XMConvertToRadians(fRotY));
		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

		m_matRotMatrices[1][i] = QuaternionMatrix;

		_float fRotZ = pGameInstance->Random_Float(vAxisZ.x, vAxisZ.y);

		RotationMatrix = XMMatrixRotationAxis(AXIS::Z, XMConvertToRadians(fRotZ));
		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

		m_matRotMatrices[2][i] = QuaternionMatrix;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CEffectObject::Free()
{
	__super::Free();
}
