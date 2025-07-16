#pragma once
#include <memory>
#include <SDL_rect.h>
#include <vector>

#include "ComponentBase.h"
#include "IObserver.h"
#include "Subject.h"
#include "Texture2D.h"

namespace svengine
{
	class RectCollider2DComponent;

	class BurgerPartComponent final : public ComponentBase, public IObserver
	{
	public:
		enum class BurgerPartState
		{
			Idle,
			Falling,
			Assembled
		};

		BurgerPartComponent(GameObject* ownerPtr, const std::string& filepath, int nrOfSlices);
		~BurgerPartComponent() override = default;

		BurgerPartComponent(const BurgerPartComponent& other) = delete;
		BurgerPartComponent(BurgerPartComponent&& other) = delete;
		BurgerPartComponent& operator=(const BurgerPartComponent& other) = delete;
		BurgerPartComponent& operator=(BurgerPartComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Notify(const Event& event, GameObject* gameObjectCausingEvent) override;

		Subject* GetBurgerPartCollisionEvent() const;

		BurgerPartState GetBurgerPartState() const;

	private:

		RectCollider2DComponent*   m_PlayerCollider  { nullptr };

		BurgerPartState			   m_BurgerPartState { BurgerPartState::Idle };

		std::unique_ptr<Subject>   m_BurgerPartCollisionEvent;

		std::shared_ptr<Texture2D> m_Texture;
		std::vector<SDL_Rect>      m_SrcRects;
		std::vector<float>		   m_OffsetsY;
		std::vector<GameObject*>   m_EnemiesOnTop;
		int						   m_NrOfSlices;
		int						   m_ExtraLevelsToFall;

		glm::vec2				   m_Velocity{ 0.0f, 35.0f };
	};
}

