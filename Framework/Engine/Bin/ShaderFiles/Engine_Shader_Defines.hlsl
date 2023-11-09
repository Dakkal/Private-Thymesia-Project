#ifndef _ENGINE_SHADER_DEFINES_HLSL_
#define _ENGINE_SHADER_DEFINES_HLSL_

/* 샘플러  */
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

/* 레스터 */
RasterizerState RS_Edit
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
};


RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;	
};

BlendState BS_LightBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

void ComputeNormalMapping(inout float3 normal, texture2D NormalMap, float3 tangent, float2 uv)
{
    // [0, 255] 범위를 [0, 1]의 범위로 변환
    // Sample이 0, 1로 변환해주는 작업을 해주고 있었음.
    float4 map = NormalMap.Sample(PointSampler, uv);

    // rgb값 중 어느 하나가 값이있으면 true, 전부 없으면 false를 반환함.
    if (any(map.rgb) == false)
        return;
    
    float3 N = normalize(normal); // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y -> 축 2개를 알면 바이탄젠트를 구할 수 있음.
    float3x3 TBN = float3x3(T, B, N); // TS -> WS
    
    // [0, 1] 범위에서 [-1, 1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);
    
    normal = worldNormal;
}

#endif