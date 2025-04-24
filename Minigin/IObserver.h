#pragma once

using EventId = unsigned int;

struct Event
{
	const EventId id;

	explicit Event(EventId _id) : id{ _id } {}
};

namespace dae
{
	class GameObject;

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(const Event& event, GameObject* observedGameObject) = 0;
	};
}

