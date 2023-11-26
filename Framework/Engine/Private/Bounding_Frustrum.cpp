#include "Bounding_Frustrum.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_Frustrum::CBounding_Frustrum()
{
}

HRESULT CBounding_Frustrum::Initialize(const BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_FRUSTRUM_DESC* pFrustrumDesc = (BOUNDING_FRUSTRUM_DESC*)pDesc;

	_vector		vRotation;

	vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pFrustrumDesc->vDegree.x), XMConvertToRadians(pFrustrumDesc->vDegree.y), XMConvertToRadians(pFrustrumDesc->vDegree.z));
	vRotation.w = 1;

	m_pFrustrum_Original = new BoundingFrustum(pFrustrumDesc->vCenter, vRotation
		, pFrustrumDesc->fRightSlope, pFrustrumDesc->fLeftSlope, pFrustrumDesc->fTopSlope, pFrustrumDesc->fBottomSlope
		, pFrustrumDesc->fNear, pFrustrumDesc->fFar);
	m_pFrustrum = new BoundingFrustum(*m_pFrustrum_Original);

	m_vCollideColor = pFrustrumDesc->vCollideColor;
	m_vColor = pFrustrumDesc->vColor;

	m_pBoundingOwner = pFrustrumDesc->pOwner;

	return S_OK;
}

void CBounding_Frustrum::Update(_matrix TransformMatrix)
{
	m_pFrustrum_Original->Transform(*m_pFrustrum, TransformMatrix);
}

_bool CBounding_Frustrum::IsCollision(CCollider::TYPE eType, CBounding* pBouding)
{
	_bool IsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		IsColl = m_pFrustrum->Intersects(*(dynamic_cast<CBounding_AABB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_OBB:
		IsColl = m_pFrustrum->Intersects(*(dynamic_cast<CBounding_OBB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_SPHERE:
		IsColl = m_pFrustrum->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_FRUSTUM:
		IsColl = m_pFrustrum->Intersects(*(dynamic_cast<CBounding_Frustrum*>(pBouding))->Get_Bouding());
		break;
	}

	return IsColl;
}

_vector CBounding_Frustrum::Get_ColWorldPos()
{
	return 	_vector(m_pFrustrum->Origin.x, m_pFrustrum->Origin.y, m_pFrustrum->Origin.z, 1.f);
}

#ifdef _DEBUG
HRESULT CBounding_Frustrum::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector		vColor = m_IsColl == true ? m_vCollideColor : m_vColor;

	DX::Draw(pBatch, *m_pFrustrum, vColor);

	return S_OK;
}
#endif // DEBUG



CBounding_Frustrum* CBounding_Frustrum::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_Frustrum* pInstance = new CBounding_Frustrum();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Frustrum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Frustrum::Free()
{
	__super::Free();

	Safe_Delete(m_pFrustrum);
	Safe_Delete(m_pFrustrum_Original);
}
