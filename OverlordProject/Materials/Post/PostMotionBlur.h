#pragma once
class PostMotionBlur : public PostProcessingMaterial
{
public:
	PostMotionBlur();
	~PostMotionBlur() override = default;
	PostMotionBlur(const PostMotionBlur& other) = delete;
	PostMotionBlur(PostMotionBlur&& other) noexcept = delete;
	PostMotionBlur& operator=(const PostMotionBlur& other) = delete;
	PostMotionBlur& operator=(PostMotionBlur&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {};
	void UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource) override;

private:
	DirectX::XMFLOAT4X4 m_PreviousViewProjection{};
};

