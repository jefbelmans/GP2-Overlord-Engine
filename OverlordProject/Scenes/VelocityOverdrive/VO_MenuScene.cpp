#include "stdafx.h"
#include "VO_MenuScene.h"
#include "Prefabs/CubePrefab.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

void VO_MenuScene::Initialize()
{
	// SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.clearColor = XMFLOAT4{ 112.f / 255.f, 139.f / 255.f, 196.f / 255.f, 1.f };
	m_SceneContext.pLights->SetDirectionalLight({ 10.f, 10.f, -10.f }, { .85f, -.74f, 0.34f });

	// UI
	// FONT
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/LemonMilk_32.fnt");

	// MAIN MENU
	// START BUTTON
	m_pStartButton = new GameObject();
	auto pButton = m_pStartButton->AddComponent(new ButtonComponent(std::bind(&VO_MenuScene::LoadGame, this), L"Textures/UI/ButtonBase.png", { 40.f, m_SceneContext.windowHeight - 230.f }, { 2.7f, 2.7f }));
	pButton->SetSelectedAssetPath(L"Textures/UI/ButtonSelected.png");
	pButton->SetPressedAssetPath(L"Textures/UI/ButtonPressed.png");
	pButton->SetSelectedColor({ .95f, .95f, .95f, 1.f });
	pButton->SetPressedColor({ .92f, .92f, .92f, 1.f });
	AddChild(m_pStartButton);

	// QUIT BUTTON
	m_pQuitButton = new GameObject();
	pButton = m_pQuitButton->AddComponent(new ButtonComponent(std::bind(&VO_MenuScene::QuitGame, this), L"Textures/UI/ButtonBase.png", { 40.f, m_SceneContext.windowHeight - 130.f }, { 2.7f, 2.7f }));
	pButton->SetSelectedAssetPath(L"Textures/UI/ButtonSelected.png");
	pButton->SetPressedAssetPath(L"Textures/UI/ButtonPressed.png");
	pButton->SetSelectedColor({ .95f, .95f, .95f, 1.f });
	pButton->SetPressedColor({ .92f, .92f, .92f, 1.f });
	AddChild(m_pQuitButton);

	// PANEL
	m_pBackgroundPanel = new GameObject();
	m_pBackgroundPanel->AddComponent(new SpriteComponent(L"Textures/UI/Panel.png", {0.f, 1.f}));
	m_pBackgroundPanel->GetTransform()->Translate(20.f, m_SceneContext.windowHeight - 20.f, 0.1f);
	m_pBackgroundPanel->GetTransform()->Scale(2.25f, 3.f, 1.f);
	// AddChild(m_pBackgroundPanel);

	// 3D SCENE
	// CAMERA
	m_pFixedCamera = AddChild(new FixedCamera());
	m_pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);
	m_pFixedCamera->GetTransform()->Translate(-0.3f, 1.f, -4.2f);
	m_pFixedCamera->GetTransform()->Rotate(-4.f, 0.f, 0.f);

	// MATERIALS
	auto pGroundMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMat->SetDiffuseColor({0.53f, 0.27f, 0.89f, 1.f});
	pGroundMat->UseDiffuseMap(false);

	const auto pVehicleMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pVehicleMat->SetDiffuseTexture(L"Textures/F1_Car.png");

	const auto pCharacterMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pCharacterMat->SetDiffuseTexture(L"Textures/Character_Diffuse.png");

	// BACKGROUND
	auto pBackground = AddChild(new GameObject);
	auto pModel = pBackground->AddComponent(new ModelComponent(L"Meshes/MenuBackground.ovm"));
	pModel->SetMaterial(pGroundMat);

	// VEHICLE
	auto pVehicle = AddChild(new GameObject);
	pModel = pVehicle->AddComponent(new ModelComponent(L"Meshes/F1_Car_Full.ovm"));
	pModel->SetMaterial(pVehicleMat);

	// CHARACTER
	auto pCharacter = AddChild(new GameObject);
	pCharacter->GetTransform()->Translate(1.8f, 0.f, 0.5f);
	pCharacter->GetTransform()->Rotate(0.f, 30.f, 0.f);
	pCharacter->GetTransform()->Scale(0.013f);

	pModel = pCharacter->AddComponent(new ModelComponent(L"Meshes/Character.ovm"));
	pModel->SetMaterial(pCharacterMat);

	// ANIMATION
	auto pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(2);
	pAnimator->Play();
}

void VO_MenuScene::Draw()
{
	// START TEXT
	TextRenderer::Get()->DrawText(m_pFont, L"START GAME", { 52.5f, m_SceneContext.windowHeight - 205.f }, XMFLOAT4{ Colors::Orange });

	// QUIT TEXT
	TextRenderer::Get()->DrawText(m_pFont, L"QUIT GAME", { 60.f, m_SceneContext.windowHeight - 105.f }, XMFLOAT4{ Colors::Orange });
}

void VO_MenuScene::OnGUI()
{
}

void VO_MenuScene::OnSceneActivated()
{
	ShadowMapRenderer::Get()->SetViewWidthHeight(30.f, 30.f);
}

void VO_MenuScene::LoadGame()
{
	SceneManager::Get()->NextScene();
}

void VO_MenuScene::QuitGame()
{
	ExitProcess(0);
}
