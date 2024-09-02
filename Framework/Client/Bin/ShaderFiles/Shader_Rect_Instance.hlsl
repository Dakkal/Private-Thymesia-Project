#include "Engine_Shader_Defines.hlsl"

/* ������̺�. */
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_TargetMatrix;
matrix			g_RotMatricesX[100], g_RotMatricesY[100], g_RotMatricesZ[100];
texture2D		g_Texture;



struct VS_IN
{
	/* �׸���� ����. m_pVB */
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;

	/* ���º�ȯ�� ����. m_pVBInstance */
	float4		vRight		 : TEXCOORD1;
	float4		vUp			 : TEXCOORD2;
	float4		vLook		 : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	/* float4 : w���� �ݵ�� ���ܾ��ϴ� ������ w�� �佺���̽� ���� ����(z)�� �����ϱ� ���ؼ���. */
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};


/* ���ؽ��� ���� ��ȯ�۾��� ��ģ��.  */
/* ��ȯ�۾� : ������ ��ġ�� ����, ��, ��������� ���Ѵ�. and �ʿ��� ��ȯ�� ���ؼ� �����Ӱ� �߰��ص� ������� .*/
/* ���ؽ��� ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /* ���� */VS_IN In, uint instanceID : SV_InstanceID)
{
	VS_OUT			Out = (VS_OUT)0;

	float4x4		TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float4 vRight = mul(float4(1.f, 0.f, 0.f, 0.f), g_RotMatricesX[instanceID]);
    float4 vUp = mul(float4(0.f, 1.f, 0.f, 0.f), g_RotMatricesX[instanceID]);
    float4 vLook = mul(float4(0.f, 0.f, 1.f, 0.f), g_RotMatricesX[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesY[instanceID]);
    vUp = mul(vUp, g_RotMatricesY[instanceID]);
    vLook = mul(vLook, g_RotMatricesY[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesZ[instanceID]);
    vUp = mul(vUp, g_RotMatricesZ[instanceID]);
    vLook = mul(vLook, g_RotMatricesZ[instanceID]);
	
    float4x4 TargetMatrix = g_TargetMatrix;
    TargetMatrix[0].xyz = normalize(g_TargetMatrix[0].xyz);
    TargetMatrix[1].xyz = normalize(g_TargetMatrix[1].xyz);
    TargetMatrix[2].xyz = normalize(g_TargetMatrix[2].xyz);
    TargetMatrix[3].xyzw = (float4)0.f;
	
    vRight = mul(vRight, TargetMatrix);
    vUp = mul(vUp, TargetMatrix);
    vLook = mul(vLook, TargetMatrix);
	
    float4 vPos = g_WorldMatrix._41_42_43_44;
	
    float4x4 newWorld = float4x4(vRight, vUp, vLook, vPos);
	
	matrix			matWV, matWVP;

    matWV = mul(newWorld, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);

	Out.vTexcoord = In.vTexcoord;

	return Out;	
}

VS_OUT VS_MAIN_BillBoard( /* ���� */VS_IN In, uint instanceID : SV_InstanceID)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

    float4 vRight = mul(g_WorldMatrix[0].xyzw, g_RotMatricesX[instanceID]);
    float4 vUp = mul(g_WorldMatrix[1].xyzw, g_RotMatricesX[instanceID]);
    float4 vLook = mul(g_WorldMatrix[2].xyzw, g_RotMatricesX[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesY[instanceID]);
    vUp = mul(vUp, g_RotMatricesY[instanceID]);
    vLook = mul(vLook, g_RotMatricesY[instanceID]);
	
    vRight = mul(vRight, g_RotMatricesZ[instanceID]);
    vUp = mul(vUp, g_RotMatricesZ[instanceID]);
    vLook = mul(vLook, g_RotMatricesZ[instanceID]);
	
    float4 vPos = g_WorldMatrix._41_42_43_44;
	
    float4x4 newWorld = float4x4(vRight, vUp, vLook, vPos);
	
	/* mul : ���(���ϱⰡ ������) ����� ���ϱ⸦ �����Ѵ�. */
    matrix matWV, matWVP;

    matWV = mul(newWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* w������ ����. ������ ������ȯ. */
/* ����Ʈ�����̽�(��������ǥ)�� ��ġ�� ��ȯ�Ѵ�. */
/* �����Ͷ����� : ������ �ѷ����� �ȼ��� ������ �����Ѵ�. */
/* �ȼ������� ���������� ����Ѵ�. */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

/* �޾ƿ� �ȼ��� ������ �������� �Ͽ� ȭ�鿡 �׷��� �ȼ��� �������� ���� �����ϳ�. */
struct PS_OUT
{
	float4	vColor : SV_TARGET0;
};

/* ���޹��� �ȼ��� ������ �̿��Ͽ� �ȼ��� �������� ���� ��������. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);

   // ���̴����� �ؽ�ó�� ���� ä���� �о��
    float alphaValue = Out.vColor.r;
    Out.vColor.a = alphaValue;
	
	
    if (alphaValue < 0.2f)
        discard;
	
	
    Out.vColor.rgb = float3(1.f, 0.7f, 0.3f);

	return Out;
}

technique11 DefaultTechnique
{
	/* */
	pass Particle
	{
        SetRasterizerState(RS_Sky);
		SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Particle_BillBord
    {
        SetRasterizerState(RS_Sky);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* ���� ���̴��� ���ؼ� ���� � �������� �����ϰ� � �Լ��� ȣ���Ͽ� �ش� ���̴��� �����Ǵ����� �����Ѵ�. */
        VertexShader = compile vs_5_0 VS_MAIN_BillBoard();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}




