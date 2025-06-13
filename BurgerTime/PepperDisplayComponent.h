#pragma once
#include "ComponentBase.h"
#include "IObserver.h"

namespace dae
{
	class TextComponent;

	class PepperDisplayComponent : public ComponentBase, public IObserver
	{
	public:

		PepperDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent, GameObject* peterPepper);
		~PepperDisplayComponent() override = default;

		PepperDisplayComponent(const PepperDisplayComponent& other) = delete;
		PepperDisplayComponent(PepperDisplayComponent&& other) = delete;
		PepperDisplayComponent& operator=(const PepperDisplayComponent& other) = delete;
		PepperDisplayComponent& operator=(PepperDisplayComponent&& other) = delete;

		void Notify(const Event& event, GameObject* gameObjectCausingEvent) override;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

	private:

		TextComponent* m_PepperDisplay;
		GameObject* m_PeterPepperPtr;
	};
}


