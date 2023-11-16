#pragma once
#include "GameObject.h"
#include "Bounding_Frustrum.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		CGameObject*		pOwner = { nullptr };
		PARTS				ePart = { PARTS::_END };
		class CTransform*	pParentTransform = { nullptr };
		class CBinBone*		pSocketBone = { nullptr };
		_matrix				SocketPivot;

		CBounding_Frustrum::BOUNDING_FRUSTRUM_DESC FrustrumDesc;

	}PART_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);

	virtual ~CPartObject() = default;

public:
	class CBinBone* Get_SocketBonePtr(const char* pBoneName);
	_matrix			Get_SocketPivotMatrix();
	const PARTS&	Get_Part_Index() { return m_ePart; }
	const _int&		Get_AnimationIndex();
	_uint			Get_CurKeyFrameNumb();
	CGameObject*	Get_PartOwner() { return m_pOwner; }

public:
	_bool			Is_AnimCurKeyFrame(_uint iIndex);
	_bool			Is_AnimOverKeyFrame(_uint iIndex);
	_bool			IsAnimationEnd();
	const _bool&	IsAnimChange();
	void			Set_AnimationIndex(_bool isLoop, _uint iAnimIndex, _float fAnimSpeed, _bool bWantReset = false, _uint iStartKeyFrame = 0, _float fChangeDuration = 0.2f);
	void			Set_First_AnimationIndex(_bool isLoop, _uint iAnimIndex, _float fAnimSpeed);
	void			Set_Anim_TargetPos(_vector vTargetPos) { m_vTargetPos = vTargetPos; }

public:
	virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_float fTimeDelta);
	virtual void	LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CGameObject*			m_pOwner = { nullptr };
	PARTS					m_ePart = { PARTS::_END };
	class CTransform*		m_pParentTransform = { nullptr };
	class CBinBone*			m_pSocketBone = { nullptr };
	_matrix					m_SocketPivotMatrix;
	_matrix					m_WorldMatrix;
	_vector					m_vTargetPos = { XMVectorZero() };

protected:
	class CRenderer*	m_pRendererCom = { nullptr };
	class CTransform*	m_pTransformCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };
	class CBinModel*	m_pModelCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };

protected:
	HRESULT Compute_RenderMatrix(_matrix ChildMatrix);

public:
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END