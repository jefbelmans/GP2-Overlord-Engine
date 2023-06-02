//Deferred_DirectionalLightPas > Fullscreen Quad Render
#include "LightPass_Helpers.fx"

//VARIABLES
//*********
float4x4 gMatrixViewProjInv; // Used to transform from screen to world space
float3 gEyePos = float3(0, 0, 0);
Light gDirectionalLight;

// Shadow mapping
Texture2D gTextureShadowMap;
Texture2D gTextureBakedShadowMap;

float4x4 gLightViewProj; // Used to transform from world to realtime light space
float4x4 gBakedLightViewProj; // Used to transform from world to baked light space
float gShadowMapBias = 0.0002f;
bool gUseBakedShadowMap = false;

//G-BUFFER DATA
//Texture2D gTextureAmbient; >> Already on Main RenderTarget
Texture2D gTextureDiffuse;
Texture2D gTextureSpecular;
Texture2D gTextureNormal;
Texture2D gTextureDepth;

SamplerState gTextureSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

//VS & PS IO
//**********
struct VS_INPUT
{
	float3 Position: POSITION;
	float2 TexCoord: TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//STATES
//******
RasterizerState gRasterizerState
{
	FillMode = SOLID;
	CullMode = BACK;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

BlendState gBlendState //Additive Blending (LIGHT-ACCUMULATION + LIGHTING-RESULTS)
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

//VERTEX SHADER
//*************
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;

	return output;
}

//PIXEL SHADER
//************
float4 PS(VS_OUTPUT input) :SV_TARGET
{
    int2 screenCoord = input.Position.xy;
    int3 loadCoord = int3(screenCoord, 0);
	
	// Calculate pixel world position from depth value
    float depth = gTextureDepth.Load(loadCoord).r;
    float3 P = DepthToWorldPosition_QUAD(depth, input.TexCoord, gMatrixViewProjInv);
	
    float3 V = normalize(P - gEyePos);
    float3 L = normalize(gDirectionalLight.Direction.xyz); // LIGHT DIRECTION
    float3 N = gTextureNormal.Load(loadCoord).xyz; // NORMAL
	
	float3 diffuse = gTextureDiffuse.Load(loadCoord).rgb; // DIFFUSE
	float4 specular = gTextureSpecular.Load(loadCoord); // SPECULAR
	float shinines = exp2(specular.a * 10.5f);
	
	// MATERIAL
    Material mat = (Material) 0;
    mat.Diffuse = diffuse;
	mat.Specular = specular.rgb;
	mat.Shininess = shinines;
	
	// Shadow mapping
    float4 lPos = mul(float4(P, 1.0f), gLightViewProj);
    float4 lPosBaked = mul(float4(P, 1.0f), gBakedLightViewProj);
	
    float shadowValue = EvaluateShadowMap(lPos, gTextureShadowMap, gShadowMapBias);
	if(gUseBakedShadowMap)
        shadowValue = min(shadowValue, EvaluateShadowMap(lPosBaked, gTextureBakedShadowMap, gShadowMapBias));
	
	// Do Lighting
    LightingResult result = DoDirectionalLighting(gDirectionalLight, mat, L, V, N);
	
    return float4(((mat.Diffuse * result.Diffuse) + (mat.Specular * result.Specular)) * shadowValue, 1.0f);
}

//TECHNIQUE
//*********
technique11 Default
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS() ));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
};
