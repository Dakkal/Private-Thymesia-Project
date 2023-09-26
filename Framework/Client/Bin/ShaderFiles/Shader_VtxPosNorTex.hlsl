
/* »ó¼öÅ×ÀÌºí. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector			g_vLightPos = vector(0.f, 0.f, 0.f, 1.f);
float			g_fLightRange = 0.f;
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_CamPosition;

Texture2D		g_DiffuseTexture[2];
Texture2D		g_MaskTexture;

Texture2D		g_BrushTexture;

vector			g_vBrushPos = vector(35.f, 0.f, 35.f, 1.f);
float			g_fBrushRange = 15.0f;


/* »ùÇÃ·¯  */
sampler LinearSampler = sampler_state {
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state {
	Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

/* ¹öÅØ½º ½¦ÀÌ´õ */
struct VS_IN
{	
	float3		vPosition : POSITION;
    float3		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
    float4		vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord; 
    Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	
	return Out;	
}

/* ÇÈ¼¿ ½¦ÀÌ´õ */
struct PS_IN
{
	float4		vPosition : SV_POSITION;
    float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
    float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

float4 Compute_TerrainPixelColor(PS_IN In)
{
    vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 20.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 20.f);
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBrushColor = vector(0.f, 0.f, 0.f, 1.f);
	
    if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
    {
        float2 vUV;
		
        vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);
		
        vBrushColor = g_BrushTexture.Sample(LinearSampler, vUV);
    }
	
    return vDestDiffuse * vMaskColor + vSourDiffuse + vSourDiffuse * (1.f - vMaskColor) + vBrushColor;
}


PS_OUT	PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    vector vMtrlDiffuse = Compute_TerrainPixelColor(In);
	
    vector vShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f)
					+ g_vLightAmbient * g_vMtrlAmbient;

    vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_CamPosition;
	
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
	

    Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade)
				+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
	
	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = Compute_TerrainPixelColor(In);
	
    vector  vLightDir = In.vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    float fLightAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
	
    vector vShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f)
					+ g_vLightAmbient * g_vMtrlAmbient;

    vector vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));
    vector vLook = In.vWorldPos - g_CamPosition;
	
    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);
	

    Out.vColor = ((g_vLightDiffuse * vMtrlDiffuse) * saturate(vShade)
				+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fLightAtt;
	
    return Out;
}

technique11 DefaultTechnique
{
	pass Terrain_Directional
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Terrain_Point
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }
}




