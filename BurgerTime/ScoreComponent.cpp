#include "ScoreComponent.h"
#include <string>

#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "TextComponent.h"

dae::ScoreComponent::ScoreComponent(GameObject* ownerPtr, TextComponent* textComponent) :
	ComponentBase(ownerPtr),
	m_ScoreDisplay(textComponent)
{

}

void dae::ScoreComponent::Update(float)
{

}

void dae::ScoreComponent::LateUpdate(float)
{

}

void dae::ScoreComponent::Render() const
{

}

void dae::ScoreComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::ScoreComponent::Notify(const Event& event, GameObject*)
{
	if (event.id == make_sdbm_hash("SmallEnemyKilled"))
	{
		m_CurrentScore += 10;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("EnemyKilled"))
	{
		m_CurrentScore += 100;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}
}