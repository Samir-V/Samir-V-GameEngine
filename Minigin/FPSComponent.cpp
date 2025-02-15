#include "FPSComponent.h"
#include <format>

dae::FPSComponent::FPSComponent(TextComponent* textComponentPtr)
{
	m_AssignedTextComponentPtr = textComponentPtr;
}

void dae::FPSComponent::Update([[maybe_unused]] float elapsedSec)
{

}

void dae::FPSComponent::FixedUpdate([[maybe_unused]] float fixedTimeStep)
{

}

void dae::FPSComponent::LateUpdate([[maybe_unused]] float elapsedSec)
{
	if (m_AssignedTextComponentPtr != nullptr && m_AssignedTextComponentPtr->GetMarkedToDestroy())
	{
		m_AssignedTextComponentPtr = nullptr;
	}

	if (m_AssignedTextComponentPtr != nullptr)
	{
		float fps = CalculateFPS(elapsedSec);

		m_AssignedTextComponentPtr->SetText(std::format("{:.1f} FPS", fps));
	}
}

void dae::FPSComponent::Render() const
{

}

void dae::FPSComponent::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::FPSComponent::SetOwner(const GameObject* owner)
{
	m_OwnerPtr = owner;
}

float dae::FPSComponent::CalculateFPS(float elapsedSec)
{
	return 1.0f / elapsedSec;
}

void dae::FPSComponent::Destroy()
{
	m_MarkedToDestroy = true;
}

bool dae::FPSComponent::GetMarkedToDestroy() const
{
	return m_MarkedToDestroy;
}