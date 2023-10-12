#include "..\Public\Calculator.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{
}

_vector CCalculator::Picking_Terrain(RECT rc, POINT pt, CTransform* pTransform, CVIBuffer* pBuffer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float3		vMousePos(pt.x, pt.y, 0.f);
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

	SimpleMath::Vector3* pBufferPos = pBuffer->Get_Buffer_Pos();
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

    return _vector(-1.f, -1.f, -1.f, -1.f);
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

    SimpleMath::Vector3* pBufferPos = pBuffer->Get_Buffer_Pos();
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

void CCalculator::Free()
{
	__super::Free();
}
