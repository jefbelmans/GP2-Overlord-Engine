#pragma once
class EventSystem final : public Singleton<EventSystem>
{
public:
	EventSystem(const EventSystem& other) = delete;
	EventSystem(EventSystem&& other) noexcept = delete;
	EventSystem& operator=(const EventSystem& other) = delete;
	EventSystem& operator=(EventSystem&& other) noexcept = delete;

	void RegisterInteractable(IInteractable* pInteractable) { m_pInteractables.emplace_back(pInteractable); }
	void UnregisterInteractable(IInteractable* pInteractable)
	{
		const auto it = std::ranges::find(m_pInteractables, pInteractable);
		if (it != m_pInteractables.end())
		{
			m_pInteractables.erase(it);
			if (m_pSelectedInteractable == pInteractable)
			{
				m_pSelectedInteractable->OnHoverEnd();
				m_pSelectedInteractable->OnClickEnd();
				m_pSelectedInteractable = nullptr;
			}
				
		}
	}

	void SetSelectedInteractable(IInteractable* pInteractable)
	{
		if (m_pSelectedInteractable != nullptr)
		{
			m_pSelectedInteractable->OnHoverEnd();
			m_pSelectedInteractable->OnClickEnd();
		}
		
		m_pSelectedInteractable = pInteractable;
	}

protected:
	void Initialize() override;

private:
	friend class OverlordGame;
	friend class Singleton<EventSystem>;

	EventSystem() = default;
	~EventSystem() = default;

	void Update();

	// MOUSE
	void HandleMouseClick(int button);
	void HandleMouseRelease(int button);
	void HandleMouseHover(const XMFLOAT2& mousePos);
	
	// KEYBOARD + CONTROLLER
	int m_SelectedIndex{ -1 };
	void HandleNavigation();

	std::vector<IInteractable*> m_pInteractables{};
	IInteractable* m_pSelectedInteractable{ nullptr };

	// SOUND
	FMOD::Channel* m_pUIChannel{ nullptr };
	FMOD::Sound* m_pClickSound{ nullptr };
	FMOD::Sound* m_pSelectSound{ nullptr };

	void InitializeSound();
};