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
	if (m_pParticleMaterial == nullptr)
		MaterialManager::Get()->CreateMaterial<ParticleMaterial>();

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

void ParticleEmitterComponent::Update(const SceneContext& /*sceneContext*/)
{
	/*float particleInterval{ (m_EmitterSettings.maxEnergy - m_EmitterSettings.minEnergy) / m_ParticleCount };
	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT hr = sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
		Logger::LogError(L"ParticleEmitterComponent::Update() > Failed to map vertex buffer!");

	VertexParticle* pVertexParticle = static_cast<VertexParticle*>(mappedResource.pData);

	const float dt = sceneContext.pGameTime->GetElapsed();
	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		if (m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], dt);
			return;
		}
			
		
		if (!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval)
		{

		}
	}*/
}

void ParticleEmitterComponent::UpdateParticle(Particle& /*p*/, float /*elapsedTime*/) const
{
	TODO_W9(L"Implement UpdateParticle")
}

void ParticleEmitterComponent::SpawnParticle(Particle& /*p*/)
{
	TODO_W9(L"Implement SpawnParticle")
}

void ParticleEmitterComponent::PostDraw(const SceneContext& /*sceneContext*/)
{
	TODO_W9(L"Implement PostDraw")
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}