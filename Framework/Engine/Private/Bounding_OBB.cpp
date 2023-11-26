#include "Bounding_OBB.h"

#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Frustrum.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
{
}

HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_OBB_DESC* pOBBDesc = (BOUNDING_OBB_DESC*)pDesc;

	_vector		vRotation;

	vRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pOBBDesc->vDegree.x), XMConvertToRadians(pOBBDesc->vDegree.y), XMConvertToRadians(pOBBDesc->vDegree.z));

	m_pOBB_Original = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, vRotation);
	m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);

	m_vCollideColor = pOBBDesc->vCollideColor;
	m_vColor = pOBBDesc->vColor;

	m_pBoundingOwner = pOBBDesc->pOwner;

	return S_OK;
}

void CBounding_OBB::Update(_matrix TransformMatrix)
{
	m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
}

_bool CBounding_OBB::IsCollision(CCollider::TYPE eType, CBounding* pBouding)
{
	_bool IsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		IsColl = m_pOBB->Intersects(*(dynamic_cast<CBounding_AABB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_OBB:
		IsColl = m_pOBB->Intersects(*(dynamic_cast<CBounding_OBB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_SPHERE:
		IsColl = m_pOBB->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_FRUSTUM:
		IsColl = m_pOBB->Intersects(*(dynamic_cast<CBounding_Frustrum*>(pBouding))->Get_Bouding());
		break;
	}

	return IsColl;
}

_vector CBounding_OBB::Get_ColWorldPos()
{
	return 	_vector(m_pOBB->Center.x, m_pOBB->Center.y, m_pOBB->Center.z, 1.f);
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector		vColor = m_IsColl == true ? m_vCollideColor : m_vColor;

	DX::Draw(pBatch, *m_pOBB, vColor);

	return S_OK;
}
#endif // _DEBUG


CBounding_OBB* CBounding_OBB::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB);
	Safe_Delete(m_pOBB_Original);
}
