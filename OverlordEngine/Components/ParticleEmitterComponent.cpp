#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	if (!m_pParticleMaterial)
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	if(m_pVertexBuffer)
		m_pVertexBuffer->Release();

	//a.Set the usage to Dynamic
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	//b.Use the ParticleCount and size of a VertexParticle to calculate the ByteWidth
	bufferDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);

	//c.This is a VertexBuffer, select the appropriate BindFlag
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//d.We want to edit the buffer at - runtime, so we need CPU write access.Select the
	//appropriate CPUAccessFlag
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//e.There are no MiscFlags(0)
	bufferDesc.MiscFlags = 0;

	//f.Create the VertexBuffer(GraphicsDevice > …)
	HRESULT hr = sceneContext.d3dContext.pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pVertexBuffer);

	//g.Don’t forget to check for errors!(HANDLE_ERROR)
	if (FAILED(hr))
		Logger::LogError(L"ParticleEmitterComponent::CreateVertexBuffer() > Failed to create vertex buffer!");
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	const float dt = sceneContext.pGameTime->GetElapsed();

	float particleInterval{ (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) * 0.5f / m_ParticleCount};
	
	m_LastParticleSpawn += dt;

	if (!m_IsPlaying)
		m_LastParticleSpawn = 0.f;

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT hr = sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
		Logger::LogError(L"ParticleEmitterComponent::Update() > Failed to map vertex buffer!");

	VertexParticle* pBuffer{ static_cast<VertexParticle*>(mappedResource.pData) };

	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		if (m_ParticlesArray[i].isActive)
			UpdateParticle(m_ParticlesArray[i], dt);
			
		
		if (!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval && m_IsPlaying)
		{
			SpawnParticle(m_ParticlesArray[i]);
			m_LastParticleSpawn -= particleInterval;
		}
			
		if (m_ParticlesArray[i].isActive)
			pBuffer[m_ActiveParticles++] = m_ParticlesArray[i].vertexInfo;
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	if (!p.isActive) return;

	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0.f)
	{
		p.isActive = false;
		return;
	}

	XMStoreFloat3(&p.vertexInfo.Position,
		XMVectorAdd(XMLoadFloat3(&p.vertexInfo.Position),
			XMVectorMultiply(XMLoadFloat3(&m_EmitterSettings.velocity),
				XMVectorSet(elapsedTime, elapsedTime, elapsedTime, 0.f))));

	float lifePercent{ p.currentEnergy / p.totalEnergy };

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercent;

	p.vertexInfo.Size.x = p.initialSize * (1.f + (p.sizeChange - 1.f) * (1.f - lifePercent));
	p.vertexInfo.Size.y = p.initialSize * (1.f + (p.sizeChange - 1.f) * (1.f - lifePercent));
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	p.isActive = true;

	p.currentEnergy = p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	const auto rndVec
	{
		XMVector3Normalize(
		{
			MathHelper::randF(-1.0f, 1.0f),
			MathHelper::randF(-1.0f, 1.0f),
			MathHelper::randF(-1.0f, 1.0f)
		})
	};
	const auto rndRange{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };

	XMStoreFloat3(&p.vertexInfo.Position,
			XMVectorAdd(
				XMVectorAdd(XMLoadFloat3(&GetTransform()->GetWorldPosition()),
					XMLoadFloat3(&m_SpawnOffset)),
					XMVectorMultiply(rndVec,XMVectorSet(rndRange, rndRange, rndRange, 0.f)))
	);



	p.vertexInfo.Size.x = p.initialSize = MathHelper::randF(m_EmitterSettings.minSize.x, m_EmitterSettings.maxSize.x);
	p.vertexInfo.Size.y = p.initialSize = MathHelper::randF(m_EmitterSettings.minSize.y, m_EmitterSettings.maxSize.y);

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	p.vertexInfo.Rotation = MathHelper::randF(-PxPi, PxPi);

	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture->GetShaderResourceView());

	const auto& techContext{ m_pParticleMaterial->GetTechniqueContext() };

	auto deviceContext{ sceneContext.d3dContext.pDeviceContext };

	deviceContext->IASetInputLayout(techContext.pInputLayout);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	constexpr UINT offset{}, startSlot{}, nrBuffers{ 1 };
	constexpr UINT stride{ sizeof(VertexParticle) };
	deviceContext->IASetVertexBuffers(startSlot, nrBuffers, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	techContext.pTechnique->GetDesc(&techDesc);
	for (unsigned int p = 0; p < techDesc.Passes; ++p)
	{
		techContext.pTechnique->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Min", &m_EmitterSettings.minSize.x, &m_EmitterSettings.minSize.y);
		ImGui::InputFloatRange("Size Max", &m_EmitterSettings.maxSize.x, &m_EmitterSettings.maxSize.y);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
		const auto& particlePos{ m_ParticlesArray[0].vertexInfo.Position };
		ImGui::Text("Particle 0 position: %f, %f, %f", particlePos.x, particlePos.y, particlePos.z);
	}
}

void ParticleEmitterComponent::SpawnBurst(int count)
{
	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		if (!m_ParticlesArray[i].isActive)
		{
			SpawnParticle(m_ParticlesArray[i]);
			count--;

			if(count <= 0)
				return;
		}
	}
}