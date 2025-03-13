#include "ScoreDisplayComponent.h"
#include <string>

#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "TextComponent.h"

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent) :
	ComponentBase(ownerPtr),
	m_ScoreDisplay(textComponent)
{

}

void dae::ScoreDisplayComponent::Update(float)
{

}

void dae::ScoreDisplayComponent::LateUpdate(float)
{

}

void dae::ScoreDisplayComponent::Render() const
{

}

void dae::ScoreDisplayComponent::RenderImGui()
{

}

void dae::ScoreDisplayComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::ScoreDisplayComponent::Notify(EventType event, GameObject* observedGameObject)
{
	switch (event)
	{
	case EventType::ScoreChanged:

		m_ScoreDisplay->SetText("Score: " + std::to_string(observedGameObject->GetComponent<PeterPepperComponent>()->GetScore()));

		break;
	case EventType::SubjectDestroyed:

		// Will handle destruction of the subject

		break;
	}
}