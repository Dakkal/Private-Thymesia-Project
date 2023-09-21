#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORM_STATE{ D3DTS_VIEW, D3DTS_PROJ, D3DTS_END	};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORM_STATE eState, _matrix TransformMatrix);
	_matrix Get_Transform_Matrix(TRANSFORM_STATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(TRANSFORM_STATE eState) const;
	_vector Get_CamPosition_Vector() const;

public:
	HRESULT Bind_TransformToShader(class CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState);
	HRESULT Bind_CamPosToShader(class CShader* pShader, const char* pConstantName);

public:
	HRESULT Initialize();
	void Tick();

private:
	_matrix			m_TransformMatrices[D3DTS_END];
	_matrix			m_TransformMatrices_Inverse[D3DTS_END];
	_vector			m_vCamPosition = { 0.f, 0.f, 0.f, 1.f };

public:
	virtual void Free() override;
};

END