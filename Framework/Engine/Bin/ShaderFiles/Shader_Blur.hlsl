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
texture2D g_BlurTarget;

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


VS_OUT VS_MAIN( /* Á¤Á¡ */VS_IN In)
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

float g_fTexelWidth = 1.f / 1280.f;
float g_fTexelHeight = 1.f / 720.f;

int   g_iGaussian_Radius = 4;
float g_BlurWeight10[21] =
{
     0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
     0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
     0.14107424,
     0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
     0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
};
float g_BlurWeight4[9] = { 0.0135f, 0.0476f, 0.1172f, 0.2011f, 0.2408f, 0.2011f, 0.1172f, 0.0476f, 0.0135f };

float4 PS_MAIN_BLUR(VS_OUT In, uniform bool bHorizontal) : SV_Target
{
    float4 vValue = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int Index = -g_iGaussian_Radius; Index <= g_iGaussian_Radius; Index++)
    {
        if (bHorizontal)
        {
            vValue += g_BlurTarget.Sample(LinearBorderSampler, 
            float2(In.vTexcoord.x + Index * g_fTexelWidth, In.vTexcoord.y)) * g_BlurWeight4[Index + g_iGaussian_Radius];
        }
        else
        {
            vValue += g_BlurTarget.Sample(LinearBorderSampler, 
            float2(In.vTexcoord.x, In.vTexcoord.y + Index * g_fTexelHeight)) * g_BlurWeight4[Index + g_iGaussian_Radius];
        }
    }

    return vValue;
}

technique11 DefaultTechnique
{
    pass BlurH 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR(true);
    }

    pass BlurV 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR(false);
    }
}





