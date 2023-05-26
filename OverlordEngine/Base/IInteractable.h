#pragma once

class IInteractable
{
public:
	IInteractable(const IInteractable& other) = delete;
	IInteractable(IInteractable&& other) noexcept = delete;
	IInteractable& operator=(const IInteractable& other) = delete;
	IInteractable& operator=(IInteractable&& other) noexcept = delete;

	// EVENTS
	virtual void OnClickBegin() = 0;
	virtual void OnClickEnd() = 0;
	virtual void OnHoverBegin() = 0;
	virtual void OnHoverEnd() = 0;

	XMFLOAT2 GetPosition() const { return m_Position; }
	XMFLOAT2 GetSize() const { return m_Size; }
	XMFLOAT2 GetScale() const { return m_Scale; }
	XMFLOAT2 GetScaledSize() const { return m_ScaledSize; }

	void SetScale(const XMFLOAT2& scale)
	{
		m_Scale = scale; 
		XMStoreFloat2(&m_ScaledSize, XMVectorMultiply(XMLoadFloat2(&m_Size), XMLoadFloat2(&m_Scale)));
	};

	void SetSize(const XMFLOAT2& size)
	{
		m_Size = size;
		XMStoreFloat2(&m_ScaledSize, XMVectorMultiply(XMLoadFloat2(&m_Size), XMLoadFloat2(&m_Scale)));
	};

protected:
	IInteractable(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT2& scale)
		: m_Position{ position }
		, m_Size{ size }
		, m_Scale{ scale }
	{
		XMStoreFloat2(&m_ScaledSize, XMVectorMultiply(XMLoadFloat2(&m_Size), XMLoadFloat2(&m_Scale)));
	}
	virtual ~IInteractable() = default;

private:
	XMFLOAT2 m_Position{};
	XMFLOAT2 m_Size{};
	XMFLOAT2 m_Scale{};
	XMFLOAT2 m_ScaledSize{};
};