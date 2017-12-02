float4x4 MatrixVP;
float4x4 MatrixVPC;
float4x4 MatrixWorld;

float dWidth, dHeight;

float x, y, z;

float3 lpos;


texture txrData;
sampler sampData = sampler_state
{
    Texture = <txrData>;
    MipFilter = NONE;
    MinFilter = NONE;
    MagFilter = NONE;
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
	float4 converPos : TEXCOORD0;
};
struct VS_OUTPUT2
{
    float4 Position : POSITION;
    float4 converPos : TEXCOORD1;
    float4 wPos : TEXCOORD2;
};
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color
};

struct txrUV
{
    int index;
    float u, v;  
};
txrUV convert_xyz_to_cube_uv(float x, float y, float z );

//Functions
VS_OUTPUT ShadowMapVS( VS_INPUT Input )
{
    VS_OUTPUT Output;
    float4 fPos =  mul(Input.Position, mul(MatrixWorld, MatrixVP));
    Output.Position = fPos;

    float4 fcPos = mul(Input.Position, MatrixWorld);
    Output.converPos = fcPos;

    return( Output );
}
PS_OUTPUT ShadowMapPS( VS_OUTPUT In)
{
    PS_OUTPUT Output;
    float3 cP = In.converPos.xyz;

    float originDepth = 1.0f - distance(lpos, cP.xyz) / 1000.0f;
    Output.RGBColor = float4(originDepth, originDepth, originDepth, 1.0f);

    return( Output );
}

///////////////////////////////////////////////////

VS_OUTPUT2 RenderSceneVS( VS_INPUT Input )
{
    VS_OUTPUT2 Output;

    float4 fPos =  mul(Input.Position, mul(MatrixWorld, MatrixVPC));
    Output.Position = fPos;
    float4 fcPos =  mul(Input.Position, mul(MatrixWorld, MatrixVP));
    Output.converPos =  fcPos;

    Output.wPos = mul(Input.Position, MatrixWorld);
    return( Output );
}

PS_OUTPUT RenderScenePS( VS_OUTPUT2 In)
{
    PS_OUTPUT Output;

    float smSize = 500.0f;

    float4 cP = In.converPos;
    cP.z += 1.0f;

    txrUV tmp = convert_xyz_to_cube_uv(cP.x, -cP.y, cP.z);

    int str = tmp.index / 3; 
    int column = tmp.index % 3;

    float pos_x, pos_y;
    pos_y = str * (smSize + y);
    pos_x = column * (smSize + z);

    pos_y += tmp.v * smSize;
    pos_x += tmp.u * smSize;

    pos_y /= smSize * 2;
    pos_x /= smSize * 3;

    float4 mapDepth = tex2D(sampData, float2(pos_x, pos_y));
    float originDepth = 1.0f - distance(lpos.xyz, In.wPos.xyz) / 1000.0f;


    if(abs(mapDepth.x - originDepth) < 0.00001f )  Output.RGBColor = float4(float3(originDepth, originDepth, originDepth)/2.0f, 1.0f);
    else Output.RGBColor = float4(float3(originDepth.x, originDepth.x, originDepth.x)/5.0f, 1.0f);  

    //if(x > 0) Output.RGBColor = float4(float3(mapDepth.x, mapDepth.y, mapDepth.x), 1.0f);
    //else Output.RGBColor = float4(float3(originDepth, originDepth, originDepth), 1.0f);

    return( Output );
}


txrUV convert_xyz_to_cube_uv(float x, float y, float z)
{
    txrUV returnVal;

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    int isXPositive = x > 0 ? 1 : 0;
    int isYPositive = y > 0 ? 1 : 0;
    int isZPositive = z > 0 ? 1 : 0;

    float maxAxis, uc, vc;

    // POSITIVE X
    if (isXPositive && absX >= absY && absX >= absZ) 
    {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -z;
        vc = y;
        returnVal.index = 0;
    }
    // NEGATIVE X
    if (!isXPositive && absX >= absY && absX >= absZ) 
    {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = z;
        vc = y;
        returnVal.index = 1;
    }
    // POSITIVE Y
    if (isYPositive && absY >= absX && absY >= absZ) 
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = x;
        vc = -z;
        returnVal.index = 3;
    }
    // NEGATIVE Y
    if (!isYPositive && absY >= absX && absY >= absZ) 
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = x;
        vc = z;
        returnVal.index = 2;
    }
    // POSITIVE Z
    if (isZPositive && absZ >= absX && absZ >= absY) 
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = x;
        vc = y;
        returnVal.index = 4;
    }
    // NEGATIVE Z
    if (!isZPositive && absZ >= absX && absZ >= absY) 
    {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -x;
        vc = y;
        returnVal.index = 5;
    }
    // Convert range from -1 to 1 to 0 to 1
    returnVal.u = 0.5f * (uc / maxAxis + 1.0f);
    returnVal.v = 0.5f * (vc / maxAxis + 1.0f);
    return returnVal;
}

//Techniques
technique map
{
    pass P0
    {
        VertexShader = compile vs_3_0 ShadowMapVS();
        PixelShader  = compile ps_3_0 ShadowMapPS();
    }
}

technique draw
{
    pass P0
    {
        VertexShader = compile vs_3_0 RenderSceneVS();
        PixelShader  = compile ps_3_0 RenderScenePS();
    }
}