#pragma once
#include "ComponentBase.h"
#include "IObserver.h"
#include "Transform.h"

namespace dae
{
	class TextComponent;

	class ScoreDisplayComponent : public dae::ComponentBase, public dae::IObserver
	{
	public:

		ScoreDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent);
		~ScoreDisplayComponent() override = default;

		ScoreDisplayComponent(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent(ScoreDisplayComponent&& other) = delete;
		ScoreDisplayComponent& operator=(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent& operator=(ScoreDisplayComponent&& other) = delete;

		void Notify(EventType event, GameObject* observedGameObject) override;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		Transform m_LocalTransform{};

		TextComponent* m_ScoreDisplay;
	};
}

