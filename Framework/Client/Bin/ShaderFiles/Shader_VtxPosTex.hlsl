#include "Engine_Shader_Defines.hlsl"

/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D		g_Texture;
Texture2D		g_Textures[2];

float			g_TimeDelta;
float           g_Alpha;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;	
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    vector vSourColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (vSourColor.a <= 0.1f)
        discard;
	
    Out.vColor = vSourColor;

	return Out;
}

PS_OUT PS_MAIN_LERP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (vSourColor.a <= 0.2f)
        discard;
	
    float lerpAlpha = lerp(0.f, 1.f, g_TimeDelta);
	
    if (lerpAlpha <= vSourColor.a)
    {
        vSourColor.a = lerpAlpha;
    }
    
    Out.vColor = vSourColor;

    return Out;
}

PS_OUT PS_MAIN_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (vSourColor.a <= 0.2f)
        discard;
	
    vSourColor.a = g_Alpha;
	
    Out.vColor = vSourColor;

    return Out;
}

PS_OUT PS_MAIN_EDIT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = (0.f, 0.f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
	pass UI
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Edit
    {
        SetRasterizerState(RS_Edit);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EDIT();
    }

    pass UI_Lerp
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LERP();
    }

    pass UI_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }
}




