#include "GameStates.h"

#include <iostream>

#include "GameHandlerComponent.h"
#include "BurgerPartComponent.h"
#include "PeterPepperComponent.h"
#include "ServiceLocator.h"

void dae::PlayingState::OnEnter(GameObject* gameHandlerObject)
{
	auto gameHandlerComp = gameHandlerObject->GetComponent<GameHandlerComponent>();

	auto& burgerParts = gameHandlerComp->GetBurgerParts();

	for (auto burgerPart : burgerParts)
	{
		m_BurgerPartComponents.emplace_back(burgerPart->GetComponent<BurgerPartComponent>());
	}
}

std::unique_ptr<dae::GameState> dae::PlayingState::Update(GameObject* , float)
{
	bool allAssembled = std::ranges::all_of(m_BurgerPartComponents, [](const BurgerPartComponent* burgerPartComponent)
		{
			return burgerPartComponent->GetBurgerPartState() == BurgerPartComponent::BurgerPartState::Assembled;
		});

	if (allAssembled)
	{
		return std::make_unique<GameWinningState>();
	}

	return nullptr;
}

void dae::PlayingState::OnExit(GameObject* )
{
}


void dae::GameWinningState::OnEnter(GameObject* gameHandlerObject)
{
	auto& sound = ServiceLocator::GetSoundSystem();
	sound.StopMusic();
	sound.Play("RoundClear.wav", 0.8f);

	auto& players = gameHandlerObject->GetComponent<GameHandlerComponent>()->GetPlayers();

	for (auto player : players)
	{
		player->GetComponent<PeterPepperComponent>()->AssertVictory();
	}
}

std::unique_ptr<dae::GameState> dae::GameWinningState::Update(GameObject*, float elapsedSec)
{
	if (m_Timer < 0.0f)
	{
		std::cout << "Last update of the game win." << "\n";
		return std::make_unique<MenuState>();
	}

	m_Timer -= elapsedSec;

	return nullptr;
}

void dae::GameWinningState::OnExit(GameObject* gameHandlerObject)
{
	auto gameHandlerComp = gameHandlerObject->GetComponent<GameHandlerComponent>();
	std::cout << "Switching to menu" << "\n";
	gameHandlerComp->SwitchLevel("Menu");
}





