#include "GameHandlerComponent.h"
#include <stdexcept>

#include <SDL.h>

#include "EnemyComponent.h"
#include "EnemyMoveComponent.h"
#include "InputManager.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "SpritesheetComponent.h"
#include "RectCollider2DComponent.h"

bool dae::GameHandlerComponent::m_IsCreated = false;

dae::GameHandlerComponent::GameHandlerComponent(GameObject* ownerPtr): ComponentBase(ownerPtr), m_GameplayData{}, m_LevelCounter{}, m_EnemiesSpawnedEvent{std::make_unique<Subject>()}
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

		const auto sceneDontDestroy = SceneManager::GetInstance().GetDontDestroyOnLoadScene();

		sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("GameplayHUD")).front()->SetIsActive(false);
	}
	else
	{
		if (m_LevelCounter != 0)
		{
			// Here add pepper
		}

		++m_LevelCounter;

		if (m_LevelCounter > 3)
		{
			m_LevelCounter = 1;
		}

		const auto sceneDontDestroy = SceneManager::GetInstance().GetDontDestroyOnLoadScene();

		sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("GameplayHUD")).front()->SetIsActive(true);

		m_GameplayData.players = sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("Player"));
		m_GameplayData.burgerParts = scene->GetGameObjectsWithTag(make_sdbm_hash("BurgerPart"));
		m_GameplayData.enemyRespawnPoints = scene->GetGameObjectsWithTag(make_sdbm_hash("EnemyRespawnPoint"));

		m_GameplayData.playerRespawnPoints = scene->GetGameObjectsWithTag(make_sdbm_hash("PlayerRespawnPoint"));
		for (const auto player : m_GameplayData.players)
		{
			auto peterPepperComponent = player->GetComponent<PeterPepperComponent>();
			peterPepperComponent->GetPlayerDiedEvent()->AddObserver(this);
			peterPepperComponent->FullRespawn();
			player->GetComponent<MoveComponent>()->Reset();

			if (!m_GameplayData.playerRespawnPoints.empty())
			{
				const int idx = std::rand() % static_cast<int>(m_GameplayData.playerRespawnPoints.size());
				auto& worldPos = m_GameplayData.playerRespawnPoints[idx]->GetWorldTransform().GetPosition();
				player->SetWorldPosition(worldPos.x, worldPos.y);
			}

			player->SetIsActive(true);
		}

		auto pepperSprays = sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("PepperSpray"));

		for (auto pepperSpray : pepperSprays)
		{
			pepperSpray->SetIsActive(false);
		}

		SpawnLevelEnemies();

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
		constexpr float respawnDelay = 2.0f;

		m_GameplayData.enemyRespawnDelays.insert({ observedGameObject, respawnDelay });
	}

	if (event.id == make_sdbm_hash("PlayerDied"))
	{
		if (std::ranges::all_of(m_GameplayData.players, [](const GameObject* player)
		{
				return player->GetComponent<PeterPepperComponent>()->GetRemainingLives() == 0;
		}))
		{
			ChangeState(std::make_unique<MenuState>());
			SwitchLevel("Menu");
			return;
		}

		++m_GameplayData.deadPlayerAmount;

		if (m_GameplayData.deadPlayerAmount == static_cast<int>(m_GameplayData.players.size()))
		{
			ResetLevel();
		}
	}
}

void dae::GameHandlerComponent::SpawnEnemy(EnemyType enemyType)
{
	auto activeScene = SceneManager::GetInstance().GetActiveScene();

	auto go = std::make_unique<GameObject>();
	go->SetTag(make_sdbm_hash("Enemy"));
	go->AddComponent<RectCollider2DComponent>(16.0f, 16.0f)->SetShouldTriggerEvents(true);
	go->AddComponent<EnemyComponent>(enemyType)->GetEnemyDyingEvent()->AddObserver(this);
	go->AddComponent<EnemyMoveComponent>(m_GameplayData.players.front(), 30.0f);

	int idx = std::rand() % static_cast<int>(m_GameplayData.enemyRespawnPoints.size());
	auto& worldPos = m_GameplayData.enemyRespawnPoints[idx]->GetWorldTransform().GetPosition();
	go->SetWorldPosition(worldPos);

	switch (enemyType)
	{
	case EnemyType::HotDog:
		{
			auto spriteSheetCompHotDog = go->AddComponent<SpritesheetComponent>("Enemies");
			spriteSheetCompHotDog->AddSprite("HotDogWalkingLeft.png", make_sdbm_hash("HotDogWalkingLeft"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompHotDog->AddSprite("HotDogWalkingRight.png", make_sdbm_hash("HotDogWalkingRight"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompHotDog->AddSprite("HotDogWalkingDown.png", make_sdbm_hash("HotDogWalkingDown"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompHotDog->AddSprite("HotDogWalkingUp.png", make_sdbm_hash("HotDogWalkingUp"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompHotDog->AddSprite("HotDogStunned.png", make_sdbm_hash("HotDogStunned"), SpritesheetComponent::SpriteMetaData(2, 0, 0.2f));
			spriteSheetCompHotDog->AddSprite("HotDogDying.png", make_sdbm_hash("HotDogDying"), SpritesheetComponent::SpriteMetaData(4, 0, 0.5f));
			spriteSheetCompHotDog->Play(make_sdbm_hash("HotDogWalkingLeft"));
			break;
		}
	case EnemyType::Pickle:
		{
			auto spriteSheetCompPickle = go->AddComponent<SpritesheetComponent>("Enemies");
			spriteSheetCompPickle->AddSprite("PickleWalkingLeft.png", make_sdbm_hash("PickleWalkingLeft"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompPickle->AddSprite("PickleWalkingRight.png", make_sdbm_hash("PickleWalkingRight"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompPickle->AddSprite("PickleWalkingDown.png", make_sdbm_hash("PickleWalkingDown"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompPickle->AddSprite("PickleWalkingUp.png", make_sdbm_hash("PickleWalkingUp"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompPickle->AddSprite("PickleStunned.png", make_sdbm_hash("PickleStunned"), SpritesheetComponent::SpriteMetaData(2, 0, 0.2f));
			spriteSheetCompPickle->AddSprite("PickleDying.png", make_sdbm_hash("PickleDying"), SpritesheetComponent::SpriteMetaData(4, 0, 0.5f));
			spriteSheetCompPickle->Play(make_sdbm_hash("PickleWalkingLeft"));
			break;
		}
	case EnemyType::Egg:
		{
			auto spriteSheetCompEgg = go->AddComponent<SpritesheetComponent>("Enemies");
			spriteSheetCompEgg->AddSprite("EggWalkingLeft.png", make_sdbm_hash("EggWalkingLeft"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompEgg->AddSprite("EggWalkingRight.png", make_sdbm_hash("EggWalkingRight"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompEgg->AddSprite("EggWalkingDown.png", make_sdbm_hash("EggWalkingDown"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompEgg->AddSprite("EggWalkingUp.png", make_sdbm_hash("EggWalkingUp"), SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
			spriteSheetCompEgg->AddSprite("EggStunned.png", make_sdbm_hash("EggStunned"), SpritesheetComponent::SpriteMetaData(2, 0, 0.2f));
			spriteSheetCompEgg->AddSprite("EggDying.png", make_sdbm_hash("EggDying"), SpritesheetComponent::SpriteMetaData(4, 0, 0.5f));
			spriteSheetCompEgg->Play(make_sdbm_hash("EggWalkingLeft"));
			break;
		}
	}

	go->Start();

	m_EnemiesSpawnedEvent->NotifyObservers(Event(make_sdbm_hash("EnemySpawned")), go.get());

	m_GameplayData.enemies.push_back(go.get());

	activeScene->Add(std::move(go));
}


void dae::GameHandlerComponent::ResetLevel()
{
	for (auto enemy : m_GameplayData.enemies)
	{
		enemy->Destroy();
	}

	m_GameplayData.enemies.clear();
	m_GameplayData.enemyRespawnDelays.clear();
	m_GameplayData.pendingSpawns.clear();

	for (const auto player : m_GameplayData.players)
	{
		auto peterPepperComponent = player->GetComponent<PeterPepperComponent>();
		peterPepperComponent->Resurrect();
		player->GetComponent<MoveComponent>()->Reset();
		const int idx = std::rand() % static_cast<int>(m_GameplayData.playerRespawnPoints.size());
		auto& worldPos = m_GameplayData.playerRespawnPoints[idx]->GetWorldTransform().GetPosition();
		player->SetWorldPosition(worldPos.x, worldPos.y);
		player->SetIsActive(true);
	}

	auto pepperSprays = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("PepperSpray"));

	for (auto pepperSpray : pepperSprays)
	{
		pepperSpray->SetIsActive(false);
	}

	SpawnLevelEnemies();

	m_GameplayData.deadPlayerAmount = 0;
}

void dae::GameHandlerComponent::AddEnemySpawnPattern(EnemySpawnPattern enemySpawnPattern)
{
	m_EnemySpawnPatterns.push_back(enemySpawnPattern);
}

void dae::GameHandlerComponent::SpawnLevelEnemies()
{
	if (m_GameplayData.enemyRespawnPoints.empty())
	{
		return;
	}

	auto [hotDogsToSpawn, picklesToSpawn, eggsToSpawn] = m_EnemySpawnPatterns[m_LevelCounter - 1];

	for (int counter{}; counter < hotDogsToSpawn; ++counter)
	{
		m_GameplayData.pendingSpawns.push_back(EnemyType::HotDog);
	}

	for (int counter{}; counter < picklesToSpawn; ++counter)
	{
		m_GameplayData.pendingSpawns.push_back(EnemyType::Pickle);
	}

	for (int counter{}; counter < eggsToSpawn; ++counter)
	{
		m_GameplayData.pendingSpawns.push_back(EnemyType::Egg);
	}

	//m_EnemiesSpawnedEvent->NotifyObservers(Event(make_sdbm_hash("EnemiesSpawned")), GetOwner());
}

dae::Subject* dae::GameHandlerComponent::GetEnemiesSpawnedEvent() const
{
	return m_EnemiesSpawnedEvent.get();
}
