//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

float gKernelSize = 3.0f;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState depthEnabled
{
	DepthEnable = true;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};
/// Create Rasterizer State (Backface culling) 
RasterizerState cullBack
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	// Set the Position
	output.Position = float4(input.Position, 1.0f);
	// Set the TexCoord
	output.TexCoord = input.TexCoord;
	
	return output;
}

//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
    int width{}, height{};
	gTexture.GetDimensions(width, height);
	
	// Step 2: calculate dx and dy (UV space for 1 pixel)
	float dx = 1.0f / width;
	float dy = 1.0f / height;
	
	// Step 3: Create a double for loop (6 iterations each)
	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = -gKernelSize; i < gKernelSize; ++i)
    {
        for (int j = -gKernelSize; j < gKernelSize; ++j)
        {
			float2 offset = float2(i * dx * 2, j * dy * 2);
			finalColor += gTexture.Sample(samPoint, input.TexCoord + offset);
			
        }
    }
	// Step 4: Divide the final color by the number of passes (in this case 6*6)	
	// Step 5: return the final color

     return finalColor / 36.f;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetDepthStencilState(depthEnabled, 0);
		SetRasterizerState(cullBack);

        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}