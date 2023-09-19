#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	CGameObject* Get_FirstObject();

private:
	list<class CGameObject*>		m_listGameObject;

public:
	static CLayer* Create();
	virtual void Free() override;

};

END