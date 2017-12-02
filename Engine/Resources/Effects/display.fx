float4x4 MatrixProjection;
float4x4 MatrixView;
float4x4 MatrixWorld;

float2 resolution;

texture txrData;
sampler sampData = sampler_state
{
    Texture = <txrData>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//Structures
struct VS_INPUT
{
	float4 	Position: 	POSITION;
	float3 	normal:   	NORMAL;
	float2 	txr: 		TEXCOORD;
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

	Output.Position = mul(Input.Position, mul(MatrixWorld, mul(MatrixView, MatrixProjection)));
	Output.converPos =  mul(Input.Position, mul(MatrixWorld, mul(MatrixView, MatrixProjection)));
	Output.originPos =  Input.Position;
	Output.txr = Input.txr;
 	Output.norm = mul(MatrixWorld, normalize(Input.normal));

    return( Output );
}

PS_OUTPUT RenderScenePS( VS_OUTPUT In)
{
    PS_OUTPUT Output;
    Output.RGBColor = tex2D(sampData, float2(In.txr.y, -In.txr.x));
    //Output.RGBColor =  float4(0.0f, 0.3f, 0.0f, 1.0f);
    return( Output );
}



//Techniques
technique main
{
    pass P0
    {
        VertexShader = compile vs_2_0 RenderSceneVS();
        PixelShader  = compile ps_2_0 RenderScenePS();
    }
}

