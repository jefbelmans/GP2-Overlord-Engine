#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRenderTarget);
	SafeDelete(m_pBakedShadowRenderTarget);
}

void ShadowMapRenderer::Initialize()
{
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
	//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
	//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height
	RENDERTARGET_DESC desc{};
	desc.enableColorBuffer = false;
	desc.enableDepthSRV = true;
	desc.width = 8192;
	desc.height = 8192;

	// Baked shadow map
	m_pBakedShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	m_pBakedShadowRenderTarget->Create(desc);

	// Real time shadow map
	desc.width = 4096;
	desc.height = 4096;
	m_pShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	m_pShadowRenderTarget->Create(desc);

	// Set view width and height
	m_ViewWidth = 155.f;
	m_ViewHeight = 155.f;

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)
	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>();

	m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Static]
		= m_pShadowMapGenerator->GetTechniqueContext((int)ShadowGeneratorType::Static);

	m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Skinned]
		= m_pShadowMapGenerator->GetTechniqueContext((int)ShadowGeneratorType::Skinned);

	// Load baked shadowmap
	LoadBakedShadowMap();
}

void ShadowMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
	//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
	//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material

	//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
	int shadowGenType = static_cast<int>(pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static);
	const auto techniqueContext = m_GeneratorTechniqueContexts[shadowGenType];
	pMeshFilter->BuildVertexBuffer(sceneContext, techniqueContext.inputLayoutID, techniqueContext.inputLayoutSize, techniqueContext.pInputLayoutDescriptions);
}

void ShadowMapRenderer::Begin(const SceneContext& sceneContext)
{
	const bool bakeShadowMap{ sceneContext.pLights->GetBakeShadows() };

	// Change the viewport to match the shadow map size (baked or real time)
	D3D11_VIEWPORT vp{};
	if (bakeShadowMap)
	{
		vp.Width = static_cast<float>(m_pBakedShadowRenderTarget->GetDesc().width);
		vp.Height = static_cast<float>(m_pBakedShadowRenderTarget->GetDesc().height);
	}
	else
	{
		vp.Width = static_cast<float>(m_pShadowRenderTarget->GetDesc().width);
		vp.Height = static_cast<float>(m_pShadowRenderTarget->GetDesc().height);
	}
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;

	sceneContext.d3dContext.pDeviceContext->RSSetViewports(1, &vp);

	//This function is called once right before we start the Shadow Pass (= generating the ShadowMap)
	//This function is responsible for setting the pipeline into the correct state, meaning
	//	- Making sure the ShadowMap is unbound from the pipeline as a ShaderResource (SRV), so we can bind it as a RenderTarget (RTV)
	//	- Calculating the Light ViewProjection, and updating the relevant Shader variables
	//	- Binding the ShadowMap RenderTarget as Main Game RenderTarget (= Everything we render is rendered to this rendertarget)
	//	- Clear the current (which should be the ShadowMap RT) rendertarget

	//1. Making sure that the ShadowMap is unbound from the pipeline as ShaderResourceView (SRV) is important, because we cannot use the same resource as a ShaderResourceView (texture resource inside a shader) and a RenderTargetView (target everything is rendered too) at the same time. In case this happens, you'll see an error in the output of visual studio - warning you that a resource is still bound as a SRV and cannot be used as an RTV.
	//	-> Unbinding an SRV can be achieved using DeviceContext::PSSetShaderResource [I'll give you the implementation for free] - double check your output because depending on your usage of ShaderResources, the actual slot the ShadowMap is using can be different, but you'll see a warning pop-up with the correct slot ID in that case.
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	// Update light VP
	CalculateLightVP(sceneContext);
	CalculateBakedLightVP(sceneContext);

	// 3. Update this matrix (m_LightVP) on the ShadowMapMaterial effect, or m_BakedLightVP when baking shadows
	m_pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", reinterpret_cast<const float*>(bakeShadowMap ? &m_BakedLightVP : &m_LightVP));

	// 4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(bakeShadowMap ? m_pBakedShadowRenderTarget : m_pShadowRenderTarget);

	// 5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	 m_pBakedShadowRenderTarget->Clear();
	 m_pShadowRenderTarget->Clear();
}

void ShadowMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)

	//1. Figure out the correct ShadowGeneratorType (Static or Skinned)
	//2. Retrieve the correct TechniqueContext for m_GeneratorTechniqueContexts
	//3. Set the relevant variables on the ShadowMapMaterial
	//		- world of the mesh
	//		- if animated, the boneTransforms
	int shadowGenType = static_cast<int>(pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static);

	const auto& techniqueContext = m_GeneratorTechniqueContexts[shadowGenType];
	m_pShadowMapGenerator->SetVariable_Matrix(L"gWorld", reinterpret_cast<const float*>(&meshWorld));

	if (pMeshFilter->HasAnimations())
		m_pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", &meshBones[0]._11, (UINT)meshBones.size());

	//4. Setup Pipeline for Drawing (Similar to ModelComponent::Draw, but for our ShadowMapMaterial)
	//	- Set InputLayout (see TechniqueContext)
	//	- Set PrimitiveTopology
	//	- Iterate the SubMeshes of the MeshFilter (see ModelComponent::Draw), for each SubMesh:
	//		- Set VertexBuffer
	//		- Set IndexBuffer
	//		- Set correct TechniqueContext on ShadowMapMaterial - use ShadowGeneratorType as ID (BaseMaterial::SetTechnique)
	//		- Perform Draw Call (same as usual, iterate Technique Passes, Apply, Draw - See ModelComponent::Draw for reference)
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;

	//Set Inputlayout
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Set Vertex Buffer
		const UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(techniqueContext.inputLayoutID, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
			&offset);

		//Set Index Buffer
		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pShadowMapGenerator->SetTechnique(shadowGenType);

		//DRAW
		D3DX11_TECHNIQUE_DESC techDesc{};
		techniqueContext.pTechnique->GetDesc(&techDesc);

		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			techniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapRenderer::End(const SceneContext& sceneContext)
{
	//This function is called at the end of the Shadow-pass, all shadow-casting meshes should be drawn to the ShadowMap at this point.
	//Now we can reset the Main Game Rendertarget back to the original RenderTarget, this also Unbinds the ShadowMapRenderTarget as RTV from the Pipeline, and can safely use it as a ShaderResourceView after this point.

	//1. Reset the Main Game RenderTarget back to default (OverlordGame::SetRenderTarget)
	//		- Have a look inside the function, there is a easy way to do this...
	m_GameContext.pGame->SetRenderTarget(nullptr);

	// Restore the viewport size to the original
	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<float>(sceneContext.windowWidth);
	vp.Height = static_cast<float>(sceneContext.windowHeight);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	sceneContext.d3dContext.pDeviceContext->RSSetViewports(1, &vp);

	if (sceneContext.pLights->GetBakeShadows())
	{
		m_pBakedShadowRenderTarget->SaveDepthToFile(sceneContext, L"Resources/Textures/Baked Maps/ShadowMap.dds");
		sceneContext.pLights->SetBakeShadows(false);
		LoadBakedShadowMap();
		if (m_IsBakedInitialized && sceneContext.useDeferredRendering)
			DeferredRenderer::Get()->SetBakedLightmapDirty();

		Logger::LogInfo(L"Successfully baked shadow map!");
	}
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetBakedShadowMap() const
{
	return m_pBakedShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapRenderer::LoadBakedShadowMap()
{
	m_IsBakedInitialized = m_pBakedShadowRenderTarget->LoadDepthFromFile(m_GameContext.d3dContext, L"Resources/Textures/Baked Maps/ShadowMap.dds");
}

void ShadowMapRenderer::CalculateLightVP(const SceneContext& sceneContext)
{
	Light& dirLight = sceneContext.pLights->GetDirectionalLight();
	if (!dirLight.isDirty) return;

	// Projection
	const auto projection = XMMatrixOrthographicLH(sceneContext.aspectRatio * m_ViewWidth, m_ViewHeight, 0.1f, 400.f);

	// View
	const auto lightDir = XMLoadFloat4(&dirLight.direction);
	const auto lightPos = XMLoadFloat4(&dirLight.position);
	const XMMATRIX view = XMMatrixLookAtLH(lightPos, lightPos + lightDir, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	XMStoreFloat4x4(&m_LightVP, XMMatrixMultiply(view, projection));

	dirLight.isDirty = false;
}

void ShadowMapRenderer::CalculateBakedLightVP(const SceneContext& sceneContext)
{
	Light& dirLight = sceneContext.pLights->GetBakedDirectionalLight();
	if (!dirLight.isDirty) return;

	// Projection
	const auto projection = XMMatrixOrthographicLH(sceneContext.aspectRatio * 400.f, 400.f, 0.1f, 400.f);

	// View
	const auto lightDir = XMLoadFloat4(&dirLight.direction);
	const auto lightPos = XMLoadFloat4(&dirLight.position);
	const XMMATRIX view = XMMatrixLookAtLH(lightPos, lightPos + lightDir, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	XMStoreFloat4x4(&m_BakedLightVP, XMMatrixMultiply(view, projection));

	dirLight.isDirty = false;
}

void ShadowMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}

void ShadowMapRenderer::Debug_DrawBakedDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pBakedShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
