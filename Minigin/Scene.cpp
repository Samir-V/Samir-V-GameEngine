#include "Scene.h"
#include "GameObject.h"

#include <algorithm>

#include "TextComponent.h"

using namespace dae;

unsigned int Scene::m_idCounter = 0;

Scene::Scene(const std::string& name) : m_Name(name) {}

Scene::~Scene() = default;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	m_Objects.emplace_back(std::move(object));
}

void Scene::Remove(std::unique_ptr<GameObject> object)
{
	m_Objects.erase(std::remove(m_Objects.begin(), m_Objects.end(), object), m_Objects.end());
}

void Scene::RemoveAll()
{
	m_Objects.clear();
}

void Scene::Start()
{
	for (auto& object : m_Objects)
	{
		if (object->IsActive())
		{
			object->Start();
		}
	}
}


void Scene::Update(float elapsedSec)
{
	for(auto& object : m_Objects)
	{
		if (object->IsActive())
		{
			object->Update(elapsedSec);
		}
	}
}

void Scene::LateUpdate(float elapsedSec)
{

	for (auto& object : m_Objects)
	{
		if (object->IsActive())
		{
			object->LateUpdate(elapsedSec);
		}
	}

	// Deletion of marked objects happens at the very end
	auto rangeToDestroy = std::ranges::remove_if(m_Objects, [](const auto& object)
		{
			return object->IsMarkedToDestroy();
		});

	// Removing the objects from the objects that stay alive
	for (auto& object : rangeToDestroy)
	{
		object->SetParent(nullptr, false);
	}

	m_Objects.erase(rangeToDestroy.begin(), m_Objects.end());
}


void Scene::Render() const
{
	for (const auto& object : m_Objects)
	{
		if (object->IsActive())
		{
			object->Render();
		}
	}
}


const std::vector<std::unique_ptr<GameObject>>& Scene::GetGameObjects()
{
	return m_Objects;
}

std::vector<GameObject*> Scene::GetGameObjectsWithTag(Tag tag) const
{
	std::vector<GameObject*> result;
	result.reserve(m_Objects.size());

	for (const auto& gameObject : m_Objects)
	{
		if (gameObject->GetTag() == tag)
		{
			result.push_back(gameObject.get());
		}
	}

	return result;
}

const std::string& Scene::GetName() const
{
	return m_Name;
}



