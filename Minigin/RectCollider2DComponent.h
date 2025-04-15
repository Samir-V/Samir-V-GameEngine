#pragma once
#include <memory>

#include "ComponentBase.h"
#include "Subject.h"
#include "Transform.h"

namespace dae
{
	class RectCollider2DComponent : public ComponentBase
	{
	public:

		struct Rect
		{
			int posX;
			int posY;
			int width;
			int height;
		};

		RectCollider2DComponent(GameObject* ownerPtr, int collisionRectWidth, int collisionRectHeight);
		~RectCollider2DComponent() override = default;

		RectCollider2DComponent(const RectCollider2DComponent& other) = delete;
		RectCollider2DComponent(RectCollider2DComponent&& other) = delete;
		RectCollider2DComponent& operator=(const RectCollider2DComponent& other) = delete;
		RectCollider2DComponent& operator=(RectCollider2DComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

		Subject* GetCollisionEnterEvent() const;
		Subject* GetCollisionExitEvent() const;
		Subject* GetCollisionStayEvent() const;

		void SetShouldTriggerEvents(bool triggerEvents);
		void SetShouldCollide(bool shouldCollide);

		bool GetShouldTriggerEvents() const;
		bool GetShouldCollide() const;

	private:

		Transform m_LocalTransform	{ };
		Rect m_CollisionRect		{ };

		bool m_ShouldTriggerEvents	{ };
		bool m_ShouldCollide		{ true };

		std::unique_ptr<Subject> m_CollisionEnterEvent;
		std::unique_ptr<Subject> m_CollisionExitEvent;
		std::unique_ptr<Subject> m_CollisionStayEvent;
	};
}

