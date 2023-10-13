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

	XMUINT4		vBlendIndices = { 0, 0, 0, 0 };
	XMFLOAT4	vBlendWeights = { 0.f, 0.f, 0.f, 0.f };

}SAVE_VTXMESH_DYNAMIC;

typedef struct tagSave_MeshInfo_Dynamic
{
	string	strName;
	_uint	iMaterialIndex;
	_uint	iNumVertices;
	_uint	iNumFaces;
	_uint	iNumBone;

	vector<SAVE_VTXMESH_DYNAMIC> vecVtxMeshes;
	vector<XMFLOAT4X4> vecOffsetMatrix;
	vector<_int> BoneIndex;

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

// 뼈
typedef struct  tagSave_Bone_Info
{
	_int		iParentBoneIndex;
	string		strBoneName;
	XMFLOAT4X4	TransformMatrix;

}SAVE_BONE_INFO;

typedef struct  tagSave_Bone
{
	vector<SAVE_BONE_INFO> Bones;

}SAVE_BONE;

//채널
typedef struct  tagSave_KeyFrame
{
	_float	fTime;
	XMFLOAT3 vScale;
	XMFLOAT4 vRotation;
	XMFLOAT4 vTranslation;

}SAVE_KEYFRAME;

typedef struct  tagSave_Channel
{
	string		strChannelName;
	_int		iBoneIndex;
	_uint		iNumKeyFrame;
	vector<SAVE_KEYFRAME> vecKeyFrames;

}SAVE_CHANNEL;

//애니메이션
typedef struct  tagSave_Anim_Info
{
	string		strAnimName;
	_float		fDuration;
	_float		fTickPerSecond;

	_uint		iNumChannel;
	vector<SAVE_CHANNEL> vecChannels;

}SAVE_ANIM_INFO;

typedef struct  tagSave_Anim
{
	_uint iNumAnim;
	vector<SAVE_ANIM_INFO> vecAnim;

}SAVE_ANIM;


