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

		Texture2DComponent(const std::string& filename);
		~Texture2DComponent() override = default;

		Texture2DComponent(const Texture2DComponent& other) = delete;
		Texture2DComponent(Texture2DComponent&& other) = delete;
		Texture2DComponent& operator=(const Texture2DComponent& other) = delete;
		Texture2DComponent& operator=(Texture2DComponent&& other) = delete;

		void Update([[maybe_unused]] float elapsedSec) override;
		void FixedUpdate([[maybe_unused]] float fixedTimeStep) override;
		void LateUpdate([[maybe_unused]] float elapsedSec) override;
		void Render() const override;

		void SetPosition(float x, float y) override;
		void SetOwner(const GameObject* owner) override;
		void Destroy() override;
		bool GetMarkedToDestroy() const override;

	private:

		bool m_MarkedToDestroy{};
		Transform m_Transform{};
		std::shared_ptr<Texture2D> m_Texture{};
		const GameObject* m_OwnerPtr{};
	};
}

