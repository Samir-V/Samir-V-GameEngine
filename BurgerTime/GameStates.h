#pragma once
#include "GameObject.h"

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
		
	};

	class GameWinningState final : public GameState
	{
		
	};

}
