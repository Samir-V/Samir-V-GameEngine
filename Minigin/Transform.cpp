#include "Transform.h"

void svengine::Transform::SetPosition(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void svengine::Transform::SetPosition(const glm::vec3& pos)
{
	m_Position = pos;
}

