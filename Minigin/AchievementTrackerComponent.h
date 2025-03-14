#pragma once

#include "ComponentBase.h"
#include "IObserver.h"
#include "Transform.h"

namespace dae
{
	class AchievementTrackerComponent : public dae::ComponentBase, public dae::IObserver
	{
	public:

		AchievementTrackerComponent(GameObject* ownerPtr);
		~AchievementTrackerComponent() override = default;

		AchievementTrackerComponent(const AchievementTrackerComponent& other) = delete;
		AchievementTrackerComponent(AchievementTrackerComponent&& other) = delete;
		AchievementTrackerComponent& operator=(const AchievementTrackerComponent& other) = delete;
		AchievementTrackerComponent& operator=(AchievementTrackerComponent&& other) = delete;

		void Notify(EventType event, GameObject* observedGameObject) override;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		Transform m_LocalTransform{};
	};
}

