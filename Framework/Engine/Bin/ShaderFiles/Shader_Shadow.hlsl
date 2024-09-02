#include "Engine_Shader_Defines.hlsl"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;


vector g_vCamPosition;
vector g_vLightDir;
vector g_vLightPos;
vector g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_DepthTexture;

texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;
texture2D g_BlurTexture;
texture2D g_ShadowTexture;

texture2D g_Texture;



struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN( /* 정점 */VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    return Out;
}

PS_OUT PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vWorldPos;
	/* 투영스페이스 상의 위치를 구한다. */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구한다. */
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    /* 월드상의 위치 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
	/* 광원기준의 뷰랑 투영행렬을 곱한다. */
    vector vPosition = mul(vWorldPos, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
    
    float2 vUV;
    vUV.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vUV.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vLightDepthDesc = g_LightDepthTexture.Sample(PointSampler, vUV);
    
    float fragDepth = vPosition.w;
    float LightDepth = (vLightDepthDesc.x * 1000.f);
    
    float2 Moments = ComputeMoments(LightDepth);
    float fShadow = ChebyshevUpperBound(Moments, fragDepth);
    fShadow = saturate(fShadow + 0.5f);
  
    Out.vColor = fShadow;
    
    return Out;
}



technique11 DefaultTechnique
{
    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

	
}




