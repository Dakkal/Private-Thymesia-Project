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

sampler LinearBorderSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
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

float4x4 RotateZ(float angle)
{
    angle = radians(angle);
    
    float c = cos(angle);
    float s = sin(angle);

    return float4x4(
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}


float4x4 RotateX(float angle)
{
    angle = radians(angle);
    
    float c = cos(angle);
    float s = sin(angle);

    return float4x4(
        1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    );
}

float4x4 RotateY(float angle)
{
    angle = radians(angle);
    
    float c = cos(angle);
    float s = sin(angle);

    return float4x4(
        c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    );
}

float4x4 RotateAroundVector(float3 vLook, float angle)
{
    float3 vRight = normalize(cross(vLook, float3(0, 1, 0))); // 수정된 부분
    float3 vUp = cross(vRight, vLook);

    float4x4 rotationMatrix = float4x4(
        vRight.x, vRight.y, vRight.z, 0,
        vUp.x, vUp.y, vUp.z, 0,
        vLook.x, vLook.y, vLook.z, 0,
        0, 0, 0, 1
    );

    angle = radians(angle);
    float c = cos(angle);
    float s = sin(angle);

    float4x4 rotation = float4x4(
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    return mul(rotationMatrix, rotation);
}

/* Moment 계산 */
/* SAVSM */
float g_DistributeFactor = 512;
float2 RecombinePrecision(float4 Value)
{
    float FactorInv = 1 / g_DistributeFactor;
    float Compute = Value.zw * FactorInv + Value.xy;
    return Compute;
}
float2 DistributePrecision(float2 Moments)
{
    float FactorInv = 1 / g_DistributeFactor;
    // Split precision    
    float2 IntPart;
    float2 FracPart = modf(Moments * g_DistributeFactor, IntPart);
    // Compose outputs to make reconstruction cheap.   
    float2 Compute = IntPart * FactorInv;
    float4 Value = float4(Compute.x, Compute.y, FracPart.x, FracPart.y);
    
    float2 ComputeMoment = RecombinePrecision(Value);
    
    return ComputeMoment;
}
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
    
    Moments = DistributePrecision(Moments);
    
    return Moments; 
} 

/* 라이트 블리딩 */
float LineStep(float a, float b, float v)
{
    return saturate((v - a) / (b - a));
}
float ReduceLightBleeding(float p_max, float Amount)
{ // Remove the [0, Amount] tail and linearly rescale (Amount, 1].    
    return LineStep(Amount, 1, p_max);
}
/* VSM */
float ChebyshevUpperBound(float2 Moments, float Depth)
{
    // Compute variance.    
    float Variance = Moments.y - (Moments.x * Moments.x);
    Variance = max(Variance, 0.00002f);
    // Compute probabilistic upper bound.    
    float d = Depth - Moments.x;
    float p_max = Variance / (Variance + d * d);
    
    p_max = ReduceLightBleeding(p_max, 0.2f);
    
    return (Depth <= Moments.x ? 1.0f : p_max);
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
    texelSize /= 8;

    for (int x = -3; x <= 3; ++x)
    {
        for (int y = -3; y <= 3; ++y)
        {
            float pcfDepth = LightDepthImg.Sample(PointSampler, projCoords.xy + float2(x, y) * texelSize).x * 1000.f;
            shadow += currentDepth > pcfDepth ? 0.5f : 1.0f;
        }
    }
    shadow /= 9.0f;
    return shadow;
}

#endif