#include "..\Public\Calculator.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "Navigation.h"
#include "LandObject.h"

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{
}

_vector CCalculator::Picking_Terrain(RECT rc, POINT pt, CTransform* pTransform, CVIBuffer* pBuffer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float3		vMousePos(pt.x, pt.y, 1.f);
	SimpleMath::Viewport viewport(rc);

    _float3 WordlMousePos = viewport.Unproject(vMousePos,
        pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
        pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW),
        pTransform->Get_WorldMatrix());

    SimpleMath::Ray ray;
    ray.position = _float3(pGameInstance->Get_CamPosition_Vector());
    ray.direction = WordlMousePos - ray.position;
    ray.direction.Normalize();

    ray.position = XMVector3TransformCoord(ray.position, pTransform->Get_WorldMatrix_Inverse());
    ray.direction = XMVector3TransformNormal(ray.direction, pTransform->Get_WorldMatrix_Inverse());
    ray.direction.Normalize();

	vector<_float3> pBufferPos = pBuffer->Get_Buffer_Pos();
	CVIBuffer_Terrain::TERRAIN_DESC tTerrainDesc = dynamic_cast<CVIBuffer_Terrain*>(pBuffer)->Get_TerrainDesc();

    _ulong		dwVtxIdx[3]{};

    _float	fDist = 0.f;

    for (_ulong i = 0; i < tTerrainDesc.iNumVerticesZ - 1; ++i)
    {
        for (_ulong j = 0; j < tTerrainDesc.iNumVerticesX - 1; ++j)
        {
            _ulong	dwIndex = i * tTerrainDesc.iNumVerticesX + j;

            // 오른쪽 위
            dwVtxIdx[0] = dwIndex + tTerrainDesc.iNumVerticesX;
            dwVtxIdx[1] = dwIndex + tTerrainDesc.iNumVerticesX + 1;
            dwVtxIdx[2] = dwIndex + 1;

            if (true == ray.Intersects(pBufferPos[dwVtxIdx[0]], pBufferPos[dwVtxIdx[1]], pBufferPos[dwVtxIdx[2]], fDist))
            {
                RELEASE_INSTANCE(CGameInstance);

                _vector vPick = _vector(ray.position.x + (ray.direction.x * fDist),
                    ray.position.y + (ray.direction.y * fDist),
                    ray.position.z + (ray.direction.z * fDist), 1.f);

                vPick = XMVector3TransformCoord(vPick, pTransform->Get_WorldMatrix());

                return vPick;
            }

            // 왼쪽 아래
            dwVtxIdx[0] = dwIndex + tTerrainDesc.iNumVerticesX;
            dwVtxIdx[1] = dwIndex + 1;
            dwVtxIdx[2] = dwIndex;

            if (true == ray.Intersects(pBufferPos[dwVtxIdx[0]], pBufferPos[dwVtxIdx[1]], pBufferPos[dwVtxIdx[2]], fDist))
            {
                RELEASE_INSTANCE(CGameInstance);

                _vector vPick = _vector(ray.position.x + (ray.direction.x * fDist),
                    ray.position.y + (ray.direction.y * fDist),
                    ray.position.z + (ray.direction.z * fDist), 1.f);

                vPick = XMVector3TransformCoord(vPick, pTransform->Get_WorldMatrix());

                return vPick;
            }
        }
    }
	

    RELEASE_INSTANCE(CGameInstance);

    return _vector(0.f, 0.f, 0.f, 1.f);
}

_vector CCalculator::Picking_Object(RECT rc, POINT pt, CTransform* pTransform, CVIBuffer* pBuffer)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    _float3		vMousePos(pt.x, pt.y, 1.f);
    SimpleMath::Viewport viewport(rc);

    _float3 WordlMousePos = viewport.Unproject(vMousePos,
        pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
        pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW),
        pTransform->Get_WorldMatrix());

    SimpleMath::Ray ray;
    ray.position = _float3(pGameInstance->Get_CamPosition_Vector());
    ray.direction = WordlMousePos - ray.position;
    ray.direction.Normalize();

    ray.position = XMVector3TransformCoord(ray.position, pTransform->Get_WorldMatrix_Inverse());
    ray.direction = XMVector3TransformNormal(ray.direction, pTransform->Get_WorldMatrix_Inverse());
    ray.direction.Normalize();

    vector<_float3> pBufferPos = pBuffer->Get_Buffer_Pos();
    vector<_ulong>Indicies = pBuffer->Get_Indicies();

    _ulong	dwVtxIdx[3]{};

    _float	fDist = 0.f;

    for (size_t iIndex = 0; iIndex < Indicies.size(); )
    {
        dwVtxIdx[0] = Indicies[iIndex++];
        dwVtxIdx[1] = Indicies[iIndex++];
        dwVtxIdx[2] = Indicies[iIndex++];

        if (true == ray.Intersects(pBufferPos[dwVtxIdx[0]], pBufferPos[dwVtxIdx[1]], pBufferPos[dwVtxIdx[2]], fDist))
        {
            RELEASE_INSTANCE(CGameInstance);

            _vector vPick = _vector(ray.position.x + (ray.direction.x * fDist),
                ray.position.y + (ray.direction.y * fDist),
                ray.position.z + (ray.direction.z * fDist), 1.f);

            vPick = XMVector3TransformCoord(vPick, pTransform->Get_WorldMatrix());

            return vPick;
        }
    }

    RELEASE_INSTANCE(CGameInstance);

    return _vector(-1.f, -1.f, -1.f, -1.f);
}

HRESULT CCalculator::Detrude_Collide(CGameObject* pColObj, CCollider* pObjCol, CTransform* pObjTransform)
{
    if (nullptr == pColObj || nullptr == pObjCol || nullptr == pObjTransform)
        return E_FAIL;

    CCollider* pCollider = dynamic_cast<CCollider*>(pColObj->Get_Component(TEXT("Com_Collider")));
    CTransform* pTargetTransform = dynamic_cast<CTransform*>(pColObj->Get_Component(TEXT("Com_Transform")));

    _float3 vTargetCenter = dynamic_cast<CBounding_AABB*>(pCollider->Get_ParentBouning())->Get_Bouding()->Center;
    _float3 vPlayerCenter = dynamic_cast<CBounding_AABB*>(pObjCol->Get_ParentBouning())->Get_Bouding()->Center;
    _float3 vFinalCenter = vPlayerCenter - vTargetCenter;

    _float3 vTargetExtents = dynamic_cast<CBounding_AABB*>(pCollider->Get_ParentBouning())->Get_Bouding()->Extents;
    _float3 vPlayerExtents = dynamic_cast<CBounding_AABB*>(pObjCol->Get_ParentBouning())->Get_Bouding()->Extents;
    _float3 vFinalExtents = 0.5 * _float3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

    if (vFinalExtents.x >= vFinalExtents.z)
    {
        // 충돌이 X 축에서 발생.
        if (vPlayerCenter.x > vTargetCenter.x)
        {
            _float vFinalExtents = fabs((vPlayerExtents.x + vTargetExtents.x)) - fabs(vFinalCenter.x);

            Vec4 vPos = pObjTransform->Get_State(CTransform::STATE_POS);
            vPos.x += vFinalExtents;

            pObjTransform->Set_State(CTransform::STATE::STATE_POS, vPos);
        }
        else
        {
            _float vFinalExtents = fabs((vPlayerExtents.x + vTargetExtents.x)) - fabs(vFinalCenter.x);

            Vec4 vPos = pObjTransform->Get_State(CTransform::STATE::STATE_POS);
            vPos.x -= vFinalExtents;

            pObjTransform->Set_State(CTransform::STATE::STATE_POS, vPos);
        }
    }
    else if (vFinalExtents.z >= vFinalExtents.x)
    {
        // 충돌이 Z 축에서 발생.
        if (vPlayerCenter.z > vTargetCenter.z)
        {
            _float vFinalExtents = fabs((vPlayerExtents.z + vTargetExtents.z)) - fabs(vFinalCenter.z);

            Vec4 vPos = pObjTransform->Get_State(CTransform::STATE_POS);
            vPos.z += vFinalExtents;

            pObjTransform->Set_State(CTransform::STATE::STATE_POS, vPos);
        }
        else
        {
            _float vFinalExtents = fabs((vPlayerExtents.z + vTargetExtents.z)) - fabs(vFinalCenter.z);

            Vec4 vPos = pObjTransform->Get_State(CTransform::STATE::STATE_POS);
            vPos.z -= vFinalExtents;

            pObjTransform->Set_State(CTransform::STATE::STATE_POS, vPos);
        }
    }

    return S_OK;
}

HRESULT CCalculator::Detrude_Sphere_Collide(CGameObject* pColObj, CCollider* pObjCol, CTransform* pObjTransform, CNavigation* pNavigation)
{
    if (nullptr == pColObj || nullptr == pObjCol || nullptr == pObjTransform)
        return E_FAIL;

    CCollider* pCollider = dynamic_cast<CCollider*>(pColObj->Get_Component(TEXT("Com_Collider")));

    _float3 vTargetCenter = dynamic_cast<CBounding_Sphere*>(pCollider->Get_ParentBouning())->Get_Bouding()->Center;
    _float3 vPlayerCenter = dynamic_cast<CBounding_Sphere*>(pObjCol->Get_ParentBouning())->Get_Bouding()->Center;

    _float vTargetRadius = dynamic_cast<CBounding_Sphere*>(pCollider->Get_ParentBouning())->Get_Bouding()->Radius;
    _float vPlayerRadius = dynamic_cast<CBounding_Sphere*>(pObjCol->Get_ParentBouning())->Get_Bouding()->Radius;

    _float3 collisionVector = vTargetCenter - vPlayerCenter;
    _float distance = collisionVector.Length();

    // 중심 간의 거리만큼 이동.
    _float overlapDistance = vTargetRadius + vPlayerRadius - distance;
    _float3 collisionNormal = XMVector3Normalize(collisionVector);

    // 이동할 벡터를 계산.
    _float3 translation = collisionNormal * overlapDistance;

    _vector vPos = pObjTransform->Get_State(CTransform::STATE_POS);

    vPos -= _vector(translation) * 0.5f;  // 두 객체를 반씩 밀어냅니다.
    vPos.w = 1;

    if (nullptr != pNavigation)
    {
        _int iMove = pNavigation->IsMove(vPos);

        if (0 == iMove)
        {
            pObjTransform->Set_State(CTransform::STATE_POS, vPos);
        }
    }
    else
        pObjTransform->Set_State(CTransform::STATE::STATE_POS, vPos);
    

    return S_OK;
}

_float3 CCalculator::QuaternionToEuler(_vector vQuaternion)
{
    _float3 euler;

    _matrix rotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    euler.x = atan2f(rotationMatrix(2, 1), rotationMatrix(2, 2));
    euler.y = asinf(-rotationMatrix(2, 0));
    euler.z = atan2f(rotationMatrix(1, 0), rotationMatrix(0, 0));

    return euler;
}

_float CCalculator::Clamp(_float value, _float min, _float max)
{
    if (value < min) return min;
    if (value > max) return max;

    return value;
}

void CCalculator::Free()
{
	__super::Free();
}
