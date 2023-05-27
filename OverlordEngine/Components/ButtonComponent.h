#pragma once

class ButtonComponent final : public BaseComponent, public IInteractable
{
public:
	ButtonComponent(const std::wstring& assetPath, const XMFLOAT2& position, const XMFLOAT2& scale = {1.f, 1.f});
	~ButtonComponent() override = default;

	ButtonComponent(const ButtonComponent& other) = delete;
	ButtonComponent(ButtonComponent&& other) noexcept = delete;
	ButtonComponent& operator=(const ButtonComponent& other) = delete;
	ButtonComponent& operator=(ButtonComponent&& other) noexcept = delete;

	void SetBaseAssetPath(const std::wstring& assetPath) { m_BaseAssetPath = assetPath; }
	void SetSelectedAssetPath(const std::wstring& assetPath) { m_SelectedAssetPath = assetPath; }
	void SetPressedAssetPath(const std::wstring& assetPath) { m_PressedAssetPath = assetPath; }

	void OnClickBegin() override;
	void OnClickEnd() override;
	void OnHoverBegin() override;
	void OnHoverEnd() override;

private:
	void Initialize(const SceneContext&) override;

	bool m_IsSelected{ false };

	SpriteComponent* m_pSpriteComponent{};

	// TEXTURES
	std::wstring m_BaseAssetPath{};
	std::wstring m_SelectedAssetPath{};
	std::wstring m_PressedAssetPath{};
};