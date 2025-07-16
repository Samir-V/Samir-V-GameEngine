#pragma once

using EventId = unsigned int;

struct Event
{
	const EventId id;

	explicit Event(EventId _id) : id{ _id } {}
};

namespace svengine
{
	class GameObject;

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void Notify(const Event& event, GameObject* gameObjectCausingEvent) = 0;
	};
}

