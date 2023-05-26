#include "stdafx.h"
#include "EventSystem.h"

void EventSystem::Initialize()
{
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
}

void EventSystem::HandleMouseClick(int /*button*/)
{
	if(m_pSelectedInteractable)
		m_pSelectedInteractable->OnClickBegin();
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
			}
			
			return;
		}
	}

	//  We are not hovering over any interactable
	if (m_pSelectedInteractable != nullptr)
	{
		m_pSelectedInteractable->OnHoverEnd();
		m_pSelectedInteractable = nullptr;
	}
}
