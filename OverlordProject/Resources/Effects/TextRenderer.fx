//INCOMPLETE!

float4x4 gTransform : WORLDVIEWPROJECTION;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
    DestBlendAlpha = ONE;
    SrcBlendAlpha = ONE;
};

DepthStencilState NoDepth
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{	
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    geomData.Channel = channel;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
    float3 pos = vertex[0].Position;
    float2 texCoord = vertex[0].TexCoord;
	
	//1. Vertex Left-Top
	//CreateVertex(...);
    CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);

	//2. Vertex Right-Top
	pos.x += vertex[0].CharSize.x;
	texCoord.x += vertex[0].CharSize.x / gTextureSize.x;
	
    CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);

	//3. Vertex Left-Bottom
	pos.x -= vertex[0].CharSize.x;
	pos.y += vertex[0].CharSize.y;
    texCoord.x -= vertex[0].CharSize.x / gTextureSize.x;
    texCoord.y += vertex[0].CharSize.y / gTextureSize.x;
	
    CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);

	//4. Vertex Right-Bottom
	pos.x += vertex[0].CharSize.x;
    texCoord.x += vertex[0].CharSize.x / gTextureSize.x;
	
    CreateVertex(triStream, pos, vertex[0].Color, texCoord, vertex[0].Channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET{

    float colorValue = gSpriteTexture.Sample(samPoint, input.TexCoord)[input.Channel];
    return float4(colorValue, colorValue, colorValue, colorValue) * input.Color;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState(NoDepth, 0);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
