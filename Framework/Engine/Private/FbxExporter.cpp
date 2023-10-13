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

    for (const auto& entry : fs::directory_iterator(FbxImportFolderPath)) 
    {
        if (fs::is_directory(entry)) 
        {
            Initialize_Static_Export(entry.path());
        }
        else if (fs::is_regular_file(entry)) 
        {
            if (fileExtension == entry.path().extension()) 
            {
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

    for (const auto& entry : fs::directory_iterator(FbxImportFolderPath)) 
    {
        if (fs::is_directory(entry)) 
        {
            Initialize_Dynamic_Export(entry.path());
        }
        else if (fs::is_regular_file(entry)) 
        {

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


    _uint iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    Assimp::Importer Import;

    const aiScene* pAIScene = Import.ReadFile(fbxPath.c_str(), iFlag);

    if (FAILED(Export_Static_Mesh(pAIScene, Save.get())))
        return E_FAIL;

    if (FAILED(Export_Material(pAIScene, fbxPath, Save.get())))
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

    _uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    Assimp::Importer Import;
    const aiScene* pAIScene = Import.ReadFile(fbxPath.c_str(), iFlag);

    if (FAILED(Export_Bone(pAIScene, Save.get())))
        return E_FAIL;

    if (FAILED(Export_Dynamic_Mesh(pAIScene, Save.get())))
        return E_FAIL;
    
    if (FAILED(Export_Material(pAIScene, fbxPath, Save.get())))
        return E_FAIL;

    if (FAILED(Export_Animation(pAIScene, Save.get())))
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

        for (size_t k = 0; k < pMesh->mNumFaces; k++)
        {
            pFile->Write<_ulong>(pMesh->mFaces[k].mIndices[0]);
            pFile->Write<_ulong>(pMesh->mFaces[k].mIndices[1]);
            pFile->Write<_ulong>(pMesh->mFaces[k].mIndices[2]);
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Dynamic_Mesh(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    pFile->Write<_uint>(pAIScene->mNumMeshes);

    for (size_t i = 0; i < pAIScene->mNumMeshes; i++)
    {
        const aiMesh* pMesh = pAIScene->mMeshes[i];

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
            const aiBone* pBone = pMesh->mBones[k];

            XMFLOAT4X4 vOffsetMatrix;
            memcpy(&vOffsetMatrix, &pBone->mOffsetMatrix, sizeof(XMFLOAT4X4));
            XMStoreFloat4x4(&vOffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&vOffsetMatrix)));
            pFile->Write<XMFLOAT4X4>(vOffsetMatrix);
            pFile->Write<string>(pBone->mName.data);

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
        if (0 == pMesh->mNumBones)
        {
            _matrix matIdentity;
            XMFLOAT4X4 savemat = matIdentity;
            pFile->Write<XMFLOAT4X4>(savemat);
            pFile->Write<string>(pMesh->mName.data);
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


        for (size_t o = 0; o < pMesh->mNumFaces; o++)
        {
            pFile->Write<_ulong>(pMesh->mFaces[o].mIndices[0]);
            pFile->Write<_ulong>(pMesh->mFaces[o].mIndices[1]);
            pFile->Write<_ulong>(pMesh->mFaces[o].mIndices[2]);
        }

        Safe_Delete_Array(pVertices);
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Material(const aiScene* pAIScene, const string& strExportPath, CAsFileUtils* pFile)
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
              
                char			szDrive[MAX_PATH] = "";
                char			szDirectory[MAX_PATH] = "";
                _splitpath_s(strExportPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

                char			szFileName[MAX_PATH] = "";
                char			szExt[MAX_PATH] = "";
                _splitpath_s(strTexturePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

                char			szFullPath[MAX_PATH] = "";
                strcpy_s(szFullPath, szDrive);
                strcat_s(szFullPath, szDirectory);
                strcat_s(szFullPath, szFileName);
                strcat_s(szFullPath, szExt);

                pFile->Write<string>(szFullPath);
            }
        }
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Bone(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    Export_BoneNode(pAIScene->mRootNode, -1, pFile);

    iNodeStop = -99;
    pFile->Write<_int>(iNodeStop);

    return S_OK;
}

HRESULT CFbxExporter::Export_BoneNode(const aiNode* pAINode, _int iParentBoneIndex, CAsFileUtils* pFile)
{
    pFile->Write<_int>(iNodeStop);
    pFile->Write<_int>(iParentBoneIndex);
    pFile->Write<string>(pAINode->mName.data);
    XMFLOAT4X4 TransformMatrix;
    memcpy(&TransformMatrix, &pAINode->mTransformation, sizeof(XMFLOAT4X4));
    XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransformMatrix)));
    pFile->Write<XMFLOAT4X4>(TransformMatrix);
  
    vecCheck.push_back(0);
    _int iParentIndex = vecCheck.size() - 1;

    for (size_t i = 0; i < pAINode->mNumChildren; i++)
    {
        Export_BoneNode(pAINode->mChildren[i], iParentIndex, pFile);
    }

    return S_OK;
}

HRESULT CFbxExporter::Export_Animation(const aiScene* pAIScene, CAsFileUtils* pFile)
{
    pFile->Write<_uint>(pAIScene->mNumAnimations);

    for (size_t i = 0; i < pAIScene->mNumAnimations; i++)
    {
        aiAnimation* pAnim = pAIScene->mAnimations[i];

        pFile->Write<string>(pAnim->mName.data);
        pFile->Write<_float>(pAnim->mDuration);
        pFile->Write<_float>(pAnim->mTicksPerSecond);
        pFile->Write<_uint>(pAnim->mNumChannels);

        for (size_t j = 0; j < pAIScene->mAnimations[i]->mNumChannels; j++)
        {
            aiNodeAnim* pChannel = pAnim->mChannels[j];

            pFile->Write<string>(pChannel->mNodeName.data);

            _uint iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
            iNumKeyFrames = max(iNumKeyFrames, pChannel->mNumPositionKeys);

            pFile->Write<_uint>(iNumKeyFrames);

            _float  fTime;
            XMFLOAT3 vScale;
            XMFLOAT4 vRotation;
            XMFLOAT4 vTranslation;

            for (size_t k = 0; k < iNumKeyFrames; k++)
            {
                if (pChannel->mNumScalingKeys > k)
                {
                    memcpy(&vScale, &pChannel->mScalingKeys[k].mValue, sizeof(XMFLOAT3));
                    fTime = pChannel->mScalingKeys[k].mTime;
                }

                if (pChannel->mNumRotationKeys > k)
                {
                    vRotation.x = pChannel->mRotationKeys[k].mValue.x;
                    vRotation.y = pChannel->mRotationKeys[k].mValue.y;
                    vRotation.z = pChannel->mRotationKeys[k].mValue.z;
                    vRotation.w = pChannel->mRotationKeys[k].mValue.w;
                    fTime = pChannel->mRotationKeys[k].mTime;
                }

                if (pChannel->mNumPositionKeys > k)
                {
                    memcpy(&vTranslation, &pChannel->mPositionKeys[k].mValue, sizeof(XMFLOAT4));
                    vTranslation.w = 1.f;
                    fTime = pChannel->mPositionKeys[k].mTime;
                }

                pFile->Write<_float>(fTime);
                pFile->Write<XMFLOAT3>(vScale);
                pFile->Write<XMFLOAT4>(vRotation);
                pFile->Write<XMFLOAT4>(vTranslation);
            }
        }
    }

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
    shared_ptr<CAsFileUtils> Load = make_shared<CAsFileUtils>();
    Load->Open(batPath, FileMode::Read);

    if (FAILED(Import_Bone(Load.get())))
        return E_FAIL;

    if (FAILED(Import_Dynamic_Mesh(Load.get())))
        return E_FAIL;

    if (FAILED(Import_Material(Load.get())))
        return E_FAIL;

    if (FAILED(Import_Animation(Load.get())))
        return E_FAIL;

    return S_OK;
}

HRESULT CFbxExporter::Import_Static_Mesh(CAsFileUtils* pFile)
{
   pFile->Read<_uint>(ModelMesh.iNumMeshes);

   for (size_t i = 0; i < ModelMesh.iNumMeshes; i++)
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

       _ulong a, b, c;
       for (size_t k = 0; k < MeshInfoDesc.iNumFaces; k++)
       {
           pFile->Read<_ulong>(a);
           pFile->Read<_ulong>(b);
           pFile->Read<_ulong>(c);

           MeshInfoDesc.vecIdicies.push_back(a);
           MeshInfoDesc.vecIdicies.push_back(b);
           MeshInfoDesc.vecIdicies.push_back(c);
       }

       ModelMesh.vecMeshStaticInfo.push_back(MeshInfoDesc);
   }

   return S_OK;
}

HRESULT CFbxExporter::Import_Dynamic_Mesh(CAsFileUtils* pFile)
{
    pFile->Read<_uint>(ModelMesh.iNumMeshes);

   
    for (size_t i = 0; i < ModelMesh.iNumMeshes; i++)
    {
        SAVE_MESHINFO_DYNAMIC meshinfo;

        pFile->Read(meshinfo.strName);
        pFile->Read<_uint>(meshinfo.iMaterialIndex);
        pFile->Read<_uint>(meshinfo.iNumFaces);
        pFile->Read<_uint>(meshinfo.iNumVertices);

        pFile->Read<_uint>(meshinfo.iNumBone);
        for (size_t k = 0; k < meshinfo.iNumBone; k++)
        {
            XMFLOAT4X4 vOffsetMatrix;
            pFile->Read<XMFLOAT4X4>(vOffsetMatrix);
            meshinfo.vecOffsetMatrix.push_back(vOffsetMatrix);
            string strName;
            pFile->Read(strName);
            meshinfo.BoneIndex.push_back(Get_BoneIndex(strName));
        }
        if (0 == meshinfo.iNumBone)
        {
            meshinfo.iNumBone = 1;

            XMFLOAT4X4 vOffsetMatrix;
            pFile->Read<XMFLOAT4X4>(vOffsetMatrix);
            meshinfo.vecOffsetMatrix.push_back(vOffsetMatrix);
            string strName;
            pFile->Read(strName);
            meshinfo.BoneIndex.push_back(Get_BoneIndex(strName));
        }

        for (size_t j = 0; j < meshinfo.iNumVertices; j++)
        {
            SAVE_VTXMESH_DYNAMIC vtxinfo;
            pFile->Read<XMFLOAT3>(vtxinfo.vPosition);
            pFile->Read<XMFLOAT3>(vtxinfo.vNormal);
            pFile->Read<XMFLOAT2>(vtxinfo.vTexcoord);
            pFile->Read<XMFLOAT3>(vtxinfo.vTangent);
            pFile->Read<XMUINT4>(vtxinfo.vBlendIndices);
            pFile->Read<XMFLOAT4>(vtxinfo.vBlendWeights);

            meshinfo.vecVtxMeshes.push_back(vtxinfo);
        }

        _ulong a, b, c;
        for (size_t k = 0; k < meshinfo.iNumFaces; k++)
        {
            pFile->Read<_ulong>(a);
            pFile->Read<_ulong>(b);
            pFile->Read<_ulong>(c);

            meshinfo.vecIdicies.push_back(a);
            meshinfo.vecIdicies.push_back(b);
            meshinfo.vecIdicies.push_back(c);
        }

        ModelMesh.vecMeshDynamicInfo.push_back(meshinfo);
    }

    return S_OK;
}

HRESULT CFbxExporter::Import_Material(CAsFileUtils* pFile)
{
    pFile->Read<_uint>(ModelMaterial.iNumMaterial);

    for (size_t i = 0; i < ModelMaterial.iNumMaterial; i++)
    {
        SAVE_MATERIAL_INFO MaterilInfo;

        for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
        {
            string      strTexturePath;
            pFile->Read(strTexturePath);

            MaterilInfo.FilePaths.push_back(strTexturePath);
        }

        ModelMaterial.vecMaterialPaths.push_back(MaterilInfo);
    }

    return S_OK;
}

HRESULT CFbxExporter::Import_Bone(CAsFileUtils* pFile)
{
    while (true)
    {
        pFile->Read<_int>(iImportNodeStop);
        if (-99 == iImportNodeStop)
             return S_OK;

        SAVE_BONE_INFO BoneInfo;

        pFile->Read<_int>(BoneInfo.iParentBoneIndex);
        pFile->Read(BoneInfo.strBoneName);
        pFile->Read<XMFLOAT4X4>(BoneInfo.TransformMatrix);

        ModelBone.Bones.push_back(BoneInfo);   
    }
   
    return S_OK;
}


HRESULT CFbxExporter::Import_Animation(CAsFileUtils* pFile)
{
    pFile->Read<_uint>(ModelAnim.iNumAnim);

   
    for (size_t i = 0; i < ModelAnim.iNumAnim; i++)
    {
        SAVE_ANIM_INFO animdesc;

        pFile->Read(animdesc.strAnimName);
        pFile->Read<_float>(animdesc.fDuration);
        pFile->Read<_float>(animdesc.fTickPerSecond);
        pFile->Read<_uint>(animdesc.iNumChannel);

        for (size_t j = 0; j < animdesc.iNumChannel; j++)
        {
            SAVE_CHANNEL channeldesc;
            pFile->Read(channeldesc.strChannelName);
            channeldesc.iBoneIndex = Get_BoneIndex(channeldesc.strChannelName);
            pFile->Read<_uint>(channeldesc.iNumKeyFrame);

            for (size_t k = 0; k < channeldesc.iNumKeyFrame; k++)
            {
                SAVE_KEYFRAME keyframedesc;

                pFile->Read<_float>(keyframedesc.fTime);
                pFile->Read<XMFLOAT3>(keyframedesc.vScale);
                pFile->Read<XMFLOAT4>(keyframedesc.vRotation);
                pFile->Read<XMFLOAT4>(keyframedesc.vTranslation);

                channeldesc.vecKeyFrames.push_back(keyframedesc);
            }

            animdesc.vecChannels.push_back(channeldesc);
        }

        ModelAnim.vecAnim.push_back(animdesc);
    }

    return S_OK;
}

_int CFbxExporter::Get_BoneIndex(const string& pBoneName) const
{
    _uint	iBoneIndex = 0;

    auto	iter = find_if(ModelBone.Bones.begin(), ModelBone.Bones.end(), [&](SAVE_BONE_INFO pBoneInfo){
            
        if (pBoneInfo.strBoneName == pBoneName)
                return true;

            ++iBoneIndex;

            return false;
     });

    if (iter == ModelBone.Bones.end())
        return -1;

    return iBoneIndex;
}

