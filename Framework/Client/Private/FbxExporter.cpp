#include "..\Public\FbxExporter.h"
#include "AsFileUtils.h"

namespace fs = std::filesystem;

CFbxExporter::CFbxExporter()
{

}

CFbxExporter::~CFbxExporter()
{
}

HRESULT CFbxExporter::Initialize_Static_Export(const wstring& FbxImportFolderPath)
{
    if (visitedDirectories.count(FbxImportFolderPath) > 0)
        return S_OK;

    visitedDirectories.insert(FbxImportFolderPath);

    for (const auto& entry : fs::directory_iterator(FbxImportFolderPath)) {
        if (fs::is_directory(entry)) {
            Initialize_Static_Export(entry.path());
        }
        else if (fs::is_regular_file(entry)) {
            if (fileExtension == entry.path().extension()) {
                if (FAILED(Start_Static_Export(entry.path())))
                    return E_FAIL;
            }
        }
    }

	return S_OK;
}

HRESULT CFbxExporter::Initialize_Dynamic_Export(const wstring& FbxImportFolderPath)
{
    if (visitedDirectories.count(FbxImportFolderPath) > 0)
        return S_OK;

    visitedDirectories.insert(FbxImportFolderPath);

    for (const auto& entry : fs::directory_iterator(FbxImportFolderPath)) {
        if (fs::is_directory(entry)) {
            Initialize_Dynamic_Export(entry.path());
        }
        else if (fs::is_regular_file(entry)) {

            if (fileExtension == entry.path().extension())
            {
                if (FAILED(Start_Dynamic_Export(entry.path())))
                    return E_FAIL;
            }
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Start_Static_Export(const wstring& FbxPath)
{
    string fbxPath;
    fbxPath.assign(FbxPath.begin(), FbxPath.end());

    char			szDrive[MAX_PATH] = "";
    char			szDirectory[MAX_PATH] = "";
    char			szFileName[MAX_PATH] = "";
    _splitpath_s(fbxPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

    char			szOpenName[MAX_PATH] = "";
    strcat_s(szOpenName, szDrive);
    strcat_s(szOpenName, szDirectory);
    strcat_s(szOpenName, szFileName);
    strcat_s(szOpenName, ".dat");

    _tchar			szWOpenName[MAX_PATH] = TEXT("");
    MultiByteToWideChar(CP_ACP, 0, szOpenName, strlen(szOpenName), szWOpenName, MAX_PATH);

    shared_ptr<CAsFileUtils> Save = make_shared<CAsFileUtils>();
    Save->Open(szWOpenName, FileMode::Write);

    Assimp::Importer Import;

    _uint iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    const aiScene* pAIScene = Import.ReadFile(fbxPath.c_str(), iFlag);

    if (FAILED(Export_Static_Mesh(pAIScene, Save.get())))
        return E_FAIL;

    if (FAILED(Export_Material(pAIScene, Save.get())))
        return E_FAIL;

    return S_OK;
}

HRESULT CFbxExporter::Start_Dynamic_Export(const wstring& FbxPath)
{
    string fbxPath;
    fbxPath.assign(FbxPath.begin(), FbxPath.end());

    char			szDrive[MAX_PATH] = "";
    char			szDirectory[MAX_PATH] = "";
    char			szFileName[MAX_PATH] = "";
    _splitpath_s(fbxPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

    char			szOpenName[MAX_PATH] = "";
    strcat_s(szOpenName, szDrive);
    strcat_s(szOpenName, szDirectory);
    strcat_s(szOpenName, szFileName);
    strcat_s(szOpenName, ".dat");

    _tchar			szWOpenName[MAX_PATH] = TEXT("");
    MultiByteToWideChar(CP_ACP, 0, szOpenName, strlen(szOpenName), szWOpenName, MAX_PATH);

    shared_ptr<CAsFileUtils> Save = make_shared<CAsFileUtils>();
    Save->Open(szWOpenName, FileMode::Write);

    Assimp::Importer Import;

    _uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    const aiScene* pAIScene = Import.ReadFile(fbxPath.c_str(), iFlag);

    if (FAILED(Export_Dynamic_Mesh(pAIScene, Save.get())))
        return E_FAIL;
    
    if (FAILED(Export_Material(pAIScene, Save.get())))
        return E_FAIL;

    return S_OK;
}

HRESULT CFbxExporter::Export_Static_Mesh(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    pFile->Write<_uint>(pAIScene->mNumMeshes);

    for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
    {
        aiMesh* pMesh = pAIScene->mMeshes[i];

        pFile->Write<string>(pMesh->mName.data);
        pFile->Write<_uint>(pMesh->mMaterialIndex);
        pFile->Write<_uint>(pMesh->mNumFaces);
        pFile->Write<_uint>(pMesh->mNumVertices);

        for (size_t j = 0; j < pMesh->mNumVertices; j++)
        {
            SAVE_VTXMESH_STATIC VtxDesc;

            memcpy(&VtxDesc.vPosition, &pMesh->mVertices[j], sizeof(XMFLOAT3));
            memcpy(&VtxDesc.vNormal, &pMesh->mNormals[j], sizeof(XMFLOAT3));
            memcpy(&VtxDesc.vTexcoord, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT2));
            memcpy(&VtxDesc.vTangent, &pMesh->mTangents[j], sizeof(XMFLOAT3));

            pFile->Write<XMFLOAT3>(VtxDesc.vPosition);
            pFile->Write<XMFLOAT3>(VtxDesc.vNormal);
            pFile->Write<XMFLOAT2>(VtxDesc.vTexcoord);
            pFile->Write<XMFLOAT3>(VtxDesc.vTangent);
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Dynamic_Mesh(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    pFile->Write<_uint>(pAIScene->mNumMeshes);

    for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
    {
        aiMesh* pMesh = pAIScene->mMeshes[i];

        pFile->Write<string>(pMesh->mName.data);
        pFile->Write<_uint>(pMesh->mMaterialIndex);
        pFile->Write<_uint>(pMesh->mNumFaces);
        pFile->Write<_uint>(pMesh->mNumVertices);

        SAVE_VTXMESH_DYNAMIC* pVertices = new SAVE_VTXMESH_DYNAMIC[pMesh->mNumVertices];

        for (size_t j = 0; j < pMesh->mNumVertices; j++)
        {
            memcpy(&pVertices[j].vPosition, &pMesh->mVertices[j], sizeof(XMFLOAT3));
            memcpy(&pVertices[j].vNormal, &pMesh->mNormals[j], sizeof(XMFLOAT3));
            memcpy(&pVertices[j].vTexcoord, &pMesh->mTextureCoords[0][j], sizeof(XMFLOAT2));
            memcpy(&pVertices[j].vTangent, &pMesh->mTangents[j], sizeof(XMFLOAT3));
        }

        pFile->Write<_uint>(pMesh->mNumBones);
        for (size_t k = 0; k < pMesh->mNumBones; k++)
        {
            aiBone* pBone = pMesh->mBones[k];

            XMFLOAT4X4 vOffsetMatrix;
            memcpy(&vOffsetMatrix, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4));
            XMStoreFloat4x4(&vOffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&vOffsetMatrix)));
            pFile->Write<XMFLOAT4X4>(vOffsetMatrix);

            for (size_t l = 0; l < pBone->mNumWeights; ++l)
            {
                if (0.f == pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.x)
                {
                    pVertices[pBone->mWeights[l].mVertexId].vBlendIndices.x = k;
                    pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.x = pBone->mWeights[l].mWeight;
                }

                else if (0.f == pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.y)
                {
                    pVertices[pBone->mWeights[l].mVertexId].vBlendIndices.y = k;
                    pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.y = pBone->mWeights[l].mWeight;
                }

                else if (0.f == pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.z)
                {
                    pVertices[pBone->mWeights[l].mVertexId].vBlendIndices.z = k;
                    pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.z = pBone->mWeights[l].mWeight;
                }

                else if (0.f == pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.w)
                {
                    pVertices[pBone->mWeights[l].mVertexId].vBlendIndices.w = k;
                    pVertices[pBone->mWeights[l].mVertexId].vBlendWeights.w = pBone->mWeights[l].mWeight;
                }
            }
        }

        for (size_t m = 0; m < pMesh->mNumVertices; m++)
        {
            pFile->Write<XMFLOAT3>(pVertices[m].vPosition);
            pFile->Write<XMFLOAT3>(pVertices[m].vNormal);
            pFile->Write<XMFLOAT2>(pVertices[m].vTexcoord);
            pFile->Write<XMFLOAT3>(pVertices[m].vTangent);
            pFile->Write<XMUINT4>(pVertices[m].vBlendIndices);
            pFile->Write<XMFLOAT4>(pVertices[m].vBlendWeights);
        }

        Safe_Delete_Array(pVertices);
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Material(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    pFile->Write<_uint>(pAIScene->mNumMaterials);

    for (size_t i = 0; i < pAIScene->mNumMaterials; i++)
    {
        aiMaterial* pMaterial = pAIScene->mMaterials[i];

        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            aiString	strAITexturePath;

            if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strAITexturePath)))
            {
                string      strTexturePath = "";
                pFile->Write<string>(strTexturePath);
            }
            else
            {
                string      strTexturePath = strAITexturePath.data;
                pFile->Write<string>(strTexturePath);
            }
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Bone(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    return S_OK;
}

HRESULT CFbxExporter::Export_Animation(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    return S_OK;
}

HRESULT CFbxExporter::Start_Static_Import(const wstring& batPath)
{
    shared_ptr<CAsFileUtils> Load = make_shared<CAsFileUtils>();
    Load->Open(batPath, FileMode::Read);

    if (FAILED(Import_Static_Mesh(Load.get())))
        return E_FAIL;

    if (FAILED(Import_Material(Load.get())))
        return E_FAIL;

    return S_OK;
}

HRESULT CFbxExporter::Start_Dynamic_Import(const wstring& batPath)
{
    return S_OK;
}

HRESULT CFbxExporter::Import_Static_Mesh(CAsFileUtils* pFile)
{
   pFile->Read<_uint>(ModelStaticMesh.iNumMeshes);

   for (size_t i = 0; i < ModelStaticMesh.iNumMeshes; i++)
   {
       SAVE_MESHINFO_STATIC MeshInfoDesc;

       pFile->Read(MeshInfoDesc.strName);
       pFile->Read<_uint>(MeshInfoDesc.iMaterialIndex);
       pFile->Read<_uint>(MeshInfoDesc.iNumFaces);
       pFile->Read<_uint>(MeshInfoDesc.iNumVertices);

       for (size_t j = 0; j < MeshInfoDesc.iNumVertices; j++)
       {
           SAVE_VTXMESH_STATIC VtxDesc;

           pFile->Read<XMFLOAT3>(VtxDesc.vPosition);
           pFile->Read<XMFLOAT3>(VtxDesc.vNormal);
           pFile->Read<XMFLOAT2>(VtxDesc.vTexcoord);
           pFile->Read<XMFLOAT3>(VtxDesc.vTangent);

           MeshInfoDesc.vecVtxMeshes.push_back(VtxDesc);
       }

       ModelStaticMesh.vecMeshInfo.push_back(MeshInfoDesc);
   }

   return S_OK;
}

HRESULT CFbxExporter::Import_Material(CAsFileUtils* pFile)
{
    pFile->Read<_uint>(ModelMaterial.iNumMaterial);

    for (size_t i = 0; i < ModelMaterial.iNumMaterial; i++)
    {
        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            string      strTexturePath;
            pFile->Read(strTexturePath);

            ModelMaterial.MaterialPaths.push_back(strTexturePath);
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Import_Bone(CAsFileUtils* pFile)
{
    return S_OK;
}

HRESULT CFbxExporter::Import_Animation(CAsFileUtils* pFile)
{
    return S_OK;
}

