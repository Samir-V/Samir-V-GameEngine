#include "GameHandlerComponent.h"

#include <stdexcept>

#include <SDL.h>
#include "InputManager.h"
#include "Scene.h"
#include "SceneManager.h"

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
		ChangeState(std::make_unique<MenuState>());
		input.SetActiveInputMap(make_sdbm_hash("MenuMap"));
	}
	else
	{
		m_Players = scene->GetGameObjectsWithTag(make_sdbm_hash("Player"));
		m_Enemies = scene->GetGameObjectsWithTag(make_sdbm_hash("Enemy"));
		m_BurgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));
		//m_EnemyRespawnPoints = scene->GetGameObjectsWithTag(make_sdbm_hash("Player"));
		ChangeState(std::make_unique<PlayingState>());
		input.SetActiveInputMap(make_sdbm_hash("GameplayMap"));
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

const std::vector<dae::GameObject*>& dae::GameHandlerComponent::GetBurgerParts() const
{
	return m_BurgerParts;
}

const std::vector<dae::GameObject*>& dae::GameHandlerComponent::GetEnemies() const
{
	return m_Enemies;
}

const std::vector<dae::GameObject*>& dae::GameHandlerComponent::GetEnemyRespawnPoints() const
{
	return m_EnemyRespawnPoints;
}

const std::vector<dae::GameObject*>& dae::GameHandlerComponent::GetPlayers() const
{
	return m_Players;
}



