
/* 상수테이블. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D		g_Texture;
Texture2D		g_Textures[2];


sampler LinearSampler = sampler_state {
	Filter = MIN_MAG_MIP_LINEAR;
};

sampler PointSampler = sampler_state {
	Filter = MIN_MAG_MIP_POINT;
};


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

	//Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
	
    vector vSourColor = g_Textures[0].Sample(LinearSampler, In.vTexcoord);
    vector vDestColor = g_Textures[1].Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = vSourColor + vDestColor;

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
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Edit
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EDIT();
    }
}




