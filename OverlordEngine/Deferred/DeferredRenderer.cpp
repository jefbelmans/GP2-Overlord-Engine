#include "stdafx.h"
#include "DeferredRenderer.h"

#include "DeferredLightRenderer.h"

DeferredRenderer::~DeferredRenderer()
{
	// Skip the first one, it's the main RT, this is handled by the Engine
	for (size_t i = 1; i < RT_COUNT; i++)
	{
		SafeDelete(m_GBuffer[i]);
	}

	SafeDelete(m_pLightPassRenderer);
	
}

void DeferredRenderer::Initialize()
{
	//Create & Collect GBuffer RTs
	const auto w = m_GameContext.windowWidth;
	const auto h = m_GameContext.windowHeight;

	const auto pDefaultRenderTarget = m_GameContext.pGame->GetRenderTarget(); // Main RT with Depth Buffer
	m_pDefaultRenderTargetView = pDefaultRenderTarget->GetRenderTargetView(); // Main RTV
	m_pDefaultDepthStencilView = pDefaultRenderTarget->GetDepthStencilView(); // Main DSV

	m_GBuffer[int(eGBufferId::LightAccumulation)] = pDefaultRenderTarget; // Ambient color + depth, additive blending is used to shade
	m_GBuffer[int(eGBufferId::Diffuse)] = CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_GBuffer[int(eGBufferId::Specular)] = CreateRenderTarget(w, h, DXGI_FORMAT_R8G8B8A8_UNORM); 
	m_GBuffer[int(eGBufferId::Normal)] = CreateRenderTarget(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT);
	m_GBuffer[int(eGBufferId::Mask)] = CreateRenderTarget(w, h, DXGI_FORMAT_R16_UNORM);

	//Collect References of RTVs
	for (size_t i = 0; i < RT_COUNT; i++)
		m_RenderTargetViews[i] = m_GBuffer[i]->GetRenderTargetView();

	//Collect SRVs (Ambient/Diffuse/Specular/Normal/Mask) + DepthSRV
	for (size_t i = 0; i < SRV_COUNT - 1; i++)
		m_ShaderResourceViews[i] = m_GBuffer[i]->GetColorShaderResourceView();

	m_ShaderResourceViews[int(eGBufferId::Depth)] = pDefaultRenderTarget->GetDepthShaderResourceView(); // DepthSRV

	//LightPassRenderer Init
	m_pLightPassRenderer = new DeferredLightRenderer;
	m_pLightPassRenderer->Initialize(m_GameContext.d3dContext);

	// CREATE READ ONLY DSV
	const auto& descDefaultRT = pDefaultRenderTarget->GetDesc();
	m_pLightPassRenderer->CreateReadOnlyDSV(m_GameContext.d3dContext, descDefaultRT.pDepth, descDefaultRT.depthFormat);
}

RenderTarget* DeferredRenderer::CreateRenderTarget(UINT width, UINT height, DXGI_FORMAT format) const
{
	RENDERTARGET_DESC rtDesc{};

	rtDesc.width = width;
	rtDesc.height = height;
	rtDesc.colorFormat = format;

	rtDesc.enableColorBuffer = true;
	rtDesc.enableColorSRV = true;

	const auto pRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	HANDLE_ERROR(pRenderTarget->Create(rtDesc))

	return pRenderTarget;
}

void DeferredRenderer::Begin(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	const auto pDeviceContext = m_GameContext.d3dContext.pDeviceContext;

	//1. Clear the G-Buffer RTVs
	const auto clearColor = XMFLOAT4{ Colors::Black }; //As we use additive blending and we do not want to add anything to the ambient color

	for (size_t i = 0; i < RT_COUNT; i++)
		pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[i], &clearColor.x);

	pDeviceContext->ClearDepthStencilView(m_pDefaultDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//2. Bind GBuffer RTVs + DSV
	pDeviceContext->OMSetRenderTargets(RT_COUNT, m_RenderTargetViews, m_pDefaultDepthStencilView);
}

void DeferredRenderer::End(const SceneContext& sceneContext) const
{
	//Ignore this function if DeferredRendering is not 'active'
	if (!sceneContext.useDeferredRendering) return;

	const auto pDeviceContext = m_GameContext.d3dContext.pDeviceContext;

	//1. Geometry Pass is finished > Unbind GBuffer RTVs
	ID3D11RenderTargetView* nullRTV[RT_COUNT]{ nullptr };
	nullRTV[0] = m_pDefaultRenderTargetView; //Keep Main RTV (Ambient)
	pDeviceContext->OMSetRenderTargets(RT_COUNT, nullRTV, nullptr);

	//3. Directional Light Pass
	m_pLightPassRenderer->DirectionalLightPass(sceneContext, m_ShaderResourceViews);

	//4. Volumetric Light Pass
	m_pLightPassRenderer->VolumetricLightPass(sceneContext, m_ShaderResourceViews, m_pDefaultRenderTargetView);

	//5. Unbind G-Buffer SRVs (Diffuse, Specular, Normal, Mask & Depth)
	ID3D11ShaderResourceView* pSRV[SRV_COUNT]{ nullptr };
	pDeviceContext->PSSetShaderResources(0, SRV_COUNT, pSRV);

	//6. Reset Game RenderTarget (back to normal)
	m_GameContext.pGame->SetRenderTarget(nullptr);

	//DEBUG >> Visualize GBUFFER
	//Draw ImGui (to default RT)
	if(m_DrawImGui)
	{
		Debug_DrawGBuffer();
	}
}

void DeferredRenderer::DrawImGui()
{
	ImGui::Checkbox("Draw GBuffer", &m_DrawImGui);

	if(m_DrawImGui)
		ImGui::SliderInt("RTV ID", &m_VizRTVid, 0, RT_COUNT - 1);
}

void DeferredRenderer::SetBakedLightmapDirty()
{
	m_pLightPassRenderer->SetBakedLightmapDirty(true);
}

void DeferredRenderer::Debug_DrawGBuffer() const
{
	//FULL SCREEN
	if(m_VizRTVid >= 1)
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[m_VizRTVid]);

	const auto aspectRatio = m_GameContext.windowWidth / float(m_GameContext.windowHeight);
	const auto width = static_cast<int>(m_GameContext.windowWidth / 3.f);
	const auto height = static_cast<int>(width / aspectRatio);

	Quad renderQuad{
		0,
		int(m_GameContext.windowHeight) - height,
		width,
		height
	};

	//GBUFFER SRVs (only Diffuse/Specular/Normal) - skip Ambient & Depth
	for (auto i{ 1 }; i < SRV_COUNT-1; ++i)
	{
		QuadRenderer::Get()->Draw(m_ShaderResourceViews[i], renderQuad);
		renderQuad.x += width;
	}
}
