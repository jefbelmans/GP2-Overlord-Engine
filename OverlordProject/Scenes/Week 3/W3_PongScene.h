#pragma once
class W3_PongScene final : public GameScene
{
public:
	W3_PongScene();
	~W3_PongScene() override = default;

	W3_PongScene(const W3_PongScene& other) = delete;
	W3_PongScene(W3_PongScene&& other) noexcept = delete;
	W3_PongScene& operator=(const W3_PongScene& other) = delete;
	W3_PongScene& operator=(W3_PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	// PADDLES
	const float m_PaddleSpeed{ 12.f };
	GameObject* m_pPaddleLeft{ nullptr };
	GameObject* m_pPaddleRight{ nullptr };
	RigidBodyComponent* m_pRBPaddleLeft{ nullptr };
	RigidBodyComponent* m_pRBPaddleRight{ nullptr };

	// PADDLE POS
	XMFLOAT3 m_LeftPaddlePos{ -20.f, 0.f, 0.f };
	XMFLOAT3 m_RightPaddlePos{ 20.f, 0.f, 0.f };

	GameObject* m_pBall{ nullptr };
	RigidBodyComponent* m_pRBBall{ nullptr };

	enum InputActions : int
	{
		P1_UP,
		P1_DOWN,
		P2_UP,
		P2_DOWN,
		RESET,
		START
	};
};

