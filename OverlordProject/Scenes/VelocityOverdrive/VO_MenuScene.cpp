#include "stdafx.h"
#include "VO_MenuScene.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

void VO_MenuScene::Initialize()
{
	// SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.clearColor = XMFLOAT4{ 112.f / 255.f, 139.f / 255.f, 196.f / 255.f, 1.f };
	m_SceneContext.pLights->SetDirectionalLight({ 10.f, 10.f, -10.f }, { .85f, -.74f, 0.34f });
	m_SceneContext.settings.showInfoOverlay = false;

	// UI
	// FONT
	m_pFontText = ContentManager::Load<SpriteFont>(L"SpriteFonts/LemonMilk_32.fnt");
	m_pFontTitle = ContentManager::Load<SpriteFont>(L"SpriteFonts/LemonMilk_96.fnt");

	// MAIN MENU
	// BANNER
	m_pBanner = AddChild(new GameObject);
	m_pBanner->AddComponent(new SpriteComponent(L"Textures/MenuBanner.png", { 0.5f, 1.f }));
	m_pBanner->GetTransform()->Translate(m_SceneContext.windowWidth * 0.5f, m_SceneContext.windowHeight, 0.f);

	// START BUTTON
	m_pStartButton = AddChild(new GameObject);
	auto pButton = m_pStartButton->AddComponent(new ButtonComponent(std::bind(&VO_MenuScene::LoadGame, this), L"Textures/UI/ButtonBase.png", { 30.f, m_SceneContext.windowHeight - 250.f }, { 2.7f, 2.7f }));
	pButton->SetSelectedAssetPath(L"Textures/UI/ButtonSelected.png");
	pButton->SetPressedAssetPath(L"Textures/UI/ButtonPressed.png");
	pButton->SetSelectedColor({ .95f, .95f, .95f, 1.f });
	pButton->SetPressedColor({ .92f, .92f, .92f, 1.f });

	// QUIT BUTTON
	m_pQuitButton = AddChild(new GameObject);
	pButton = m_pQuitButton->AddComponent(new ButtonComponent(std::bind(&VO_MenuScene::QuitGame, this), L"Textures/UI/ButtonBase.png", { 30.f, m_SceneContext.windowHeight - 150.f }, { 2.7f, 2.7f }));
	pButton->SetSelectedAssetPath(L"Textures/UI/ButtonSelected.png");
	pButton->SetPressedAssetPath(L"Textures/UI/ButtonPressed.png");
	pButton->SetSelectedColor({ .95f, .95f, .95f, 1.f });
	pButton->SetPressedColor({ .92f, .92f, .92f, 1.f });

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
	// GAME NAME
	TextRenderer::Get()->DrawText(m_pFontTitle, L"Velocity Overdrive", { 39.f,  39.f }, XMFLOAT4{ 0.77f, 0.33f, 0.22f, 0.85f });
	TextRenderer::Get()->DrawText(m_pFontTitle, L"Velocity Overdrive", { 36.f,  34.f }, XMFLOAT4{ 0.86f, 0.42f, 0.19f, 0.85f });
	TextRenderer::Get()->DrawText(m_pFontTitle, L"Velocity Overdrive", { 33.f, 30.f }, XMFLOAT4{ 0.95f, 0.51f, 0.16f, 1.f });

	// START TEXT
	TextRenderer::Get()->DrawText(m_pFontText, L"START GAME", { 42.5f, m_SceneContext.windowHeight - 225.f }, XMFLOAT4{ Colors::Orange });

	// QUIT TEXT
	TextRenderer::Get()->DrawText(m_pFontText, L"QUIT GAME", { 50.f, m_SceneContext.windowHeight - 125.f }, XMFLOAT4{ Colors::Orange });
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
