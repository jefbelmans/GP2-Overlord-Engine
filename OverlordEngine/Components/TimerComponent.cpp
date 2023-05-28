#include "stdafx.h"
#include "TimerComponent.h"

void TimerComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/LemonMilk_32.fnt");
	m_IsPaused = true;

	FormatLaptime();

	m_ssLastLap.str(L"--:--.---");
	m_ssBestLap.str(L"--:--.---");
}

void TimerComponent::Start()
{
	m_IsPaused = false;
}

void TimerComponent::Pause()
{
	m_IsPaused = true;
}

void TimerComponent::Reset()
{
	m_CurrentLap = 0.f;
	m_LastLap = 0.f;
	m_BestLap = 0.f;
}

void TimerComponent::Lap()
{
	m_LastLap = m_CurrentLap;
	if(m_LastLap != 0.f)
		m_ssLastLap.str(m_ssCurrentLap.str());

	if ((m_CurrentLap < m_BestLap || m_BestLap == 0.f) && m_CurrentLap != 0.f)
	{
		m_BestLap = m_CurrentLap;
		m_ssBestLap.str(m_ssCurrentLap.str());
	}
	m_CurrentLap = 0.f;
}

void TimerComponent::Update(const SceneContext& sceneContext)
{
	if (!m_IsPaused)
	{
		m_CurrentLap += sceneContext.pGameTime->GetElapsed();
		FormatLaptime();
	}
}

void TimerComponent::Draw(const SceneContext& sceneContext)
{
	TextRenderer::Get()->DrawText(m_pFont, L"Lap: "  + m_ssCurrentLap.str(), {sceneContext.windowWidth - 265.f, 50.f}, XMFLOAT4{Colors::Orange});
	TextRenderer::Get()->DrawText(m_pFont, L"Best: " + m_ssBestLap.str(), {sceneContext.windowWidth - 265.f, 130.f}, XMFLOAT4{ Colors::Orange });
}

void TimerComponent::FormatLaptime()
{
	m_ssCurrentLap.str(L"");

	int ms = static_cast<int>(m_CurrentLap * 1000);
	int minutes = ms / (60 * 1000);
	ms -= minutes * 60 * 1000;
	int seconds = (ms / 1000) % 60;
	ms -= seconds * 1000;
	int milliseconds = ms % 1000;

	m_ssCurrentLap << std::setw(2) << std::setfill(L'0') << minutes << L":"
		<< std::setw(2) << std::setfill(L'0') << seconds << L"."
		<< std::setw(3) << std::setfill(L'0') << milliseconds;
}
