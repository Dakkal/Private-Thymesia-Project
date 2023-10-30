#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTransform;
class CStateMachine;
class CNavigation;
class CRenderer;
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	enum class PARTS { BODY, WEAPON_R, WEAPON_L, _END };

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);

	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	 CGameObject* Get_Parts(CPlayer::PARTS ePart) { return m_Parts[(_uint)ePart]; }

private:
	vector<class CGameObject*>		m_Parts;

private: 
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CStateMachine*			m_pStateMachineCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PlayerParts();
	HRESULT Ready_State();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override; 
	virtual void Free() override;
};

END
