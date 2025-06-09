#include "GameStates.h"

#include <iostream>

#include "GameHandlerComponent.h"
#include "RectCollider2DComponent.h"
#include "BurgerPartComponent.h"
#include "EnemyComponent.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ServiceLocator.h"

void dae::PlayingState::OnEnter(GameObject* gameHandlerObject)
{
	m_GameHandlerComponentPtr = gameHandlerObject->GetComponent<GameHandlerComponent>();

	auto& burgerParts = m_GameHandlerComponentPtr->GetGameplayDataRef().burgerParts;

	for (auto burgerPart : burgerParts)
	{
		m_BurgerPartComponents.emplace_back(burgerPart->GetComponent<BurgerPartComponent>());
	}
}

std::unique_ptr<dae::GameState> dae::PlayingState::Update(GameObject*, float elapsedSec)
{
	bool allAssembled = std::ranges::all_of(m_BurgerPartComponents, [](const BurgerPartComponent* burgerPartComponent)
		{
			return burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Assembled;
		});

	if (allAssembled)
	{
		return std::make_unique<GameWinningState>();
	}

	auto& gameplayDataRef = m_GameHandlerComponentPtr->GetGameplayDataRef();
	auto& enemyRespawnPoints = gameplayDataRef.enemyRespawnPoints;

	for (auto it = gameplayDataRef.enemyRespawnDelays.begin(); it != gameplayDataRef.enemyRespawnDelays.end(); )
	{
		it->second -= elapsedSec;

		if (it->second <= 0.0f)
		{
			int idx = std::rand() % static_cast<int>(enemyRespawnPoints.size());
			auto& worldPos = enemyRespawnPoints[idx]->GetWorldTransform().GetPosition();
			it->first->SetWorldPosition(worldPos.x, worldPos.y);
			it->first->GetComponent<RectCollider2DComponent>()->SetLocalPosition(0.0f, 0.0f);
			it->first->GetComponent<EnemyComponent>()->Resurrect();
			it = gameplayDataRef.enemyRespawnDelays.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (!gameplayDataRef.pendingSpawns.empty())
	{
		m_TimeSinceLastSpawn += elapsedSec;

		if (m_TimeSinceLastSpawn >= 1.0f)
		{
			m_TimeSinceLastSpawn = 0.0f;

			EnemyType nextType = gameplayDataRef.pendingSpawns.front();
			gameplayDataRef.pendingSpawns.erase(gameplayDataRef.pendingSpawns.begin());

			m_GameHandlerComponentPtr->SpawnEnemy(nextType);
		}
	}

	return nullptr;
}

void dae::PlayingState::OnExit(GameObject* )
{
	std::cout << "Exiting playing state" << "\n";
}


void dae::GameWinningState::OnEnter(GameObject* gameHandlerObject)
{
	auto& sound = ServiceLocator::GetSoundSystem();
	sound.StopMusic();
	sound.Play("RoundClear.wav", 0.8f);

	auto gameHandlerComp = gameHandlerObject->GetComponent<GameHandlerComponent>();

	auto& players = gameHandlerComp->GetGameplayDataRef().players;

	for (auto player : players)
	{
		player->GetComponent<PeterPepperComponent>()->AssertVictory();
	}

	auto& enemies = gameHandlerComp->GetGameplayDataRef().enemies;

	for (auto enemy : enemies)
	{
		enemy->SetRunsUpdate(false);
	}
}

std::unique_ptr<dae::GameState> dae::GameWinningState::Update(GameObject*, float elapsedSec)
{
	if (m_Timer < 0.0f)
	{
		return std::make_unique<PlayingState>();
	}

	m_Timer -= elapsedSec;

	return nullptr;
}

void dae::GameWinningState::OnExit(GameObject* gameHandlerObject)
{
	auto gameHandlerComp = gameHandlerObject->GetComponent<GameHandlerComponent>();
	auto& players = gameHandlerComp->GetGameplayDataRef().players;

	for (auto player : players)
	{
		player->SetIsActive(false);
	}

	int currentLevelCounter = gameHandlerComp->GetLevelCounter();
	++currentLevelCounter;

	if (currentLevelCounter > 3)
	{
		currentLevelCounter = 1;
	}

	gameHandlerComp->SwitchLevel("Level" + std::to_string(currentLevelCounter));
}





