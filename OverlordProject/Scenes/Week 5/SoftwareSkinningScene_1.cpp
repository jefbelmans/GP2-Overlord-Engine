#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"
#include "Managers/MaterialManager.h"

SoftwareSkinningScene_1::~SoftwareSkinningScene_1()
{
	delete[] m_RotBone0;
	delete[] m_RotBone1;
}

void SoftwareSkinningScene_1::Initialize()
{
	// SCENE SETTINGS
	m_SceneContext.settings.enableOnGUI = true;

	auto pMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pMaterial->SetColor(XMFLOAT4(.7f, .7f, .7f, 1.f));

	auto pRoot = new GameObject();
	AddChild(pRoot);

	m_pBone0 = new BoneObject(pMaterial, 15.f);
	pRoot->AddChild(m_pBone0);

	m_pBone1 = new BoneObject(pMaterial, 15.f);
	m_pBone0->AddBone(m_pBone1);
}

void SoftwareSkinningScene_1::Update()
{
	if (m_AutoRotation)
	{
		float rot = m_RotationSign * 45.f * m_SceneContext.pGameTime->GetElapsed();
		m_RotBone0[2] += rot;
		m_RotBone1[2] -= rot * 2.f;

		if (std::abs(m_RotBone0[2]) >= 45.f)
			m_RotationSign *= -1;
	}
	

	m_pBone0->GetTransform()->Rotate(m_RotBone0[0], m_RotBone0[1], m_RotBone0[2]);
	m_pBone1->GetTransform()->Rotate(m_RotBone1[0], m_RotBone1[1], m_RotBone1[2]);
}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::Text("Manual Rotation");

	ImGui::InputFloat3("Bone 0", m_RotBone0);
	ImGui::InputFloat3("Bone 1", m_RotBone1);
	ImGui::Checkbox("Automatic Rotation", &m_AutoRotation);
}