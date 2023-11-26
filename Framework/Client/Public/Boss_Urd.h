#pragma once
#include "Client_Defines.h"
#include "LandObject.h"
#include "PartObject.h"
#include "Throw_Weapon_Urd.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CStateMachine;
class CCollider;
END

BEGIN(Client)

class CBoss_Urd final : public CLandObject
{
protected:
	CBoss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Urd(const CBoss_Urd& rhs);

	virtual ~CBoss_Urd() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Enter_Object();
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta) override;

public:
	list<CThrow_Weapon_Urd*>*		Get_Projectiles()			  { return &m_listProjectile; }

	CTransform*						Get_PlayerTransform()	const { return m_pPlayerTransform; }
	const _bool&					Is_LookPlayer()			const { return m_bIsLookPlayer; }
	void							Set_LookPlayer(_bool bIsLook) { m_bIsLookPlayer = bIsLook; }
	_float							Get_PlayerDistance();

public:
	void	Out_Player(_float fTimeDelta);
	void	Look_Player(_float fTimeDelta);
	void	Set_PlayerTransform();

public:
	void			Set_Seq1_State(_bool IsSeq1)	{ m_IsSeq1 = IsSeq1; }
	const _bool&	Is_Seq1() const					{ return m_IsSeq1; }

	void			Set_Excute(_bool IsExcute)		{ m_IsExcute = IsExcute; }
	const _bool&	Is_Excute() const				{ return m_IsExcute; }

	void			Add_SkillCnt()					{ ++m_iSkillCnt; }
	void			Reset_SkillCnt()				{ m_iSkillCnt = 0; }
	const _uint&	Get_SkillCnt()					{ return m_iSkillCnt; }

	void			Set_SkillActive(_uint iSkillActive) { m_iSkillActive = iSkillActive; }
	const _uint&	Get_SkillActive()					{ return m_iSkillActive; }

private:
	list<CThrow_Weapon_Urd*>		m_listProjectile;


private:
	CTransform*				m_pPlayerTransform = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CStateMachine*			m_pStateMachineCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	_int					iIndex = 0;

	_float					m_fReleaseTimeAcc = { 0.f };
	_bool					m_bIsOutPlayer = { false };
	_bool					m_bIsLookPlayer = { false };

	_uint					m_iSkillCnt = { 0 };
	_uint					m_iSkillActive = { 0 };

	_bool					m_bFirstDrop = { true };
	_bool					m_IsSeq = { false };
	_bool					m_IsSeq1 = { true };
	_bool					m_IsExcute = { false };
	

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Ready_State();

public:
	static CBoss_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
