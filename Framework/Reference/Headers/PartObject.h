#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartDesc
	{
		CGameObject*	pOwner = { nullptr };
		class CTransform*		pParentTransform = { nullptr };
		class CBinBone*		pSocketBone = { nullptr };
		_matrix			SocketPivot;

	}PART_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);

	virtual ~CPartObject() = default;

public:
	class CBinBone* Get_SocketBonePtr(const char* pBoneName);
	_matrix Get_SocketPivotMatrix();

public:
	_bool Is_AnimCurKeyFrame(_uint iIndex);
	const _int& Get_AnimationIndex();
	const _bool& IsAnimationEnd();
	void Set_AnimationIndex(_bool isLoop, _uint iAnimIndex, _float fAnimSpeed,_uint iStartKeyFrame = 0);

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }

protected:
	CGameObject*			m_pOwner = { nullptr };
	class CTransform*		m_pParentTransform = { nullptr };
	class CBinBone*			m_pSocketBone = { nullptr };
	_matrix					m_SocketPivotMatrix;
	_matrix					m_WorldMatrix;


protected: 
	class CRenderer*	m_pRendererCom = { nullptr };
	class CTransform* m_pTransformCom = { nullptr };
	class CShader*	m_pShaderCom = { nullptr };
	class CBinModel*	m_pModelCom = { nullptr };
	class CCollider*	m_pColliderCom = { nullptr };

protected:
	HRESULT Compute_RenderMatrix(_matrix ChildMatrix);

public:
	static CPartObject* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END