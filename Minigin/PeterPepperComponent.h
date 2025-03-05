#pragma once
#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class PeterPepperComponent final : public ComponentBase
	{
	public:

		PeterPepperComponent(GameObject* ownerPtr, float maxSpeed);
		~PeterPepperComponent() override = default;

		PeterPepperComponent(const PeterPepperComponent& other) = delete;
		PeterPepperComponent(PeterPepperComponent&& other) = delete;
		PeterPepperComponent& operator=(const PeterPepperComponent& other) = delete;
		PeterPepperComponent& operator=(PeterPepperComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;
		void RenderImGui() override;

		void SetLocalPosition(float x, float y) override;
		void SetXDirection(float x);
		void SetYDirection(float y);
		void AddInputToDirection(float x, float y);

	private:

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};
		glm::vec2 m_Direction{};

		const float m_MaxSpeed{};
	};
}
