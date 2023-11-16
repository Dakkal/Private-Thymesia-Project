#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CCollider;

union COLLIDER_ID
{
	struct
	{
		_uint iLeft_id;
		_uint iRight_id;
	};

	_ulonglong ID;
};

class CCollideManager final : public CBase
{
	DECLARE_SINGLETON(CCollideManager)

private:
	CCollideManager();
	virtual ~CCollideManager() = default;

public:
	void	Check_Collision(const _uint iLevel, const LAYER_TAG& _eType1, const LAYER_TAG& _eType2, _float fTimedelta);
	void	Check_Part_Collision(CGameObject* _pObj1, CGameObject* _pObj2, CCollider* _pObj1Col, CCollider* _pObj2Col, _float fTimedelta);

private:
	typedef map<_ulonglong, _bool> CHECKCOLLI;
	typedef map<_ulonglong, _bool>::iterator CHECKITER;

private:
	void	Set_Info(CHECKITER& _iter, CCollider* _pCol1, CCollider* _pCol2);

private:
	CHECKCOLLI			m_mapColInfo;

public:
	virtual void Free() override;
};

END