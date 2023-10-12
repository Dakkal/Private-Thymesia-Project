#pragma once
#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "AsFileUtils.h"
#include "FbxStruct.h"
#include <set>

class CFbxExporter
{
public:
	CFbxExporter();
	~CFbxExporter();

public:
	SAVE_MESH_STATIC	Get_Static_Mesh() { return ModelStaticMesh; };
	SAVE_MATERIAL		Get_Material() { return ModelMaterial; };

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
	HRESULT Export_Animation(const aiScene* pAIScene, CAsFileUtils* pFile);

	HRESULT Import_Static_Mesh(CAsFileUtils* pFile);
	HRESULT Import_Material(CAsFileUtils* pFile);
	HRESULT Import_Bone(CAsFileUtils* pFile);
	HRESULT Import_Animation(CAsFileUtils* pFile);

private:
	wstring fileExtension = { TEXT(".fbx") };
	set<filesystem::path> visitedDirectories;

private:
	SAVE_MESH_STATIC	ModelStaticMesh;
	SAVE_MATERIAL		ModelMaterial;
};