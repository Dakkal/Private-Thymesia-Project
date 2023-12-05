#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBinModel;
class CShader;
class CRenderer;
class CTransform;
class CCollider;
END

BEGIN(Client)

class CLeft_BossDoor final : public CGameObject
{
protected:
	CLeft_BossDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLeft_BossDoor(const CLeft_BossDoor& rhs);
	virtual ~CLeft_BossDoor() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() override;


private:
	_bool		m_bPlayOnce = { false };

	CCollider* m_pColliderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CBinModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float3		m_vAngle = { 0.f, 0.f, 0.f };


public:
	static CLeft_BossDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END