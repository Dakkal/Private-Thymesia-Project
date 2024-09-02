#include "Engine_Shader_Defines.hlsl"

/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_TargetMatrix;
matrix			g_RotMatricesX[100], g_RotMatricesY[100], g_RotMatricesZ[100];
texture2D		g_Texture;



struct VS_IN
{
	/* 그리기용 정점. m_pVB */
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;

	/* 상태변환용 정점. m_pVBInstance */
	float4		vRight		 : TEXCOORD1;
	float4		vUp			 : TEXCOORD2;
	float4		vLook		 : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	/* float4 : w값을 반드시 남겨야하는 이유는 w에 뷰스페이스 상의 깊이(z)를 보관하기 위해서다. */
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};


/* 버텍스에 대한 변환작업을 거친다.  */
/* 변환작업 : 정점의 위치에 월드, 뷰, 투영행렬을 곱한다. and 필요한 변환에 대해서 자유롭게 추가해도 상관없다 .*/
/* 버텍스의 구성 정보를 변경한다. */
VS_OUT VS_MAIN( /* 정점 */VS_IN In, uint instanceID : SV_InstanceID)
{
	VS_OUT			Out = (VS_OUT)0;

	float4x4		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float4 vRight = mul(float4(1.f, 0.f, 0.f, 0.f), g_RotMatricesX[instanceID]);
    float4 vUp = mul(float4(0.f, 1.f, 0.f, 0.f), g_RotMatricesX[instanceID]);
    float4 vLook = mul(float4(0.f, 0.f, 1.f, 0.f), g_RotMatricesX[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesY[instanceID]);
    vUp = mul(vUp, g_RotMatricesY[instanceID]);
    vLook = mul(vLook, g_RotMatricesY[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesZ[instanceID]);
    vUp = mul(vUp, g_RotMatricesZ[instanceID]);
    vLook = mul(vLook, g_RotMatricesZ[instanceID]);
	
    float4x4 TargetMatrix = g_TargetMatrix;
    TargetMatrix[0].xyz = normalize(g_TargetMatrix[0].xyz);
    TargetMatrix[1].xyz = normalize(g_TargetMatrix[1].xyz);
    TargetMatrix[2].xyz = normalize(g_TargetMatrix[2].xyz);
    TargetMatrix[3].xyzw = (float4)0.f;
	
    vRight = mul(vRight, TargetMatrix);
    vUp = mul(vUp, TargetMatrix);
    vLook = mul(vLook, TargetMatrix);
	
    float4 vPos = g_WorldMatrix._41_42_43_44;
	
    float4x4 newWorld = float4x4(vRight, vUp, vLook, vPos);
	
	matrix			matWV, matWVP;

    matWV = mul(newWorld, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	Out.vTexcoord = In.vTexcoord;

	return Out;	
}

VS_OUT VS_MAIN_BillBoard( /* 정점 */VS_IN In, uint instanceID : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float4 vRight = mul(g_WorldMatrix[0].xyzw, g_RotMatricesX[instanceID]);
    float4 vUp = mul(g_WorldMatrix[1].xyzw, g_RotMatricesX[instanceID]);
    float4 vLook = mul(g_WorldMatrix[2].xyzw, g_RotMatricesX[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesY[instanceID]);
    vUp = mul(vUp, g_RotMatricesY[instanceID]);
    vLook = mul(vLook, g_RotMatricesY[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesZ[instanceID]);
    vUp = mul(vUp, g_RotMatricesZ[instanceID]);
    vLook = mul(vLook, g_RotMatricesZ[instanceID]);
	
    float4 vPos = g_WorldMatrix._41_42_43_44;
	
    float4x4 newWorld = float4x4(vRight, vUp, vLook, vPos);
	
	/* mul : 모든(곱하기가 가능한) 행렬의 곱하기를 수행한다. */
    matrix matWV, matWVP;

    matWV = mul(newWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* w나누기 연산. 진정한 투영변환. */
/* 뷰포트스페이스(윈도우좌표)로 위치를 변환한다. */
/* 래스터라이즈 : 정점에 둘러쌓인 픽셀의 정보를 생성한다. */
/* 픽셀정보는 정점정보에 기반한다. */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

/* 받아온 픽셀의 정보를 바탕으로 하여 화면에 그려질 픽셀의 최종적인 색을 결정하낟. */
struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

/* 전달받은 픽셀의 정보를 이용하여 픽셀의 최종적인 색을 결정하자. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);

   // 쉐이더에서 텍스처와 알파 채널을 읽어옴
    float alphaValue = Out.vColor.r;
    Out.vColor.a = alphaValue;
	
	
    if (alphaValue < 0.2f)
        discard;
	
	
    Out.vColor.rgb = float3(1.f, 0.7f, 0.3f);

	return Out;
}

technique11 DefaultTechnique
{
	/* */
	pass Particle
	{
        SetRasterizerState(RS_Sky);
		SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Particle_BillBord
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN_BillBoard();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}




