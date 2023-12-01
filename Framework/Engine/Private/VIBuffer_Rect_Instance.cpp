#include "..\Public\VIBuffer_Rect_Instance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(CGameObject* pOwner, const CVIBuffer_Rect_Instance& rhs)
	: CVIBuffer_Instancing(pOwner, rhs)
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
	INSTANCE_DESC InstanceDesc;
	memmove(&InstanceDesc, pArg, sizeof(InstanceDesc));

	/* 인스턴스용 데이터. 셋. */
	m_iNumInstance = InstanceDesc.iNumInstance;
	m_iNumIndicesPerInstance = 6;

	/* 그리기용 정점을 생성하는 파트. */
	m_iStride = sizeof(VTXPOSTEX); /* 정점하나의 크기 .*/
	m_iNumVertices = 4;

	m_iIndexStride = 2; /* 인덱스 하나의 크기. 2 or 4 */
	m_iNumIndices = 6 * m_iNumInstance;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 2;

#pragma region VERTEX_BUFFER

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	// m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	/* 정점버퍼와 인덱스 버퍼를 만드낟. */
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Update(XYZ xyz, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	/* 파티클의 움직임을 부여한다. */
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (true == m_pIsLifeOver[i]) continue;

		m_pTimeAccs[i] += fTimeDelta;

		if (X == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeeds[i] * fTimeDelta;
		}
		else if (Y == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;
		}
		else if (Z == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeeds[i] * fTimeDelta;
		}
		else if (XY == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeeds[i] * fTimeDelta;
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;
		}
		else if (XZ == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeeds[i] * fTimeDelta;
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeeds[i] * fTimeDelta;
		}
		else if (YZ == xyz)
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeeds[i] * fTimeDelta;
		}

		if (m_pTimeAccs[i] >= m_pLifeTimes[i])
		{
			((VTXINSTANCE*)SubResource.pData)[i].vRight = _vector();

			m_pIsLifeOver[i] = true;
			--m_iLifeCnt;
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);


	if (0 == m_iLifeCnt)
	{
		m_IsUpdate = false;
		m_IsAllLifeOver = true;
	}


	return S_OK;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(class CGameObject* pOwner, void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pOwner , *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();


}
