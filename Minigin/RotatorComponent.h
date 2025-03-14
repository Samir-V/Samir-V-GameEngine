#pragma once
#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class RotatorComponent final : public ComponentBase
	{
	public:

		RotatorComponent(GameObject* ownerPtr, float radius, float angularSpeed);
		~RotatorComponent() override = default;

		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		Transform m_LocalTransform{};

		float m_AngularSpeed;
		float m_Radius;
		float m_Angle;
	};
}

