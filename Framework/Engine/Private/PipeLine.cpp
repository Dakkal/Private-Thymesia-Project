#include "PipeLine.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORM_STATE eState, _matrix TransformMatrix)
{
	if (eState >= D3DTS_END || eState < D3DTS_VIEW)
		return;

	m_TransformMatrices[eState] = TransformMatrix;
}

_matrix CPipeLine::Get_Transform_Matrix(TRANSFORM_STATE eState) const
{
	if (eState >= D3DTS_END || eState < D3DTS_VIEW)
		return XMMatrixIdentity();

	return m_TransformMatrices[eState];
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(TRANSFORM_STATE eState) const
{
	if (eState >= D3DTS_END || eState < D3DTS_VIEW)
		return XMMatrixIdentity();

	return m_TransformMatrices_Inverse[eState];
}

_vector CPipeLine::Get_CamPosition_Vector() const
{
	return m_vCamPosition;
}

HRESULT CPipeLine::Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformMatrices[eState]);
}

HRESULT CPipeLine::Bind_CamPosToShader(CShader* pShader, const char* pConstantName)
{
	return pShader->Bind_RawValue(pConstantName, &m_vCamPosition, sizeof(_vector));
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		m_TransformMatrices[i] = XMMatrixIdentity();
		m_TransformMatrices_Inverse[i] = XMMatrixIdentity();
	}

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		m_TransformMatrices_Inverse[i] = m_TransformMatrices[i].Invert();
	}

	memmove(&m_vCamPosition, &m_TransformMatrices_Inverse[D3DTS_VIEW].m[3][0], sizeof(_vector));
}

void CPipeLine::Free()
{
	__super::Free();
}
