#include "ScoreComponent.h"
#include <string>

#include "BurgerPartComponent.h"
#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

dae::ScoreComponent::ScoreComponent(GameObject* ownerPtr, TextComponent* textComponent) :
	ComponentBase(ownerPtr),
	m_ScoreDisplay(textComponent)
{

}

void dae::ScoreComponent::Start()
{
	auto scene = dae::SceneManager::GetInstance().GetSceneByName("MainScene");

	if (scene)
	{
		auto burgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));

		for (auto burgerPart : burgerParts)
		{
			auto bPartComponent = burgerPart->GetComponent<BurgerPartComponent>();
			bPartComponent->GetBurgerPartCollisionEvent()->AddObserver(this);
		}
	}
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
	/*if (event.id == make_sdbm_hash("SmallEnemyKilled"))
	{
		m_CurrentScore += 10;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("EnemyKilled"))
	{
		m_CurrentScore += 100;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}*/

	if (event.id == make_sdbm_hash("BurgerPartLanded"))
	{
		m_CurrentScore += 50;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("HotDogKilled"))
	{
		m_CurrentScore += 100;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("EggKilled"))
	{
		m_CurrentScore += 300;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("PickleKilled"))
	{
		m_CurrentScore += 200;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}
}