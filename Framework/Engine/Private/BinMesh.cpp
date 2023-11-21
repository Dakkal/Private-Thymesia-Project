#include "../Public/BinMesh.h"
#include "Shader.h"
#include "BinBone.h"

CBinMesh::CBinMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CBinMesh::CBinMesh(CGameObject* pOwner, const CBinMesh& rhs)
	: CVIBuffer(pOwner, rhs)
{
}

HRESULT CBinMesh::Initialize_Prototype(CBinModel::TYPE eModelType, SAVE_MESHINFO_STATIC tStaticMesh, _matrix PivotMatrix)
{
	m_strName = tStaticMesh.strName;

	m_iStride = CBinModel::TYPE_NONANIM == eModelType ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);

	m_iMaterialIndex = tStaticMesh.iMaterialIndex;
	m_iNumVertices = tStaticMesh.iNumVertices;
	m_iIndexStride = 4;
	m_iNumIndices = tStaticMesh.iNumFaces * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER 
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = tStaticMesh.vecVtxMeshes[i].vPosition;
		pVertices[i].vPosition = XMVector3TransformCoord(pVertices[i].vPosition, PivotMatrix);

		pVertices[i].vNormal = tStaticMesh.vecVtxMeshes[i].vNormal;
		pVertices[i].vNormal = XMVector3TransformNormal(pVertices[i].vNormal, PivotMatrix);

		pVertices[i].vTexcoord = tStaticMesh.vecVtxMeshes[i].vTexcoord;

		pVertices[i].vTangent = tStaticMesh.vecVtxMeshes[i].vTangent;
		pVertices[i].vTangent = XMVector3TransformNormal(pVertices[i].vTangent, PivotMatrix);
		m_BufferPoses.push_back(pVertices[i].vPosition);
	}

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint	iNumIndices = 0;

	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = tStaticMesh.vecIdicies[i];
	}
	m_Indicies = tStaticMesh.vecIdicies;

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CBinMesh::Initialize_Prototype(CBinModel::TYPE eModelType, SAVE_MESHINFO_DYNAMIC tDynamicMesh, _matrix PivotMatrix)
{
	m_strName = tDynamicMesh.strName;

	m_iStride = CBinModel::TYPE_NONANIM == eModelType ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);

	m_iMaterialIndex = tDynamicMesh.iMaterialIndex;
	m_iNumVertices = tDynamicMesh.iNumVertices;
	m_iIndexStride = 4;
	m_iNumIndices = tDynamicMesh.iNumFaces * 3;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumVBs = 1;

#pragma region VERTEX_BUFFER 
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = m_iStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vPosition = tDynamicMesh.vecVtxMeshes[i].vPosition;
		pVertices[i].vNormal = tDynamicMesh.vecVtxMeshes[i].vNormal;
		pVertices[i].vTexcoord = tDynamicMesh.vecVtxMeshes[i].vTexcoord;
		pVertices[i].vTangent = tDynamicMesh.vecVtxMeshes[i].vTangent;
		pVertices[i].vBlendIndices = tDynamicMesh.vecVtxMeshes[i].vBlendIndices;
		pVertices[i].vBlendWeights = tDynamicMesh.vecVtxMeshes[i].vBlendWeights;
	}

	m_iNumBones = tDynamicMesh.iNumBone;
	m_OffsetMatrices = tDynamicMesh.vecOffsetMatrix;
	m_Bones = tDynamicMesh.BoneIndex;

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_tBufferDesc, sizeof m_tBufferDesc);

	m_tBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tBufferDesc.CPUAccessFlags = 0;
	m_tBufferDesc.MiscFlags = 0;
	m_tBufferDesc.StructureByteStride = 0;

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint	iNumIndices = 0;

	for (size_t i = 0; i < m_iNumIndices; i++)
	{
		pIndices[i] = tDynamicMesh.vecIdicies[i];
	}
	m_Indicies = tDynamicMesh.vecIdicies;

	ZeroMemory(&m_tInitialData, sizeof m_tInitialData);
	m_tInitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CBinMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CBinMesh::Bind_BoneMatrices(CShader* pShader, const vector<class CBinBone*>& Bones, const char* pConstantName, _matrix PivotMatrix)
{
	_matrix		BoneMatrices[256];
	ZeroMemory(BoneMatrices, sizeof(_matrix) * 256);

	_uint		iIndex = 0;

	for (auto iBoneIndex : m_Bones)
	{
		_matrix Combined_Bone = Bones[iBoneIndex]->Get_CombinedTransform();

		BoneMatrices[iIndex++] = (m_OffsetMatrices[iIndex] * Combined_Bone) * PivotMatrix;
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, m_iNumBones);
}

CBinMesh* CBinMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBinModel::TYPE eModelType, const SAVE_MESHINFO_STATIC tMesh, _matrix PivotMatrix)
{
	CBinMesh* pInstance = new CBinMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, tMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBinMesh* CBinMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBinModel::TYPE eModelType, const SAVE_MESHINFO_DYNAMIC tMesh, _matrix PivotMatrix)
{
	CBinMesh* pInstance = new CBinMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, tMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBinMesh::Clone(CGameObject* pOwner, void* pArg)
{
	CBinMesh* pInstance = new CBinMesh(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBinMesh::Free()
{
	__super::Free();
}
