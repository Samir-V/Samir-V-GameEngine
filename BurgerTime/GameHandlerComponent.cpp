#include "GameHandlerComponent.h"
#include <stdexcept>

#include <SDL.h>

#include "EnemyComponent.h"
#include "InputManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ServiceLocator.h"

bool dae::GameHandlerComponent::m_IsCreated = false;

dae::GameHandlerComponent::GameHandlerComponent(GameObject* ownerPtr): ComponentBase(ownerPtr)
{
	if (!m_IsCreated)
	{
		m_IsCreated = true;
	}
	else
	{
		throw std::runtime_error("GameHandlerComponent was created already");
	}

	m_State = std::make_unique<MenuState>();
}

void dae::GameHandlerComponent::Start()
{
	const auto scene = SceneManager::GetInstance().GetActiveScene();
	auto& input = InputManager::GetInstance();

	if (scene->GetName() == "Menu")
	{
		input.SetActiveInputMap(make_sdbm_hash("MenuMap"));
	}
	else
	{
		m_GameplayData.players = scene->GetGameObjectsWithTag(make_sdbm_hash("Player"));
		m_GameplayData.enemies = scene->GetGameObjectsWithTag(make_sdbm_hash("Enemy"));
		m_GameplayData.burgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));
		m_GameplayData.enemyRespawnPoints = scene->GetGameObjectsWithTag(make_sdbm_hash("EnemyRespawnPoint"));

		for (auto enemy : m_GameplayData.enemies)
		{
			enemy->GetComponent<EnemyComponent>()->GetEnemyDyingEvent()->AddObserver(this);
		}

		ChangeState(std::make_unique<PlayingState>());
		input.SetActiveInputMap(make_sdbm_hash("GameplayMap"));

		auto& sound = ServiceLocator::GetSoundSystem();

		sound.Play("MainTheme.mp3", 0.8f, true);
	}
}

void dae::GameHandlerComponent::Render() const
{
}

void dae::GameHandlerComponent::Update(float elapsedSec)
{
	auto newState = m_State->Update(GetOwner(), elapsedSec);

	if (newState != nullptr)
	{
		ChangeState(std::move(newState));
	}
}

void dae::GameHandlerComponent::LateUpdate(float)
{
}

void dae::GameHandlerComponent::SwitchLevel(const std::string& name)
{
	SceneManager::GetInstance().SetActiveScene(name);
}

void dae::GameHandlerComponent::SetGameMode(GameMode gameMode)
{
	m_GameMode = gameMode;
}

void dae::GameHandlerComponent::ChangeState(std::unique_ptr<GameState> newState)
{
	if (newState && typeid(*m_State) == typeid(*newState))
	{
		return;
	}

	m_State->OnExit(GetOwner());

	m_State = std::move(newState);

	m_State->OnEnter(GetOwner());
}

dae::GameHandlerComponent::GameplayData& dae::GameHandlerComponent::GetGameplayDataRef()
{
	return m_GameplayData;
}

void dae::GameHandlerComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("EnemyDied"))
	{
		constexpr float respawnDelay = 5.0f;

		m_GameplayData.enemyRespawnDelays.insert({ observedGameObject, respawnDelay });
	}
}



