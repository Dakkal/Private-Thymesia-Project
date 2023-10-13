#pragma once
#include "Engine_Defines.h"
#include "AsFileUtils.h"
#include "FbxStruct.h"

class ENGINE_DLL CFbxExporter
{
public:
	CFbxExporter();
	~CFbxExporter();

public:
	SAVE_MESH_STATIC	Get_Static_Mesh() { return ModelStaticMesh; };
	SAVE_MESH_DYNAMIC	Get_Dynamic_Mesh() { return ModelDynamicMesh; };
	SAVE_MATERIAL		Get_Material() { return ModelMaterial; };
	SAVE_BONE			Get_Bone() { return ModelBone; }
	SAVE_ANIM			Get_Anim() { return ModelAnim; }

public:
	HRESULT Initialize_Static_Export(const wstring& FbxImportFolderPath);
	HRESULT Initialize_Dynamic_Export(const wstring& FbxImportFolderPath);

	HRESULT Start_Static_Import(const wstring& batPath);
	HRESULT Start_Dynamic_Import(const wstring& batPath);

private:
	HRESULT Start_Static_Export(const wstring& FbxPath);
	HRESULT Start_Dynamic_Export(const wstring& FbxPath);

	HRESULT Export_Static_Mesh(const aiScene* pAIScene, CAsFileUtils* pFile);
	HRESULT Export_Dynamic_Mesh(const aiScene* pAIScene, CAsFileUtils* pFile);
	HRESULT Export_Material(const aiScene* pAIScene, CAsFileUtils* pFile);
	HRESULT Export_Bone(const aiScene* pAIScene, CAsFileUtils* pFile);
	HRESULT Export_BoneNode(const aiNode* pAINode, _int iParentBoneIndex, CAsFileUtils* pFile);
	HRESULT Export_Animation(const aiScene* pAIScene, CAsFileUtils* pFile);

	HRESULT Import_Static_Mesh(CAsFileUtils* pFile);
	HRESULT Import_Dynamic_Mesh(CAsFileUtils* pFile);
	HRESULT Import_Material(CAsFileUtils* pFile);
	HRESULT Import_Bone(CAsFileUtils* pFile);
	HRESULT Import_Animation(CAsFileUtils* pFile);

private:
	_int	Get_BoneIndex(const string& pBoneName) const;

private:
	wstring fileExtension = { TEXT(".fbx") };
	set<filesystem::path> visitedDirectories;

	set<_int> visitedChildren;
	_int	  iNodeStop = { 0 };
	_int	  iImportNodeStop = { 0 };

private:
	SAVE_MESH_STATIC	ModelStaticMesh;
	SAVE_MESH_DYNAMIC	ModelDynamicMesh;
	SAVE_MATERIAL		ModelMaterial;
	SAVE_BONE			ModelBone;
	SAVE_ANIM			ModelAnim;
};