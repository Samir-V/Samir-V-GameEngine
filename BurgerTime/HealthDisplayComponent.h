#pragma once
#include "ComponentBase.h"
#include "IObserver.h"
#include "Transform.h"

namespace dae
{
	class TextComponent;

	class HealthDisplayComponent final : public ComponentBase, public IObserver
	{
	public:

		HealthDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent);
		~HealthDisplayComponent() override = default;

		HealthDisplayComponent(const HealthDisplayComponent& other) = delete;
		HealthDisplayComponent(HealthDisplayComponent&& other) = delete;
		HealthDisplayComponent& operator=(const HealthDisplayComponent& other) = delete;
		HealthDisplayComponent& operator=(HealthDisplayComponent&& other) = delete;

		void Notify(const Event& event, GameObject* observedGameObject) override;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		Transform m_LocalTransform{};

		TextComponent* m_HealthDisplay;
	};
}


