#pragma once
#include <memory>
#include <unordered_set>

#include "ComponentBase.h"
#include "Subject.h"
#include "Transform.h"

namespace dae
{
	class Scene;

	class RectCollider2DComponent : public ComponentBase
	{
	public:

		struct Rect
		{
			float posX;
			float posY;
			float width;
			float height;
		};

		RectCollider2DComponent(GameObject* ownerPtr, float collisionRectWidth, float collisionRectHeight);
		~RectCollider2DComponent() override;

		RectCollider2DComponent(const RectCollider2DComponent& other) = delete;
		RectCollider2DComponent(RectCollider2DComponent&& other) = delete;
		RectCollider2DComponent& operator=(const RectCollider2DComponent& other) = delete;
		RectCollider2DComponent& operator=(RectCollider2DComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;

		void SetShouldTriggerEvents(bool triggerEvents);
		void SetShouldCollide(bool shouldCollide);
		void SetIsStatic(bool isStatic);

		bool RayIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, float rayLength) const;

		Subject* GetCollisionEnterEvent() const;
		Subject* GetCollisionExitEvent() const;
		Subject* GetCollisionStayEvent() const;

		Rect GetCollisionRect() const;

		static bool IsOverlapping(const Rect& rect1, const Rect& rect2);
		static glm::vec2 GetCollisionOverlapShift(const Rect& rect1, const Rect& rect2);
		static std::unordered_set<GameObject*> GetRayIntersectedGameObjects(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, float rayLength);
		static void RemoveAllCollidersFromScene(const Scene* scene);

	private:

		static std::vector<RectCollider2DComponent*> m_Colliders;

		Transform m_LocalTransform	{ };
		Rect m_CollisionRect		{ };

		bool m_ShouldTriggerEvents	{ };
		bool m_ShouldCollide		{ };
		bool m_IsStatic				{ };

		std::unique_ptr<Subject> m_CollisionEnterEvent;
		std::unique_ptr<Subject> m_CollisionExitEvent;
		std::unique_ptr<Subject> m_CollisionStayEvent;

		std::unordered_set<RectCollider2DComponent*> m_PreviousFrameCollisions;
	};
}

