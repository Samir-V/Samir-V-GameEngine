#pragma once
#include <vector>

#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class CacheTrasherComponent final : public ComponentBase
	{
	public:

		CacheTrasherComponent(GameObject* ownerPtr);
		~CacheTrasherComponent() override = default;

		CacheTrasherComponent(const CacheTrasherComponent& other) = delete;
		CacheTrasherComponent(CacheTrasherComponent&& other) = delete;
		CacheTrasherComponent& operator=(const CacheTrasherComponent& other) = delete;
		CacheTrasherComponent& operator=(CacheTrasherComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

	private:

		void ShowEx1UI() const;
		bool CalculateEx1(int samples, std::vector<float>& measurements) const;

		Transform m_LocalTransform{};
	};
}
