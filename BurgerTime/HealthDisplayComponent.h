#pragma once
#include <vector>

#include "ComponentBase.h"
#include "IObserver.h"

namespace svengine
{
	class Texture2DComponent;

	class HealthDisplayComponent final : public ComponentBase, public IObserver
	{
	public:

		HealthDisplayComponent(GameObject* ownerPtr, std::vector<Texture2DComponent*> lifeDisplays, GameObject* peterPepper);
		~HealthDisplayComponent() override = default;

		HealthDisplayComponent(const HealthDisplayComponent& other) = delete;
		HealthDisplayComponent(HealthDisplayComponent&& other) = delete;
		HealthDisplayComponent& operator=(const HealthDisplayComponent& other) = delete;
		HealthDisplayComponent& operator=(HealthDisplayComponent&& other) = delete;

		void Notify(const Event& event, GameObject* gameObjectCausingEvent) override;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

	private:

		std::vector<Texture2DComponent*> m_LifeDisplays;
		GameObject* m_PeterPepperPtr;
	};
}


