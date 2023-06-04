#include "stdafx.h"
#include "EventSystem.h"

void EventSystem::Initialize()
{
	InitializeSound();
}

void EventSystem::Update()
{
	// CACHE MOUSE POS
	const auto& mousePosPoint = InputManager::GetMousePosition();
	const XMFLOAT2 mousePos = { static_cast<float>(mousePosPoint.x),static_cast<float>(mousePosPoint.y) };

	// REGISTER HOVER
	HandleMouseHover(mousePos);

	// REGISTER CLICKS
	if (InputManager::IsMouseButton(InputState::pressed, 1))
		HandleMouseClick(0);
	else if(InputManager::IsMouseButton(InputState::released, 1))
		HandleMouseRelease(0);

	// KEYBOARD + CONTROLLER
	HandleNavigation();
}

void EventSystem::HandleMouseClick(int /*button*/)
{
	if (m_pSelectedInteractable)
	{
		m_pSelectedInteractable->OnClickBegin();
		SoundManager::Get()->GetSystem()->playSound(m_pClickSound, nullptr, false, &m_pUIChannel);
	}
		
}

void EventSystem::HandleMouseRelease(int /*button*/)
{
	if (m_pSelectedInteractable)
		m_pSelectedInteractable->OnClickEnd();
}

void EventSystem::HandleMouseHover(const XMFLOAT2& mousePos)
{
	for (IInteractable* interactable : m_pInteractables)
	{
		if (!interactable->GetIsActive()) continue;

		const auto& interactablePos = interactable->GetPosition();
		const auto& interactableSize = interactable->GetScaledSize();

		if (mousePos.x >= interactablePos.x && mousePos.x <= interactablePos.x + interactableSize.x &&
			mousePos.y >= interactablePos.y && mousePos.y <= interactablePos.y + interactableSize.y)
		{
			// We are hovering over an interactable
			if (m_pSelectedInteractable != interactable)
			{
				if(m_pSelectedInteractable != nullptr)
					m_pSelectedInteractable->OnHoverEnd();

				m_pSelectedInteractable = interactable;
				m_pSelectedInteractable->OnHoverBegin();
				SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pUIChannel);
			}
			
			return;
		}
	}

	//  We are not hovering over any interactable
	if (m_pSelectedInteractable != nullptr && m_SelectedIndex == -1)
	{
		m_pSelectedInteractable->OnHoverEnd();
		m_pSelectedInteractable = nullptr;
	}
}

void EventSystem::HandleNavigation()
{
	if(m_pInteractables.empty()) return;

	if (InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_DPAD_DOWN) ||
		InputManager::IsKeyboardKey(InputState::pressed, VK_DOWN))
	{
		if(m_pSelectedInteractable != nullptr)
			m_pSelectedInteractable->OnHoverEnd();

		m_SelectedIndex = (m_SelectedIndex + 1) % m_pInteractables.size();
		m_pSelectedInteractable = m_pInteractables[m_SelectedIndex];
		m_pSelectedInteractable->OnHoverBegin();
		SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pUIChannel);
	}

	else if (InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_DPAD_UP) ||
		InputManager::IsKeyboardKey(InputState::pressed, VK_UP))
	{
		if (m_pSelectedInteractable != nullptr)
			m_pSelectedInteractable->OnHoverEnd();

		m_SelectedIndex = (m_SelectedIndex - 1);

		if(m_SelectedIndex < 0) 
			m_SelectedIndex = int(m_pInteractables.size() - 1);

		m_pSelectedInteractable = m_pInteractables[m_SelectedIndex];
		m_pSelectedInteractable->OnHoverBegin();
		SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pUIChannel);
	}

	else if (InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A) ||
		InputManager::IsKeyboardKey(InputState::pressed, VK_RETURN))
	{
		if (m_pSelectedInteractable != nullptr)
		{
			m_pSelectedInteractable->OnClickBegin();
			SoundManager::Get()->GetSystem()->playSound(m_pClickSound, nullptr, false, &m_pUIChannel);
		}	
	}

	else if (InputManager::IsGamepadButton(InputState::released, XINPUT_GAMEPAD_A) ||
		InputManager::IsKeyboardKey(InputState::released, VK_RETURN))
	{
		if (m_pSelectedInteractable != nullptr)
			m_pSelectedInteractable->OnClickEnd();
	}
}

void EventSystem::InitializeSound()
{
	auto pFmodSystem = SoundManager::Get()->GetSystem();
	auto result = pFmodSystem->createStream("Resources/Sounds/Confirm.wav", FMOD_LOOP_OFF, nullptr, &m_pClickSound);
	if (result != FMOD_OK)
	{
		std::wstringstream strstr;
		strstr << L"FMOD error! \n[" << result << L"] " << FMOD_ErrorString(result) << std::endl;
		Logger::LogError(strstr.str());
	}

	result = pFmodSystem->createStream("Resources/Sounds/Select.wav", FMOD_LOOP_OFF, nullptr, &m_pSelectSound);
	if (result != FMOD_OK)
	{
		std::wstringstream strstr;
		strstr << L"FMOD error! \n[" << result << L"] " << FMOD_ErrorString(result) << std::endl;
		Logger::LogError(strstr.str());
	}
}