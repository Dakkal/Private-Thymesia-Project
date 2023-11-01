#include "Bounding_AABB.h"

#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB()
{
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_AABB_DESC* pAABBDesc = (BOUNDING_AABB_DESC*)pDesc;

	m_pAABB_Original = new BoundingBox(pAABBDesc->vCenter, pAABBDesc->vExtents);
	m_pAABB = new BoundingBox(*m_pAABB_Original);

	m_vCollideColor = pAABBDesc->vCollideColor;
	m_vColor = pAABBDesc->vColor;

	m_pBoundingOwner = pAABBDesc->pOwner;

	return S_OK;
}

void CBounding_AABB::Update(_matrix TransformMatrix)
{
	XMMATRIX	  Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[2]));

	m_pAABB_Original->Transform(*m_pAABB, Matrix);
}

_bool CBounding_AABB::IsCollision(CCollider::TYPE eType, CBounding* pBouding)
{
	_bool IsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		IsColl = m_pAABB->Intersects(*(dynamic_cast<CBounding_AABB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_OBB:
		IsColl = m_pAABB->Intersects(*(dynamic_cast<CBounding_OBB*>(pBouding))->Get_Bouding());
		break;
	case CCollider::TYPE_SPHERE:
		IsColl = m_pAABB->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBouding))->Get_Bouding());
		break;
	}

	return IsColl;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector		vColor = m_IsColl == true ? m_vCollideColor : m_vColor;
	
	DX::Draw(pBatch, *m_pAABB, vColor);

	return S_OK;
}
#endif // DEBUG



CBounding_AABB* CBounding_AABB::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pAABB_Original);
}
