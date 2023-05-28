#include "stdafx.h"
#include "VO_MenuScene.h"
#include "Prefabs/CubePrefab.h"

void VO_MenuScene::Initialize()
{
	// SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.clearColor = XMFLOAT4{ 112.f / 255.f, 139.f / 255.f, 196.f / 255.f, 1.f };

	// UI
	// FONT
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/LemonMilk_32.fnt");

	// MAIN MENU
	// START BUTTON
	m_pStartButton = new GameObject();
	auto pButton = m_pStartButton->AddComponent(new ButtonComponent(std::bind(&VO_MenuScene::LoadGame, this), L"Textures/UI/ButtonBase.png", { 40.f, m_SceneContext.windowHeight - 130.f }, { 3.f, 3.f }));
	pButton->SetSelectedAssetPath(L"Textures/UI/ButtonSelected.png");
	pButton->SetPressedAssetPath(L"Textures/UI/ButtonPressed.png");
	pButton->SetSelectedColor({ .95f, .95f, .95f, 1.f });
	pButton->SetPressedColor({ .92f, .92f, .92f, 1.f });
	AddChild(m_pStartButton);

	// PANEL
	m_pBackgroundPanel = new GameObject();
	m_pBackgroundPanel->AddComponent(new SpriteComponent(L"Textures/UI/Panel.png", {0.f, 1.f}));
	m_pBackgroundPanel->GetTransform()->Translate(20.f, m_SceneContext.windowHeight - 20.f, 0.1f);
	m_pBackgroundPanel->GetTransform()->Scale(4.f, 4.f, 1.f);
	AddChild(m_pBackgroundPanel);

	// 3D SCENE
	// CAMERA
	m_pFixedCamera = AddChild(new FixedCamera());
	m_pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);
	m_pFixedCamera->GetTransform()->Translate(8.f, 5.f, -15.f);
	m_pFixedCamera->GetTransform()->Rotate(15.f, -45.f, 0.f);

	// ENVIRONMENT
	auto* pPlane = new CubePrefab({ 10000.f, 0.5f, 10000.f }, XMFLOAT4{ 197.f / 255.f, 139.f / 255.f, 189 / 255.f, 1.f });
	AddChild(pPlane);

	auto* pVehicle = new CubePrefab({ 2.1f, 2.f, 6.f }, XMFLOAT4{Colors::BlueViolet});
	pVehicle->GetTransform()->Translate(0.f, 1.f, 0.f);
	AddChild(pVehicle);
}

void VO_MenuScene::Draw()
{
	
}

void VO_MenuScene::OnGUI()
{
}

void VO_MenuScene::LoadGame()
{
	SceneManager::Get()->NextScene();
}