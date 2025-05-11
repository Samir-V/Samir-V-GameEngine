#pragma once
#include "GameObject.h"

namespace dae
{
	class EnemyState
	{
	public:
		virtual ~EnemyState() = default;
		virtual void OnEnter(GameObject*) {}
		virtual std::unique_ptr<EnemyState> Update(GameObject*, float) { return nullptr; }
		virtual void OnExit(GameObject*) {}
	};

	class WalkingState final : public EnemyState
	{
		// Regular state
	};

	class ClimbingState final : public EnemyState
	{
		
	};

	class FinishedClimbingState final : public EnemyState
	{
		
	};

	class StunnedState final : public EnemyState
	{
		// State when enemy was sprayed
	};

	class FallingState final : public EnemyState
	{
		// State when enemy was on the burger part and falls with it
	};

	class DyingState final : public EnemyState
	{
		// Dying state
	};

}