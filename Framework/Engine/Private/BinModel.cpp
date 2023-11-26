#include "../Public/BinModel.h"
#include "GameInstance.h"
#include "BinAnimation.h"
#include "BinBone.h"
#include "BinMesh.h"
#include "BinChannel.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "Navigation.h"
#include "LandObject.h"
#include "PartObject.h"

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
	
	return S_OK;
}

HRESULT CBinModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CBinModel::First_Set_Animation(_bool isLoop, _uint iAnimationIndex, _float fAnimSpeed)
{
	if (iAnimationIndex >= m_iNumAnims ||
		iAnimationIndex == m_iCurAnimIndex)
		return S_OK;

	m_iCurAnimIndex = iAnimationIndex;

	m_Animations[m_iCurAnimIndex]->Reset();
	m_Animations[m_iCurAnimIndex]->Set_Loop(isLoop);
	m_Animations[m_iCurAnimIndex]->Set_AnimSpeed(fAnimSpeed);

	return S_OK;
}

HRESULT CBinModel::Set_Animation(_bool isLoop, _uint iAnimationIndex, _float fAnimSpeed, _bool bWantReset, _uint iStartNumKeyFrames, _float fRooAnimDist, _float fChangeDuration)
{
	if (false == m_bIsFirstAnim)
	{
		m_iCurAnimIndex = iAnimationIndex;

		m_Animations[m_iCurAnimIndex]->Reset();
		m_Animations[m_iCurAnimIndex]->Set_Loop(isLoop);
		m_Animations[m_iCurAnimIndex]->Set_AnimSpeed(fAnimSpeed);
		m_bIsFirstAnim = true;

		return S_OK;
	}

	if (true == bWantReset && iAnimationIndex == m_iCurAnimIndex)
	{
		m_bWanttoReset = bWantReset;

		m_bIsAnimChange = true;;
		
		m_bIsNextAnimLoop = isLoop;
		m_iNextStartNumKeyFrames = iStartNumKeyFrames;

		m_fChangeTrackPosition = 0.f;
		m_fChangeDuration = fChangeDuration;

		m_Animations[m_iCurAnimIndex]->Set_AnimSpeed(fAnimSpeed);

		CurChannels = m_Animations[m_iCurAnimIndex]->Get_Channels();

		m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
		m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

		return S_OK;
	}
	else
	{
		m_bWanttoReset = false;
	}

	if (true == m_bIsAnimChange && iAnimationIndex == m_iCurAnimIndex)
	{
		m_bIsAnimChange = false;

		return S_OK;
	}

	if (iAnimationIndex >= m_iNumAnims ||
		iAnimationIndex == m_iCurAnimIndex)
			return S_OK;
		
	m_bIsAnimChange = true;

	m_iNextAnimIndex = iAnimationIndex;
	m_bIsNextAnimLoop = isLoop;
	m_iNextStartNumKeyFrames = iStartNumKeyFrames;
	m_fChangeTrackPosition = 0.f;
	m_fChangeDuration = fChangeDuration;

	CurChannels = m_Animations[m_iCurAnimIndex]->Get_Channels();
	NextChannels = m_Animations[m_iNextAnimIndex]->Get_Channels();

	m_Animations[m_iNextAnimIndex]->Reset();
	m_Animations[m_iNextAnimIndex]->Set_AnimSpeed(fAnimSpeed);

	m_fRooAnimDist = fRooAnimDist;

	return S_OK;
}

HRESULT CBinModel::Change_Animation(_float fDuration, _float fTimeDelta)
{
	if (0.f == m_fChangeTrackPosition && false == m_bWanttoReset)
	{
		m_Animations[m_iNextAnimIndex]->Set_StartKeyFrames(m_iNextStartNumKeyFrames, fTimeDelta);
	}

	m_fChangeTrackPosition += fTimeDelta;

	_vector	vScale;
	_vector vRotation;
	_vector vTranslation;

	for (size_t i = 0; i < CurChannels.size(); i++)
	{
		KEYFRAME CurKeyframe = CurChannels[i]->Get_CurKeyFrame();
		KEYFRAME NextKeyframe;
		if (true == m_bWanttoReset)
		{
			NextKeyframe = CurChannels[i]->Get_KeyFrames().front();
		}
		else
		{
			NextKeyframe = NextChannels[i]->Get_CurKeyFrame();
		}
		

		while (m_fChangeTrackPosition >= fDuration)
		{
			m_bIsAnimChange = false;

			if(false == m_bWanttoReset)
				m_iCurAnimIndex = m_iNextAnimIndex;

			m_Animations[m_iCurAnimIndex]->Reset();
			m_Animations[m_iCurAnimIndex]->Set_Loop(m_bIsNextAnimLoop);
			m_Animations[m_iCurAnimIndex]->Set_StartKeyFrames(m_iNextStartNumKeyFrames, fTimeDelta);

			m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
			m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

			return S_OK;
		}

		_float	fRatio = (m_fChangeTrackPosition - 0.f) / fDuration;
		if (1.f <= fRatio)
			fRatio = 1.f;

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

		m_Bones[CurChannels[i]->Get_Channel_BoneIndex()]->Set_Transform(TransformationMatrix);
	}
	return S_OK;
}

HRESULT CBinModel::Play_Animation(_float fTimeDelta)
{
	if (true == m_bIsAnimChange)
	{
		Change_Animation(m_fChangeDuration, fTimeDelta);
	}
	if (false == m_bIsAnimChange)
	{
		m_Animations[m_iCurAnimIndex]->Update_TransformationMatrix(&m_Bones, fTimeDelta);
	}
	
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones);
	}
	
	auto rootBone = Get_BonePtr("root");

	if (m_Animations[m_iCurAnimIndex]->IsLoop() && m_Animations[m_iCurAnimIndex]->IsFinished())
	{
		m_PrevRootPos = { 0.f, 0.f, 0.f, 1.f };
		m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };
	}

	m_PrevRootPos = m_CurRootPos;
	m_CurRootPos = rootBone->Get_RootPos();
	if (0 == m_PrevRootPos.z)
		m_PrevRootPos = m_CurRootPos;
	
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

	if (nullptr == m_Materials[iMaterialIndex].pTextures[eType])
		return E_FAIL;

	return m_Materials[iMaterialIndex].pTextures[eType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

const string& CBinModel::Get_MaterialName(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return string();

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	if (iMaterialIndex >= m_iNumMaterials)
		return string();

	return m_Materials[iMaterialIndex].strMaterialName;
}

HRESULT CBinModel::Render(_uint iMeshIndex)
{
	if(true == m_Meshes[iMeshIndex]->Get_RenderState())
		m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_uint CBinModel::Get_CurKeyFrameNumb()
{
	_uint iNumb = 0;

	auto& KeyFrames = m_Animations[m_iCurAnimIndex]->Get_CurKeyFrames();

	for (auto& KeyFrame : KeyFrames)
	{
		if (iNumb < KeyFrame)
			iNumb = KeyFrame;
	}

	return iNumb;
}

_bool CBinModel::Is_CurAnimKeyFrame(_uint iIndex)
{
	/* 애니메이션 전환중에는 확인할 필요X */
	if (true == m_bIsAnimChange)
		return false;

	auto& KeyFrames = m_Animations[m_iCurAnimIndex]->Get_CurKeyFrames();

	_uint iMax = 0;

	for (auto& KeyFrame : KeyFrames)
	{
		if (iMax < KeyFrame)
		{
			iMax = KeyFrame;
		}
	}

	if (iMax == iIndex)
		return true;
	else
		return false;
}

_bool CBinModel::Is_OverAnimKeyFrame(_uint iIndex)
{
	/* 애니메이션 전환중에는 확인할 필요X */
	if (true == m_bIsAnimChange)
		return false;

	auto& KeyFrames = m_Animations[m_iCurAnimIndex]->Get_CurKeyFrames();

	_uint iMax = 0;

	for (auto& KeyFrame : KeyFrames)
	{
		if (iMax < KeyFrame)
		{
			iMax = KeyFrame;
		}
	}

	if (iMax < iIndex)
		return false;
	else
		return true;
}

_bool CBinModel::Is_CurAnimFinished()
{
	if (true == m_bIsAnimChange)
		return false;

	return m_Animations[m_iCurAnimIndex]->IsFinished(); 
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

HRESULT CBinModel::Set_OwnerPosToRootPos(CTransform* pTransform, _float fTimeDelta, class CNavigation* pNavi, _vector TargetPos)
{
	if (nullptr == pTransform)
		return E_FAIL;

	if (true == m_bIsAnimChange || 0.f == m_CurRootPos.z)
		return S_OK;

	_vector vPos = pTransform->Get_State(CTransform::STATE_POS);
	_vector vDir = m_PrevRootPos - m_CurRootPos;
	_vector vWorldDir = XMVector3TransformNormal(vDir, pTransform->Get_WorldMatrix());
	vWorldDir.y *= -1;
	vWorldDir.Normalize();

	_float fDist = vDir.Length() * m_fRooAnimDist;

	vPos += vWorldDir * fDist * fTimeDelta;

	if (1 == TargetPos.w)
	{
		_float fTargetDist = _vector(TargetPos - vPos).Length();
		if (fTargetDist <= FLT_EPSILON)
		{
			vPos = pTransform->Get_State(CTransform::STATE_POS);

			vWorldDir.z = 0.f;
			vPos += vWorldDir * fDist * fTimeDelta;
			vPos.w = 1.f;
		}
	}

	if (nullptr != pNavi)
	{
		_int iMove = pNavi->IsMove(vPos);

		if (0 == iMove)
		{
			pTransform->Set_State(CTransform::STATE_POS, vPos);
		}
		else if (-2 == iMove)
		{
			_bool bFind;

			if (OBJECT_TYPE::PART == m_pOwner->Get_ObjectType())
			{
				CGameObject* pPartOwner = dynamic_cast<CPartObject*>(m_pOwner)->Get_PartOwner();
				bFind = dynamic_cast<CLandObject*>(pPartOwner)->Find_NaviMesh(vPos);
			}
			else
			{
				bFind = dynamic_cast<CLandObject*>(m_pOwner)->Find_NaviMesh(vPos);
			}
			
			if (true == bFind)
				pTransform->Set_State(CTransform::STATE_POS, vPos);
		}
	}
	else
		pTransform->Set_State(CTransform::STATE_POS, vPos);

	return S_OK;
}

CBinBone* CBinModel::Get_BonePtr(const string& pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBinBone* pBone)
		{
			if (pBone->Get_BoneName() == pBoneName)
				return true;

			return false;
		});

	if (m_Bones.end() == iter)
		return nullptr;

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

		SAVE_MATERIAL_INFO tMaterialInfo = strTexture.vecMaterialPaths[i];

		MeshMaterial.strMaterialName = tMaterialInfo.strMaterialName;

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

	return S_OK;
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

	return S_OK;
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
