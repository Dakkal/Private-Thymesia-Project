#include "Bounding_Sphere.h"

#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "DebugDraw.h"

CBounding_Sphere::CBounding_Sphere()
{
}

HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_SPHERE_DESC* pSphereDesc = (BOUNDING_SPHERE_DESC*)pDesc;

	m_pSphere_Original = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pSphere = new BoundingSphere(*m_pSphere_Original);

	m_vCollideColor = pSphereDesc->vCollideColor;
	m_vColor = pSphereDesc->vColor;

	m_pBoundingOwner = pSphereDesc->pOwner;

	return S_OK;
}

void CBounding_Sphere::Update(_matrix TransformMatrix)
{
	m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
}

_bool CBounding_Sphere::IsCollision(CCollider::TYPE eType, CBounding* pBouding)
{
	_bool IsColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		IsColl = m_pSphere->Intersects(*(dynamic_cast<CBounding_AABB*>(pBouding)->Get_Bouding()));
		break;
	case CCollider::TYPE_OBB:
		IsColl = m_pSphere->Intersects(*(dynamic_cast<CBounding_OBB*>(pBouding)->Get_Bouding()));
		break;
	case CCollider::TYPE_SPHERE:
		IsColl = m_pSphere->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBouding)->Get_Bouding()));
		break;
	}

	return IsColl;
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	_vector		vColor = m_IsColl == true ? m_vCollideColor : m_vColor;

	DX::Draw(pBatch, *m_pSphere, vColor);

	return S_OK;
}
#endif

CBounding_Sphere* CBounding_Sphere::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere);
	Safe_Delete(m_pSphere_Original);
}
