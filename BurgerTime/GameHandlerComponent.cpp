#include "GameHandlerComponent.h"

#include <stdexcept>

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

void dae::GameHandlerComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::GameHandlerComponent::SwitchLevel(const std::string& name)
{
	SceneManager::GetInstance().SetActiveScene(name);

	const auto scene = SceneManager::GetInstance().GetActiveScene();

	m_Players = scene->GetGameObjectsWithTag(make_sdbm_hash("Player"));
	m_Enemies = scene->GetGameObjectsWithTag(make_sdbm_hash("Enemy"));
	m_BurgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));
	//m_EnemyRespawnPoints = scene->GetGameObjectsWithTag(make_sdbm_hash("Player"));

	if (name == "Menu")
	{
		ChangeState(std::make_unique<MenuState>());
	}
	else
	{
		ChangeState(std::make_unique<PlayingState>());
	}
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