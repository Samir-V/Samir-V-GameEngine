#include "FPSComponent.h"
#include <format>

dae::FPSComponent::FPSComponent(GameObject* ownerPtr, TextComponent* textComponentPtr): ComponentBase(ownerPtr)
{
	m_AssignedTextComponentPtr = textComponentPtr;
}

void dae::FPSComponent::Start()
{
	
}


void dae::FPSComponent::Update(float elapsedSec)
{
	assert(m_AssignedTextComponentPtr && "m_AssignedTextComponentPtr is null!");

	float fps = CalculateFPS(elapsedSec);

	m_AssignedTextComponentPtr->SetText(std::format("{:.1f} FPS", fps));
}

void dae::FPSComponent::LateUpdate(float)
{
}

void dae::FPSComponent::Render() const
{
}

float dae::FPSComponent::CalculateFPS(float elapsedSec)
{
	return 1.0f / elapsedSec;
}