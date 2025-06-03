#pragma once
#include "GameObject.h"

namespace dae
{
	class BurgerPartComponent;
}

namespace dae
{
	class GameState
	{
	public:
		virtual ~GameState() = default;
		virtual void OnEnter(GameObject*) {}
		virtual std::unique_ptr<GameState> Update(GameObject*, float) { return nullptr; }
		virtual void OnExit(GameObject*) {}
	};

	class MenuState final : public GameState
	{
		
	};

	class PauseState final : public GameState
	{
		
	};

	class PlayingState final : public GameState
	{
	public:

		void OnEnter(GameObject* gameHandlerObject) override;
		std::unique_ptr<GameState> Update(GameObject* gameHandlerObject, float elapsedSec) override;
		void OnExit(GameObject* gameHandlerObject) override;

	private:

		std::vector<BurgerPartComponent*> m_BurgerPartComponents;
	};

	class GameWinningState final : public GameState
	{
	public:

		void OnEnter(GameObject* gameHandlerObject) override;
		std::unique_ptr<GameState> Update(GameObject* gameHandlerObject, float elapsedSec) override;
		void OnExit(GameObject* gameHandlerObject) override;

	private:
	};

}
