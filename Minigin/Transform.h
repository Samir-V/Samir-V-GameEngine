#pragma once
#include <glm.hpp>

namespace svengine
{
	class Transform final
	{
	public:
		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(float x, float y, float z);
		void SetPosition(const glm::vec3& pos);
	private:
		glm::vec3 m_Position;
	};
}
