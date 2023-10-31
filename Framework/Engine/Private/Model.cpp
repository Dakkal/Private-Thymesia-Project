#include "..\Public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
	: CComponent(pDeivce, pContext)
{
}

CModel::CModel(CGameObject* pOwner, const CModel& rhs)
	: CComponent(pOwner, rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnims(rhs. m_iNumAnims)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		m_Animations.push_back(pPrototypeAnimation->Clone());
	}

	for (auto& pPrototypeBone : rhs.m_Bones)
	{
		m_Bones.push_back(pPrototypeBone->Clone());
	}

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_AddRef(Material.pTextures[i]);
		}
	}

	for (auto& pMesh : m_Meshes)
	{
		Safe_AddRef(pMesh);
	}
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix)
{
	m_eModelType = eModelType;

	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (TYPE_NONANIM == eModelType)
		iFlag |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_PivotMatrix = PivotMatrix;

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Set_Animation(_bool isLoop, _uint iAnimationIndex)
{
	if (iAnimationIndex >= m_iNumAnims ||
		iAnimationIndex == m_iCurAnimIndex)
		return S_OK;

	m_Animations[m_iCurAnimIndex]->Reset();

	m_iCurAnimIndex = iAnimationIndex;

	m_Animations[m_iCurAnimIndex]->Set_Loop(isLoop);


	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	m_Animations[m_iCurAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta);

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones);
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_int CModel::Get_BoneIndex(const char* pBoneName) const
{
	_uint	iBoneIndex = 0;

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) 
	{	
		if (false == strcmp(pBone->Get_BoneName(), pBoneName))
			return true;

		++iBoneIndex;
		
		return false;
	});

	if (iter == m_Bones.end())
		return -1;

	return iBoneIndex;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, _uint iMeshIndex, const char* pConstantName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, m_PivotMatrix);
}

HRESULT CModel::Bind_MaterialTexture(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTextures[eType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, m_eModelType, m_pAIScene->mMeshes[i], m_PivotMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pMaterials = m_pAIScene->mMaterials[i];

		MESH_MATERIAL	MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString	strTexturePath;

			if (FAILED(pMaterials->GetTexture(aiTextureType(j), 0, &strTexturePath)))
				continue;


			char			szDrive[MAX_PATH] = "";
			char			szDirectory[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char			szTmp[MAX_PATH] = "";
			strcpy_s(szTmp, szDrive);
			strcat_s(szTmp, szDirectory);
			strcat_s(szTmp, szFileName);
			strcat_s(szTmp, szExt);

			_tchar			szFullPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szTmp, strlen(szTmp), szFullPath, MAX_PATH);

			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}


	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int iParentIndex = m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnims = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnims; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(this, m_pAIScene->mAnimations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;


		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(CGameObject* pOwner, void* pArg)
{
	CModel* pInstance = new CModel(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
	{
		Safe_Release(pAnimation);
	}
	m_Animations.clear();

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_Release(Material.pTextures[i]);
		}
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	m_Importer.FreeScene();
}
