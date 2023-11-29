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

/* Moment 계산 */
float2 ComputeMoments(float LightDepth)
{
    float2 Moments; 
    // First moment is the depth itself.   
    Moments.x = LightDepth;
    // Compute partial derivatives of depth.    
    float dx = ddx(LightDepth);
    float dy = ddy(LightDepth);
    // Compute second moment over the pixel extents.   
    Moments.y = LightDepth * LightDepth + 0.25 * (dx * dx + dy * dy);
    return Moments; 
} 

/* VSM */
float ChebyshevUpperBound(float2 Moments, float Depth)
{
    // One-tailed inequality valid if t > Moments.x    
    float p = (Depth <= Moments.x);
    // Compute variance.    
    float Variance = Moments.y - (Moments.x * Moments.x);
    Variance = max(Variance, 0.0005f);
    // Compute probabilistic upper bound.    
    float d = Depth - Moments.x;
    float p_max = Variance / (Variance + d * d);
    
    return max(p, p_max);
}

/* 라이트 블리딩 */
float linstep(float minValue, float maxValue, float v)
{
    return clamp(((v - minValue) / (maxValue - minValue)), 0, 1);
}

float ReduceLightBleeding(float p_max, float Amount)
{ // Remove the [0, Amount] tail and linearly rescale (Amount, 1].    
    return linstep(Amount, 1, p_max);
}

/* SAVSM */
float g_DistributeFactor = 256;
float2 RecombinePrecision(float4 Value)
{
    float FactorInv = 1 / g_DistributeFactor;
    return (Value.zw * FactorInv + Value.xy);
}
float2 DistributePrecision(float2 Moments) 
{   float FactorInv = 1 / g_DistributeFactor;   
    // Split precision    
    float2 IntPart;  
    float2 FracPart = modf(Moments * g_DistributeFactor, IntPart);
    // Compose outputs to make reconstruction cheap.    
    float4 Value = float4(IntPart * FactorInv, FracPart);
    return RecombinePrecision(Value);
}

//PCF
float PCF_ShadowCalculation(float4 fragPosLightSpace, texture2D LightDepthImg)
{
    // perform perspective divide
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = projCoords.y * -0.5f + 0.5f;

    float2 vTexCoords = saturate(projCoords.xy);

    if (vTexCoords.x != projCoords.x || vTexCoords.y != projCoords.y)
        return 1.0f;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
        return 1.0;

    // PCF
    float shadow = 0.0;
    // 클라이언트 화면 크기
    float2 texelSize = float2(1.f / 1280.f, 1.f / 720.f);
    // 쉐도우 이미지 크기배율
    texelSize /= 6;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = LightDepthImg.Sample(PointSampler, projCoords.xy + float2(x, y) * texelSize).x;
            shadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
        }
    }
    shadow /= 9.0f;
    return shadow;
}

#endif