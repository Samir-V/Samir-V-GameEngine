#include "FPSComponent.h"

#include <format>
#include <utility>

dae::FPSComponent::FPSComponent(std::shared_ptr<TextComponent> textComponent)
{
	m_AssignedTextComponent = std::move(textComponent);
}

void dae::FPSComponent::Update(float elapsedSec)
{
	float fps = CalculateFPS(elapsedSec);

	m_AssignedTextComponent->SetText(std::format("{:.1f} FPS", fps));
}

void dae::FPSComponent::FixedUpdate(float fixedTimeStep)
{
	fixedTimeStep;
}

void dae::FPSComponent::Render() const
{

}

void dae::FPSComponent::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::FPSComponent::SetOwner(std::weak_ptr<GameObject> owner)
{
	m_Owner = owner;
}

float dae::FPSComponent::CalculateFPS(float elapsedSec)
{
	return 1.0f / elapsedSec;
}

