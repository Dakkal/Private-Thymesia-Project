#pragma once
#include "Engine_Defines.h"
// 스태틱 메쉬저장
typedef struct  tagSave_Vertex_Mesh_Static
{
	XMFLOAT3	vPosition;
	XMFLOAT3	vNormal;
	XMFLOAT2	vTexcoord;
	XMFLOAT3	vTangent;

}SAVE_VTXMESH_STATIC;

typedef struct tagSave_MeshInfo_Static
{
	 string	strName;
	_uint	iMaterialIndex;
	_uint	iNumVertices;
	_uint	iNumFaces;
	vector<SAVE_VTXMESH_STATIC> vecVtxMeshes;

}SAVE_MESHINFO_STATIC;

typedef struct tagSave_Mesh_Static
{
	_uint	iNumMeshes;
	vector<SAVE_MESHINFO_STATIC> vecMeshInfo;

}SAVE_MESH_STATIC;


// 다이나믹 메쉬저장
typedef struct  tagSave_Vertex_Mesh_Dynamic
{
	XMFLOAT3	vPosition;
	XMFLOAT3	vNormal;
	XMFLOAT2	vTexcoord;
	XMFLOAT3	vTangent;

	XMUINT4		vBlendIndices;
	XMFLOAT4	vBlendWeights;

}SAVE_VTXMESH_DYNAMIC;

typedef struct tagSave_MeshInfo_Dynamic
{
	string	strName;
	_uint	iMaterialIndex;
	_uint	iNumVertices;
	_uint	iNumFaces;
	_uint	iNumBone;
	vector<SAVE_VTXMESH_DYNAMIC> vecVtxMeshes;
	vector<XMFLOAT4X4>  vOffsetMatrix;

}SAVE_MESHINFO_DYNAMIC;

typedef struct tagSave_Mesh_Dynamic
{
	_uint	iNumMeshes;
	vector<SAVE_MESHINFO_DYNAMIC> vecMeshInfo;

}SAVE_MESH_DYNAMIC;


// 머테리얼
typedef struct  tagSave_Material
{
	_uint	iNumMaterial;
	vector<string> MaterialPaths;

}SAVE_MATERIAL;