#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "EnemyComponent.h"
#include "GameStates.h"
#include "IObserver.h"

namespace dae
{
	class GameObject;

	class GameHandlerComponent final : public ComponentBase, public IObserver
	{
	public:
		enum class GameMode
		{
			Solo,
			Coop,
			Versus
		};

		struct GameplayData
		{
			std::vector<GameObject*> players;
			std::vector<GameObject*> enemies;
			std::vector<GameObject*> enemyRespawnPoints;
			std::vector<GameObject*> burgerParts;

			std::unordered_map<GameObject*, float> enemyRespawnDelays;
		};

		GameHandlerComponent(GameObject* ownerPtr);
		~GameHandlerComponent() override = default;

		GameHandlerComponent(const GameHandlerComponent& other) = delete;
		GameHandlerComponent(GameHandlerComponent&& other) = delete;
		GameHandlerComponent& operator=(const GameHandlerComponent& other) = delete;
		GameHandlerComponent& operator=(GameHandlerComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Notify(const Event& event, GameObject* observedGameObject) override;

		void SwitchLevel(const std::string& name);
		void SetGameMode(GameMode gameMode);
		void ChangeState(std::unique_ptr<GameState> newState);

		GameplayData& GetGameplayDataRef();

		void SpawnEnemy(EnemyType enemyType);

	private:

		static bool m_IsCreated;

		GameMode m_GameMode{ GameMode::Solo };

		GameplayData m_GameplayData;

		std::unique_ptr<GameState> m_State{};
	};
}

