#pragma once
class VO_MenuScene final : public GameScene
{
public:
	VO_MenuScene() : GameScene(L"Velocity Overdrive - Menu Scene") {};
	~VO_MenuScene() override = default;
	VO_MenuScene(const VO_MenuScene& other) = delete;
	VO_MenuScene(VO_MenuScene&& other) noexcept = delete;
	VO_MenuScene& operator=(const VO_MenuScene& other) = delete;
	VO_MenuScene& operator=(VO_MenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Draw() override;
	void OnGUI() override;

	void OnSceneActivated() override;

private:
#pragma region UI
	// FONT
	SpriteFont* m_pFontText{};
	SpriteFont* m_pFontTitle{};

	// MAIN MENU
	GameObject* m_pStartButton{};
	GameObject* m_pOptionsButton{};
	GameObject* m_pQuitButton{};

	// OPTIONS MENU
	GameObject* m_pBackButton{};
	GameObject* m_pBakedShadowsCheckBox{};

	GameObject* m_pBanner{};
#pragma endregion

#pragma region Camera
	FixedCamera* m_pFixedCamera{};
#pragma endregion

	void LoadGame();
	void QuitGame();
};

