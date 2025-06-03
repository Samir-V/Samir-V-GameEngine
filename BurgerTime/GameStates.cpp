#include "GameStates.h"

#include "GameHandlerComponent.h"
#include "BurgerPartComponent.h"

void dae::PlayingState::OnEnter(GameObject* gameHandlerObject)
{
	auto gameHandlerComp = gameHandlerObject->GetComponent<GameHandlerComponent>();

	auto burgerParts = gameHandlerComp->GetBurgerParts();

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


void dae::GameWinningState::OnEnter(GameObject* )
{
	
}

std::unique_ptr<dae::GameState> dae::GameWinningState::Update(GameObject* , float )
{

	return nullptr;
}

void dae::GameWinningState::OnExit(GameObject* )
{
	
}





