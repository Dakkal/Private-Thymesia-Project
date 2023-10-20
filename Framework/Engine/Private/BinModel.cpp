#include "../Public/BinModel.h"
#include "BinAnimation.h"
#include "BinBone.h"
#include "BinMesh.h"
#include "BinChannel.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"

CBinModel::CBinModel(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext)
	: CComponent(pDeivce, pContext)
{
}

CBinModel::CBinModel(CGameObject* pOwner, const CBinModel& rhs)
	:CComponent(pOwner, rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iNumAnims(rhs.m_iNumAnims)
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

HRESULT CBinModel::Initialize_Prototype(TYPE eModelType, const wstring& pModelFilePath, _matrix PivotMatrix)
{
	m_eModelType = eModelType;

	m_PivotMatrix = PivotMatrix;

	class CFbxExporter FbxExporter;


	if (CBinModel::TYPE_NONANIM == m_eModelType)
 		FbxExporter.Start_Static_Import(pModelFilePath);
	else
		FbxExporter.Start_Dynamic_Import(pModelFilePath);

	if (CBinModel::TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Ready_Bones(FbxExporter.Get_Bone())))
			return E_FAIL;
	}
	

	if (FAILED(Ready_Meshes(FbxExporter.Get_Mesh())))
		return E_FAIL;

	if (FAILED(Ready_Materials(FbxExporter.Get_Material(), pModelFilePath)))
		return E_FAIL;

	if (CBinModel::TYPE_ANIM == m_eModelType)
	{
		if (FAILED(Ready_Animations(FbxExporter.Get_Anim())))
			return E_FAIL;
	}
	
}

HRESULT CBinModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CBinModel::First_Set_Animation(_bool isLoop, _int iAnimationIndex)
{
	if (iAnimationIndex >= m_iNumAnims ||
		iAnimationIndex == m_iCurAnimIndex)
		return S_OK;

	m_iCurAnimIndex = iAnimationIndex;

	m_Animations[m_iCurAnimIndex]->Reset();
	m_Animations[m_iCurAnimIndex]->Set_Loop(isLoop);

	return S_OK;

}

HRESULT CBinModel::Set_Animation(_bool isLoop, _int iAnimationIndex)
{
	if (iAnimationIndex >= m_iNumAnims ||
		iAnimationIndex == m_iCurAnimIndex)
		return S_OK;

	m_bIsAnimChange = true;

	m_iNextAnimIndex = iAnimationIndex;
	m_bIsNextAnimLoop = isLoop;
	m_fChangeTrackPosition = 0.f;

	CurChannels = m_Animations[m_iCurAnimIndex]->Get_Channels();
	NextChannels = m_Animations[m_iNextAnimIndex]->Get_Channels();

	return S_OK;
}

HRESULT CBinModel::Change_Animation(_float fDuration, _float fTimeDelta)
{
	m_fChangeTrackPosition += fTimeDelta;

	_vector	vScale;
	_vector vRotation;
	_vector vTranslation;

	for (auto& pCurChannel : CurChannels)
	{
		for (auto& pNextChannel : NextChannels)
		{
			if (pCurChannel->Get_Channel_BoneIndex() == pNextChannel->Get_Channel_BoneIndex())
			{
				KEYFRAME CurKeyframe = pCurChannel->Get_CurKeyFrame();
				KEYFRAME NextKeyframe = pNextChannel->Get_KeyFrames().front();

				while (m_fChangeTrackPosition >= fDuration)
				{
					m_bIsAnimChange = false;
					m_iCurAnimIndex = m_iNextAnimIndex;
					m_Animations[m_iCurAnimIndex]->Reset();
					m_Animations[m_iCurAnimIndex]->Set_Loop(m_bIsNextAnimLoop);
					return S_OK;
				}

				_float	fRatio = (m_fChangeTrackPosition - 0.f) / fDuration;

				_vector vSourScale = _vector(CurKeyframe.vScale);
				_vector vDestScale = _vector(NextKeyframe.vScale);
				vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);

				_vector vSourRotation = CurKeyframe.vRotation;
				_vector vDestRotation = NextKeyframe.vRotation;
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);

				_vector vSourTranslation = CurKeyframe.vTranslation;
				_vector vDestTranslation = NextKeyframe.vTranslation;
				vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

				_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, _vector(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

				m_Bones[pCurChannel->Get_Channel_BoneIndex()]->Set_Transform(TransformationMatrix);
			}
		}
	}




	return S_OK;
}

HRESULT CBinModel::Play_Animation(_float fTimeDelta)
{
	if (true == m_bIsAnimChange)
	{
		Change_Animation(0.2, fTimeDelta);
	}
	if (false == m_bIsAnimChange)
	{
		m_Animations[m_iCurAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta);
	}
	
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones);
	}
	
	auto rootBone = Get_BonePtr("root");

	if (true == m_Animations[m_iCurAnimIndex]->IsFinished())
	{
		m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
		m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };
	}

	m_PrevRootPos = m_CurRootPos;
	m_CurRootPos = rootBone->Get_RootPos(m_PivotMatrix);

	return S_OK;
}

HRESULT CBinModel::Bind_BoneMatrices(CShader* pShader, _uint iMeshIndex, const char* pConstantName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, m_PivotMatrix);
}

HRESULT CBinModel::Bind_MaterialTexture(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTextures[eType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CBinModel::Render(_uint iMeshIndex)
{
	if(true == m_Meshes[iMeshIndex]->Get_RenderState())
		m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CBinModel::Set_Model_WireFrame(_uint iMeshIndex, _bool eWireFrame)
{
	m_Meshes[iMeshIndex]->Set_RasterState(eWireFrame);

	return S_OK;
}

_int CBinModel::Get_BoneIndex(const string& strBoneName) const
{
	_uint	iBoneIndex = 0;

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBinBone* pBone) {

		if (pBone->Get_BoneName() == strBoneName)
			return true;

		++iBoneIndex;

		return false;
		});

	if (iter == m_Bones.end())
		return -1;

	return iBoneIndex;
}

void CBinModel::Set_OwnerPosToRootPos(CTransform* pTransform, _float fTimeDelta)
{
	if (true == m_bIsAnimChange)
		return;

	_vector vPos = pTransform->Get_State(CTransform::STATE_POS);
	_vector vDir = m_PrevRootPos - m_CurRootPos;
	_vector vWorldDir = XMVector3TransformNormal(vDir, pTransform->Get_WorldMatrix());
	vWorldDir.y *= -1;
	vWorldDir.Normalize();

	_float fDist = vDir.Length() * 0.65f;

	vPos += vWorldDir * fDist * fTimeDelta;
	pTransform->Set_State(CTransform::STATE_POS, vPos);
}

CBinBone* CBinModel::Get_BonePtr(const string& pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBinBone* pBone)
		{
			if (pBone->Get_BoneName() == pBoneName)
				return true;

			return false;
		});

	return *iter;
}

HRESULT CBinModel::Ready_Meshes(const SAVE_MESH tMehsDesc)
{
	m_iNumMeshes = tMehsDesc.iNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		if (tMehsDesc.vecMeshStaticInfo.size() > tMehsDesc.vecMeshDynamicInfo.size())
		{
			CBinMesh* pMesh = CBinMesh::Create(m_pDevice, m_pContext, m_eModelType, tMehsDesc.vecMeshStaticInfo[i], m_PivotMatrix);
			if (nullptr == pMesh)
				return E_FAIL;

			m_Meshes.push_back(pMesh);
		}
		else
		{
			CBinMesh* pMesh = CBinMesh::Create(m_pDevice, m_pContext, m_eModelType, tMehsDesc.vecMeshDynamicInfo[i], m_PivotMatrix);
			if (nullptr == pMesh)
				return E_FAIL;

			m_Meshes.push_back(pMesh);
		}
	}

	return S_OK;
}

HRESULT CBinModel::Ready_Materials(const SAVE_MATERIAL strTexture, const wstring& pModelFilePath)
{
	m_iNumMaterials = strTexture.iNumMaterial;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL	MeshMaterial;
		ZeroMemory(&MeshMaterial, sizeof MeshMaterial);

		SAVE_MATERIAL_INFO tMaterialInfo = strTexture.vecMaterialPaths[i];

		for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			wstring	strTexturePath;

			if ("" == tMaterialInfo.FilePaths[j])
				continue;

			strTexturePath.assign(tMaterialInfo.FilePaths[j].begin(), tMaterialInfo.FilePaths[j].end());

			MeshMaterial.pTextures[j] = CTexture::Create(m_pDevice, m_pContext, strTexturePath);
			if (nullptr == MeshMaterial.pTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}
}

HRESULT CBinModel::Ready_Bones(const SAVE_BONE tBoneDesc)
{
	for (size_t i = 0; i < tBoneDesc.Bones.size(); i++)
	{
		CBinBone* pBone = CBinBone::Create(tBoneDesc.Bones[i]);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}
}

HRESULT CBinModel::Ready_Animations(const SAVE_ANIM tAnimDesc)
{
	m_iNumAnims = tAnimDesc.iNumAnim;

	for (size_t i = 0; i < m_iNumAnims; i++)
	{
		CBinAnimation* pAnimation = CBinAnimation::Create(this, tAnimDesc.vecAnim[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CBinModel* CBinModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const wstring& pModelFilePath, _matrix PivotMatrix)
{
	CBinModel* pInstance = new CBinModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBinModel::Clone(CGameObject* pOwner, void* pArg)
{
	CBinModel* pInstance = new CBinModel(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBinModel::Free()
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
}
