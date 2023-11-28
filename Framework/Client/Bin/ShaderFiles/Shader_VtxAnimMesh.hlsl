#include "Engine_Shader_Defines.hlsl"

/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix			g_BoneMatrices[256];

Texture2D		g_DiffuseTexture;
Texture2D		g_NormalTexture;

texture2D       g_DissolveTexture;
float           g_Time;
float           g_DissolveDuration;


/* 버텍스 쉐이더 */
struct VS_IN
{	
	float3		vPosition : POSITION;
    float3		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0;
    float3      vTangent :  TANGENT;
	
    uint4		vBlendIndices : BLENDINDEX;
    float4		vBlendWeights : BLENDWEIGHT;
};


struct VS_OUT
{
    float4		vPosition : SV_POSITION;
    float4		vNormal   : NORMAL;
    float3      vTangent  : TANGENT;
    float3      vBiTangent : BINORMAL;
    
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
    float4      vProjPos  : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

    float fW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fW;
	
    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    vector vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix);
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vBiTangent = normalize(cross(Out.vNormal.xyz, Out.vTangent));
    
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
  
	return Out;	
}

/* 픽셀 쉐이더 */
struct PS_IN
{
	float4		vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
    float3      vTangent : TANGENT;
    float3      vBiTangent : BINORMAL;
    
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
    float4      vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT	PS_MAIN_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_ALPHA_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));
    
  

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_NONALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_NONALPHA_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fDissolveAlpha = saturate(-g_Time / g_DissolveDuration + vMtrlDissolve.r);
 
    if (fDissolveAlpha < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));
    
    float fDissolveAlpha = saturate(-g_Time / g_DissolveDuration + vMtrlDissolve.r);
 
    if (fDissolveAlpha < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
      
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    float Depth_Color = In.vProjPos.w / 1000.0f;
    
    Out.vLightDepth = vector(Depth_Color, Depth_Color * Depth_Color, Depth_Color, 1.f);

    return Out;
}


technique11 DefaultTechnique
{
    pass Mesh_Alpha
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

    pass Mesh_Alpha_Normal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA_NORMAL();
    }

    pass Mesh_NonAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONALPHA();
    }

    pass Mesh_NonAlpha_Normal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONALPHA_NORMAL();
    }

    pass Mesh_Alpha_NonCull
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }

    pass Mesh_Alpha_NonCull_Normal
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA_NORMAL();
    }

    pass Mesh_Alpha_NonCull_Dissolve
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass Mesh_Alpha_NonCull_Dissolve_Normal
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE_NORMAL();
    }

    pass Mesh_Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

}




