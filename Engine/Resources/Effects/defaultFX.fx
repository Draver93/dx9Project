float4x4 MatrixProjection;
float4x4 MatrixView;
float4x4 MatrixWorld;

//Structures
struct VS_INPUT
{
	float4 	Position: 	POSITION;
	float3 	normal:   	NORMAL;
	float2 	txr: 		TEXCOORD;
    float4 	weight: 	BLENDWEIGHT;
	float4	boneID: 	BLENDINDICES;
};
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 txr: TEXCOORD0;
	float3 norm: TEXCOORD1;
	float4 converPos : TEXCOORD2;
	float4 originPos : TEXCOORD3;
};
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color
};

//Functions
VS_OUTPUT RenderSceneVS( VS_INPUT Input )
{
    VS_OUTPUT Output;

	Output.Position = mul(mul(Input.Position, MatrixWorld), mul(MatrixView, MatrixProjection));
	Output.converPos = mul(Input.Position,  mul(MatrixWorld, mul(MatrixView, MatrixProjection)));
	Output.originPos = mul(Input.Position, MatrixWorld);
	Output.txr = Input.txr;
 	Output.norm = normalize(Input.normal);

    return( Output );
}

PS_OUTPUT RenderScenePS( VS_OUTPUT In)
{
    PS_OUTPUT Output;

    float range = 10.0f;
    Output.RGBColor = float4(0.5f, 1.0f, 1.0f, 1.0f);

    return( Output );
}

//Techniques
technique mainTec
{
    pass P0
    {
        VertexShader = compile vs_3_0 RenderSceneVS();
        PixelShader  = compile ps_3_0 RenderScenePS();
    }
}
