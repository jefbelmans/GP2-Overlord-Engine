#include "stdafx.h"
#include "W3_PongScene.h"

// PREFABS
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

W3_PongScene::W3_PongScene()
	: GameScene(L"Pong Scene")
{
}

void W3_PongScene::Initialize()
{
	// SCENE SETUP
	m_SceneContext.settings.clearColor = XMFLOAT4{ Colors::Black };
	m_SceneContext.settings.drawGrid = false;

	m_SceneContext.pCamera->GetTransform()->Translate(0.f, 20.f, -35.f);

	// INPUT ACTIONS
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::P1_UP, InputState::down, VK_UP));
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::P1_DOWN, InputState::down, VK_DOWN));
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::P2_UP, InputState::down, 'Y'));
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::P2_DOWN, InputState::down, 'H'));
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::RESET, InputState::pressed, 'R'));
	m_SceneContext.pInput->AddInputAction(InputAction(InputActions::START, InputState::pressed, VK_SPACE));

	// CONSTANTS
	auto& pPhysX = PxGetPhysics();
	const XMFLOAT3 paddleDimensions{ 1.f, 2.f, 5.f };
	const PxBoxGeometry boxGeometry{ paddleDimensions.x / 2.f, paddleDimensions.y / 2.f, paddleDimensions.z / 2.f };
	auto pDefaultMaterial = pPhysX.createMaterial(.0f, .0f, 0.99999f);

	// PADDLE LEFT
	m_pPaddleLeft = new CubePrefab(paddleDimensions, XMFLOAT4{ Colors::White });
	m_pPaddleLeft->GetTransform()->Translate(m_LeftPaddlePos);
	AddChild(m_pPaddleLeft);

	// RIGIDBODY LEFT
	m_pRBPaddleLeft = m_pPaddleLeft->AddComponent(new RigidBodyComponent());
	m_pRBPaddleLeft->AddCollider(boxGeometry, *pDefaultMaterial);
	m_pRBPaddleLeft->SetKinematic(true);


	// PADDLE RIGHT
	m_pPaddleRight = new CubePrefab(paddleDimensions, XMFLOAT4{ Colors::White });
	m_pPaddleRight->GetTransform()->Translate(m_RightPaddlePos);
	AddChild(m_pPaddleRight);

	// RIGIDBODY RIGHT
	m_pRBPaddleRight = m_pPaddleRight->AddComponent(new RigidBodyComponent());
	m_pRBPaddleRight->AddCollider(boxGeometry, *pDefaultMaterial);
	m_pRBPaddleRight->SetKinematic(true);

	// BALL
	m_pBall = new SpherePrefab(1.f, 10, XMFLOAT4{ Colors::Red });
	AddChild(m_pBall);

	// WALLS
	const PxBoxGeometry nsWallGeometry = PxBoxGeometry(25.f, 1.5f, 0.5f);
	const PxBoxGeometry ewWallGeometry = PxBoxGeometry(10.f, 1.5f, 0.5f);

	auto northWall = new GameObject();
	auto rb = northWall->AddComponent(new RigidBodyComponent());
	rb->AddCollider(nsWallGeometry, *pDefaultMaterial);
	rb->SetKinematic(true);
	northWall->GetTransform()->Translate(0.f, 0.f, 10.f);
	AddChild(northWall);

	auto southWall = new GameObject();
	rb = southWall->AddComponent(new RigidBodyComponent());
	rb->AddCollider(nsWallGeometry, *pDefaultMaterial);
	rb->SetKinematic(true);
	southWall->GetTransform()->Translate(0.f, 0.f, -10.f);
	AddChild(southWall);

	auto eastWall = new GameObject();
	rb = eastWall->AddComponent(new RigidBodyComponent());
	rb->AddCollider(ewWallGeometry, *pDefaultMaterial);
	rb->SetKinematic(true);
	eastWall->GetTransform()->Rotate(0.f, 90.f, 0.f);
	eastWall->GetTransform()->Translate(25.f, 0.f, 0.f);
	AddChild(eastWall);

	auto westWall = new GameObject();
	rb = westWall->AddComponent(new RigidBodyComponent());
	rb->AddCollider(ewWallGeometry, *pDefaultMaterial);
	rb->SetKinematic(true);
	westWall->GetTransform()->Rotate(0.f, 90.f, 0.f);
	westWall->GetTransform()->Translate(-25.f, 0.f, 0.f);
	AddChild(westWall);

	// RIGIDBDOY BALL
	m_pRBBall = m_pBall->AddComponent(new RigidBodyComponent());
	m_pRBBall->AddCollider(PxSphereGeometry{ 1.f }, *pDefaultMaterial);
	m_pRBBall->SetConstraint(
		RigidBodyConstraint::RotX |
		RigidBodyConstraint::RotY |
		RigidBodyConstraint::RotZ |
		RigidBodyConstraint::TransY
		, false);
}

void W3_PongScene::Update()
{
	// INPUT PLAYER 1
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::P1_UP))
	{
		m_LeftPaddlePos.z += m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();
		m_LeftPaddlePos.z = std::clamp(m_LeftPaddlePos.z, -6.f, 6.f);
		m_pPaddleLeft->GetTransform()->Translate(m_LeftPaddlePos);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::P1_DOWN))
	{
		m_LeftPaddlePos.z -= m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();
		m_LeftPaddlePos.z = std::clamp(m_LeftPaddlePos.z, -6.f, 6.f);
		m_pPaddleLeft->GetTransform()->Translate(m_LeftPaddlePos);
	}

	// INPUT PLAYER 2
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::P2_UP))
	{
		m_RightPaddlePos.z += std::min(m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed(), 5.f);
		m_RightPaddlePos.z = std::clamp(m_RightPaddlePos.z, -6.f, 6.f);
		m_pPaddleRight->GetTransform()->Translate(m_RightPaddlePos);
	}
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::P2_DOWN))
	{
		m_RightPaddlePos.z -= m_PaddleSpeed * m_SceneContext.pGameTime->GetElapsed();
		m_RightPaddlePos.z = std::clamp(m_RightPaddlePos.z, -6.f, 6.f);
		m_pPaddleRight->GetTransform()->Translate(m_RightPaddlePos);
	}

	// RESET
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::RESET))
	{
		// PADDLES
		m_LeftPaddlePos.z = 0.f;
		m_RightPaddlePos.z = 0.f;

		m_pPaddleLeft->GetTransform()->Translate(m_LeftPaddlePos);
		m_pPaddleRight->GetTransform()->Translate(m_RightPaddlePos);

		m_pRBBall->ClearForce();
		m_pBall->GetTransform()->Translate(0.f, 0.f, 0.f);
	}

	// START
	if (m_SceneContext.pInput->IsActionTriggered(InputActions::START))
	{
		m_pRBBall->AddForce(XMFLOAT3(10.f, 0.f, 10.f), PxForceMode::eIMPULSE);
	}
}

void W3_PongScene::Draw()
{
}

void W3_PongScene::OnGUI()
{
}
