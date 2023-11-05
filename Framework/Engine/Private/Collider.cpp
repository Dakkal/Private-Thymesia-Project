#include "..\Public\Collider.h"
#include "GameObject.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Frustrum.h"
#include "PipeLine.h"

_uint CCollider::g_iNextID = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(CGameObject* pOwner, const CCollider& rhs)
	: CComponent(pOwner, rhs)
	, m_iColID(g_iNextID++)
	, m_eColliderType(rhs.m_eColliderType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t		iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	CBounding::BOUNDING_DESC* pBoundingDesc = (CBounding::BOUNDING_DESC*)pArg;

	pBoundingDesc->pOwner = this;

	switch (m_eColliderType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pBoundingDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pBoundingDesc);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(pBoundingDesc);
		break;
	case TYPE_FRUSTUM:
		m_pBounding = CBounding_Frustrum::Create(pBoundingDesc);
		break;
	}

	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Update(_matrix TransformMatrix)
{
	if (false == m_bActive)
		return;

	m_pBounding->Update(TransformMatrix);
}

void CCollider::LateUpdate()
{
	if (false == m_bActive)
		return;

	if (true == Is_Colli())
		m_pBounding->Set_Coll(true);
	else
		m_pBounding->Set_Coll(false);
}

_bool CCollider::IsCollision(CCollider* pTargetCollider)
{
	if (false == m_bActive)
		return false;

	return m_pBounding->IsCollision(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding);
}

void CCollider::Set_Colli(_bool _IsColli)
{
	return m_pBounding->Set_Coll(_IsColli);
}

void CCollider::OnCollision_Enter(CGameObject* _pColObj, _float fTimedelta)
{
	++m_iNumbCol;

	if (false == m_bActive)
		return;

	m_pOwner->OnCollision_Enter(_pColObj, fTimedelta);
}

void CCollider::OnCollision_Stay(CGameObject* _pColObj, _float fTimedelta)
{
	if (false == m_bActive)
		return;

	m_pOwner->OnCollision_Stay(_pColObj, fTimedelta);
}

void CCollider::OnCollision_Exit(CGameObject* _pColObj, _float fTimedelta)
{
	--m_iNumbCol;

	if (false == m_bActive)
		return;

	m_pOwner->OnCollision_Exit(_pColObj, fTimedelta);
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (false == m_bActive)
		return S_OK;

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(class CGameObject* pOwner, void* pArg)
{
	CCollider* pInstance = new CCollider(pOwner , *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (false == m_IsCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif

}
