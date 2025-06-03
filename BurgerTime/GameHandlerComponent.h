#pragma once
#include <memory>
#include <string>
#include <vector>

#include "GameStates.h"

namespace dae
{
	class GameObject;

	class GameHandlerComponent final : public ComponentBase
	{
	public:
		enum class GameMode
		{
			Solo,
			Coop,
			Versus
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

		void SetLocalPosition(float x, float y) override;

		void SwitchLevel(const std::string& name);
		void SetGameMode(GameMode gameMode);
		void ChangeState(std::unique_ptr<GameState> newState);

		const std::vector<GameObject*>& GetBurgerParts() const;
		const std::vector<GameObject*>& GetPlayers() const;
		const std::vector<GameObject*>& GetEnemies() const;
		const std::vector<GameObject*>& GetEnemyRespawnPoints() const;

	private:

		static bool m_IsCreated;

		Transform m_LocalTransform{};

		GameMode m_GameMode{ GameMode::Solo };

		std::vector<GameObject*> m_Players{};
		std::vector<GameObject*> m_Enemies{};
		std::vector<GameObject*> m_EnemyRespawnPoints{};
		std::vector<GameObject*> m_BurgerParts{};

		std::unique_ptr<GameState> m_State{};
	};
}

