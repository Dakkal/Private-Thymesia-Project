#include "Engine_Shader_Defines.hlsl"

/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D		g_DiffuseTexture;
Texture2D		g_NormalTexture;

texture2D       g_DissolveTexture;
float           g_Time;
float           g_DissolveDuration;

vector          g_vColor = { 0.f, 0.f, 0.f, 0.f };


matrix g_ViewMatrixInv, g_ProjMatrixInv;

vector g_vCamPosition;
vector g_vLightDir;
vector g_vLightPos;
vector g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* 버텍스 쉐이더 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4		vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
    float3      vTangent : TANGENT;
    float3      vBiTangent : BINORMAL;
    
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
    float4      vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord; 
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vBiTangent = normalize(cross(Out.vNormal.xyz, Out.vTangent));
    
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
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

PS_OUT	PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    if(0.f < g_vColor.a)
    {
        vector vColor = g_vColor;
        
        vColor.a *= vMtrlDiffuse.r;
  
        
        Out.vDiffuse = vColor;
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
        Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);
    }
    else
    {
        Out.vDiffuse = vMtrlDiffuse;
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
        Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);
    }

	return Out;
}

PS_OUT PS_MAIN_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    if (0.f < g_vColor.a)
    {
        vector vColor = g_vColor;
        
        vColor.a *= vMtrlDiffuse.r;
        
        if (vMtrlDiffuse.a < 0.3f)
            discard;
        
        Out.vDiffuse = vColor;
        Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
        Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);
    }
    else
    {
        Out.vDiffuse = vMtrlDiffuse;
        Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
        Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);
    }

    return Out;
}

PS_OUT PS_MAIN_SKY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
	
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
    
    if (fDissolveAlpha < 0.5f)
        Out.vDiffuse = vector(0.043735f, 0.366253f, 0.234551f, 1.f);
    else
        Out.vDiffuse = vMtrlDiffuse;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_DISSOLVE_NORMAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));
    
    vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fDissolveAlpha = saturate(-g_Time / g_DissolveDuration + vMtrlDissolve.r);
 
    if (fDissolveAlpha < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
      
    
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TangentMatrix = float3x3(In.vTangent, In.vBiTangent, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, TangentMatrix));
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    if (0.f < g_vColor.a)
    {
        vector vColor = g_vColor;
        
        vColor.a *= vMtrlDiffuse.r;
        vMtrlDiffuse = vColor;
    }


    vector vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

    vector vReflect = reflect(normalize(g_vLightDir), float4(vNormal, 0));
		
    vector vLook = In.vWorldPos - g_vCamPosition;

    vector vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f);
    
    Out.vDiffuse = vMtrlDiffuse * vShade + vSpecular;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Mesh
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

    pass Mesh_Normal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Mesh_NonCull
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Mesh_NonCull_Normal
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Mesh_Dissovle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass Mesh_Dissove_Normal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE_NORMAL();
    }

    pass Mesh_Sky
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKY();
    }

    pass Mesh_Blend
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();
    }

}




