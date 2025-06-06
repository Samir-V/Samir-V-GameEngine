#include "ScoreComponent.h"
#include <string>

#include "BurgerPartComponent.h"
#include "EnemyComponent.h"
#include "GameHandlerComponent.h"
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
	if (auto scene = SceneManager::GetInstance().GetActiveScene())
	{
		auto burgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));

		for (auto burgerPart : burgerParts)
		{
			auto bPartComponent = burgerPart->GetComponent<BurgerPartComponent>();
			bPartComponent->GetBurgerPartCollisionEvent()->AddObserver(this);
		}
	}

	if (auto sceneDontDestroyOnLoad = SceneManager::GetInstance().GetDontDestroyOnLoadScene())
	{
		auto gameHandler = sceneDontDestroyOnLoad->GetGameObjectsWithTag(make_sdbm_hash("GameHandler"));

		gameHandler.front()->GetComponent<GameHandlerComponent>()->GetEnemiesSpawnedEvent()->AddObserver(this);
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

void dae::ScoreComponent::Notify(const Event& event, GameObject*)
{
	if (event.id == make_sdbm_hash("EnemiesSpawned"))
	{
		auto scene = SceneManager::GetInstance().GetActiveScene();

		auto enemies = scene->GetGameObjectsWithTag(make_sdbm_hash("Enemy"));

		for (auto enemy : enemies)
		{
			auto enemyComponent = enemy->GetComponent<EnemyComponent>();
			enemyComponent->GetEnemyDyingEvent()->AddObserver(this);
		}
	}

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

	if (event.id == make_sdbm_hash("PickleKilled"))
	{
		m_CurrentScore += 200;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}

	if (event.id == make_sdbm_hash("EggKilled"))
	{
		m_CurrentScore += 300;
		m_ScoreDisplay->SetText("Score: " + std::to_string(m_CurrentScore));
	}
}