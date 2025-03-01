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

		struct TransformHelper
		{
			float matrix[16]
			{
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
			};
		};

		class GameObject3D
		{
		public:
			TransformHelper transform;
			int ID;
		};

		class GameObject3DAlt
		{
		public:
			TransformHelper* transform;
			int ID;
		};

		void ShowEx1UI();
		bool CalculateEx1(int samples, std::vector<float>& measurements);

		void ShowEx2UI();
		bool CalculateEx2(int samples, std::vector<float>& measurements);
		bool CalculateEx2Alt(int samples, std::vector<float>& measurements);

		int m_NrOfSamplesEx1{ 10 };
		bool m_IsEx1GraphActive{ false };
		std::vector<float> m_Ex1Measurements{};

		int m_NrOfSamplesEx2{ 10 };
		bool m_IsEx2GraphActive{ false };
		bool m_IsEx2AltGraphActive{ false };
		std::vector<float> m_Ex2Measurements{};
		std::vector<float> m_Ex2AltMeasurements{};

		Transform m_LocalTransform{};
	};
}
