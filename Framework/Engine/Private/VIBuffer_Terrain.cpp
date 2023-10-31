#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
	ZeroMemory(&m_tTerrainDesc, sizeof TERRAIN_DESC);
}

CVIBuffer_Terrain::CVIBuffer_Terrain(class CGameObject* pOwner, const CVIBuffer_Terrain& rhs)
	: CVIBuffer(pOwner, rhs)
	, m_tTerrainDesc(rhs.m_tTerrainDesc)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	if (TEXT("") == strHeightMapFilePath)
		return S_OK;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	_ulong* pPixel = { nullptr };
	pPixel = new _ulong[ih.biWidth * ih.biHeight];

	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_tTerrainDesc.iNumVerticesX = ih.biWidth;
	m_tTerrainDesc.iNumVerticesZ = ih.biHeight;
	m_iStride = sizeof(VTXPOSNORTEX); /* 정점하나의 크기 .*/
	m_iNumVertices = m_tTerrainDesc.iNumVerticesX * m_tTerrainDesc.iNumVerticesZ;

	m_iIndexStride = 4; 
	m_iNumIndices = (m_tTerrainDesc.iNumVerticesX - 1) * (m_tTerrainDesc.iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER
	VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

	for (size_t i = 0; i < m_tTerrainDesc.iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_tTerrainDesc.iNumVerticesZ; j++)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.f, i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_tTerrainDesc.iNumVerticesX - 1.f), i / (m_tTerrainDesc.iNumVerticesZ - 1.f));

			m_BufferPoses.push_back(pVertices[iIndex].vPosition);
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = 0;

	for (size_t i = 0; i < m_tTerrainDesc.iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_tTerrainDesc.iNumVerticesX - 1; j++)
		{
			_uint		iIndex = i * m_tTerrainDesc.iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_tTerrainDesc.iNumVerticesX,		//5
				iIndex + m_tTerrainDesc.iNumVerticesX + 1,	//6
				iIndex + 1,						//1
				iIndex							//0
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = _vector(pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition);

			vDestDir = _vector(pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			pVertices[iIndices[0]].vNormal = pVertices[iIndices[0]].vNormal + vNormal;
			pVertices[iIndices[1]].vNormal = pVertices[iIndices[1]].vNormal + vNormal;
			pVertices[iIndices[2]].vNormal = pVertices[iIndices[2]].vNormal + vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = _vector(pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition);

			vDestDir = _vector(pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			pVertices[iIndices[0]].vNormal = pVertices[iIndices[0]].vNormal + vNormal;
			pVertices[iIndices[2]].vNormal = pVertices[iIndices[2]].vNormal + vNormal;
			pVertices[iIndices[3]].vNormal = pVertices[iIndices[3]].vNormal + vNormal;
		}
	}

	
	
#pragma endregion

#pragma region Setting
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}


HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TERRAIN_DESC* pTerrain_Desc = (TERRAIN_DESC*)pArg;
		m_tTerrainDesc.iNumVerticesX = pTerrain_Desc->iNumVerticesX;
		m_tTerrainDesc.iNumVerticesZ = pTerrain_Desc->iNumVerticesZ;
		m_tTerrainDesc.bIsWireFrame = pTerrain_Desc->bIsWireFrame;

		m_iStride = sizeof(VTXPOSNORTEX); /* 정점하나의 크기 .*/
		m_iNumVertices = m_tTerrainDesc.iNumVerticesX * m_tTerrainDesc.iNumVerticesZ;

		m_iIndexStride = 4;
		m_iNumIndices = (m_tTerrainDesc.iNumVerticesX - 1) * (m_tTerrainDesc.iNumVerticesZ - 1) * 2 * 3;
		m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_iNumVBs = 1;

#pragma region VERTEX_BUFFER
		VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

		for (size_t i = 0; i < m_tTerrainDesc.iNumVerticesZ; i++)
		{
			for (size_t j = 0; j < m_tTerrainDesc.iNumVerticesX; j++)
			{
				_uint		iIndex = i * m_tTerrainDesc.iNumVerticesX + j;

				pVertices[iIndex].vPosition = _float3(j, 0.f, i);
				pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
				pVertices[iIndex].vTexcoord = _float2(j / (m_tTerrainDesc.iNumVerticesX - 1.f), i / (m_tTerrainDesc.iNumVerticesZ - 1.f));

				m_BufferPoses.push_back(pVertices[iIndex].vPosition);
			}
		}

#pragma endregion

#pragma region INDEX_BUFFER

		_uint* pIndices = new _uint[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

		_uint		iNumIndices = 0;

		for (size_t i = 0; i < m_tTerrainDesc.iNumVerticesZ - 1; i++)
		{
			for (size_t j = 0; j < m_tTerrainDesc.iNumVerticesX - 1; j++)
			{
				_uint		iIndex = i * m_tTerrainDesc.iNumVerticesX + j;

				_uint		iIndices[4] = {
					iIndex + m_tTerrainDesc.iNumVerticesX,		//5
					iIndex + m_tTerrainDesc.iNumVerticesX + 1,	//6
					iIndex + 1,						//1
					iIndex							//0
				};

				_vector		vSourDir, vDestDir, vNormal;

				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];

				vSourDir = _vector(pVertices[iIndices[1]].vPosition - pVertices[iIndices[0]].vPosition);

				vDestDir = _vector(pVertices[iIndices[2]].vPosition - pVertices[iIndices[1]].vPosition);

				vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

				pVertices[iIndices[0]].vNormal = pVertices[iIndices[0]].vNormal + vNormal;
				pVertices[iIndices[1]].vNormal = pVertices[iIndices[1]].vNormal + vNormal;
				pVertices[iIndices[2]].vNormal = pVertices[iIndices[2]].vNormal + vNormal;

				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];

				vSourDir = _vector(pVertices[iIndices[2]].vPosition - pVertices[iIndices[0]].vPosition);

				vDestDir = _vector(pVertices[iIndices[3]].vPosition - pVertices[iIndices[2]].vPosition);

				vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

				pVertices[iIndices[0]].vNormal = pVertices[iIndices[0]].vNormal + vNormal;
				pVertices[iIndices[2]].vNormal = pVertices[iIndices[2]].vNormal + vNormal;
				pVertices[iIndices[3]].vNormal = pVertices[iIndices[3]].vNormal + vNormal;
			}
		}



#pragma endregion

#pragma region Setting
		ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
		m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
		m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_tBufferDesc.CPUAccessFlags = 0;
		m_tBufferDesc.MiscFlags = 0;
		m_tBufferDesc.StructureByteStride = m_iStride;

		ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
		m_tInitialData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);
		m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_tBufferDesc.CPUAccessFlags = 0;
		m_tBufferDesc.MiscFlags = 0;
		m_tBufferDesc.StructureByteStride = 0;

		ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
		m_tInitialData.pSysMem = pIndices;

		if (FAILED(__super::Create_Buffer(&m_pIB)))
			return E_FAIL;

	

		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
#pragma endregion

		return S_OK;
	}

	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(class CGameObject* pOwner, void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pOwner , *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	__super::Free();

}
