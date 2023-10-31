#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBinModel;
class CShader;
class CRenderer;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CSkyDome final : public CGameObject
{
private:
	CSkyDome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyDome(const CSkyDome& rhs);
	virtual ~CSkyDome() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CBinModel* m_pModelCom = { nullptr };


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkyDome* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END