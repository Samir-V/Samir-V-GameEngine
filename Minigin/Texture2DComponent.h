#pragma once
#include <memory>
#include <string>

#include "ComponentBase.h"
#include "Texture2D.h"

namespace svengine
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

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

	private:

		std::shared_ptr<Texture2D> m_Texture{};
	};
}

