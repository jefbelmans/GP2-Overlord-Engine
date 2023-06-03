#pragma once
class SpriteFont;

class TimerComponent : public BaseComponent
{
public:
	TimerComponent() = default;
	~TimerComponent() override = default;
	TimerComponent(const TimerComponent& other) = delete;
	TimerComponent(TimerComponent&& other) noexcept = delete;
	TimerComponent& operator=(const TimerComponent& other) = delete;
	TimerComponent& operator=(TimerComponent&& other) noexcept = delete;

	void Start();
	void Pause();
	void Reset();
	void Lap();
	void EnableDrawing(bool doEnable) { m_IsEnabled = doEnable;}

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
	void Draw(const SceneContext& sceneContext) override;

private:
	bool m_IsPaused{};
	bool m_IsEnabled{ true };
	
	float m_CurrentLap{};
	float m_LastLap{};
	float m_BestLap{};

	SpriteFont* m_pFont{};
	std::wstringstream m_ssCurrentLap{};
	std::wstringstream m_ssLastLap{};
	std::wstringstream m_ssBestLap{};

	void FormatLaptime();
};

