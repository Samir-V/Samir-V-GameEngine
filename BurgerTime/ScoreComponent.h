#pragma once
#include "ComponentBase.h"
#include "IObserver.h"

namespace dae
{
	class TextComponent;

	class ScoreComponent : public ComponentBase, public IObserver
	{
	public:

		ScoreComponent(GameObject* ownerPtr, TextComponent* textComponent);
		~ScoreComponent() override = default;

		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		void Notify(const Event& event, GameObject* observedGameObject) override;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		int GetScore() const;

	private:

		int m_CurrentScore{ 0 };

		TextComponent* m_ScoreDisplay;
	};
}

