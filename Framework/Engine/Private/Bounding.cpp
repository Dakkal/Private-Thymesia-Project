#include "Bounding.h"

CBounding::CBounding()
{
}

HRESULT CBounding::Initialize(const BOUNDING_DESC* pDesc)
{
	return S_OK;
}

void CBounding::Free()
{
	__super::Free();
}
