#pragma once
#include <memory>
#include <string>

#include "ComponentBase.h"
#include "Transform.h"
#include "Texture2D.h"

namespace dae
{
	class Texture2DComponent final : public ComponentBase
	{
	public:

		Texture2DComponent(GameObject* ownerPtr, const std::string& filename);
		~Texture2DComponent() override = default;

		Texture2DComponent(const Texture2DComponent& other) = delete;
		Texture2DComponent(Texture2DComponent&& other) = delete;
		Texture2DComponent& operator=(const Texture2DComponent& other) = delete;
		Texture2DComponent& operator=(Texture2DComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void FixedUpdate(float fixedTimeStep) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetPosition(float x, float y) override;
		void Destroy() override;
		bool IsMarkedToDestroy() const override;

	private:

		bool m_MarkedToDestroy{};
		Transform m_Transform{};
		std::shared_ptr<Texture2D> m_Texture{};
	};
}

