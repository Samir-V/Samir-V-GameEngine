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
		void SetXVelocity(float x);
		void SetYVelocity(float y);

		float GetMaxSpeed() const;

	private:

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};

		const float m_MaxSpeed{};
	};
}
