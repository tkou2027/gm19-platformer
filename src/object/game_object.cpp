#include "game_object.h"
#include "platform/assert.h"
#include "global_context.h"
#include "scene/scene.h"

namespace foo {
	// Object ========
	void GameObject::AddObject(GameObject* object, GameObject* parent)
	{
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		FOO_ASSERT(!!scene, "Failed to get current scene");
		GameObjectList& objects = scene->GetGameObjects();
		objects.AddObject(object, parent);
	}

	void GameObject::SetHitStop(int frames)
	{
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		FOO_ASSERT(!!scene, "Failed to get current scene");
		scene->GetGameObjects().SetHitStop(frames);
	}

	void GameObject::SetNamedObject(ObjectName name, GameObject* object)
	{
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		FOO_ASSERT(!!scene, "Failed to get current scene");
		scene->SetNamedObject(name, object);
	}
	GameObject* GameObject::GetNamedObject(ObjectName name) const
	{
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		FOO_ASSERT(!!scene, "Failed to get current scene");
		GameObject* object = scene->GetNamedObject(name);
		FOO_ASSERT(!!object, "Failed to get named object in scene");
		return object;
	}

	// List ========
	GameObjectList::GameObjectList()
	{
		m_objects.Initialize(OBJECT_MAX);
	}

	void GameObjectList::Update()
	{
		// hit stop
		if (m_hit_stop)
		{
			if (m_hit_stop_countdown <= 0)
			{
				m_hit_stop_countdown = 0;
				m_hit_stop = false;
			}
			else
			{
				m_hit_stop_countdown--;
				return;
			}
		}
		// update
		for (int i = 0; i < m_objects.GetCount(); i++)
		{
			GameObject* obj = (GameObject*)m_objects[i];
			if (obj->GetActive())
			{
				// custom updates
				obj->Update();
				// component updates
				obj->GetComponentList().Update();
			}
		}
		HandleRemove();
	}

	void GameObjectList::Finalize()
	{
		int cnt = 0;
		for (int i = 0; i < m_objects.GetCount(); i++)
		{
			GameObject* obj = (GameObject*)m_objects[i];
			// component finalize
			obj->GetComponentList().Finalize();
			// custom finalize
			obj->Finalize();
		}
		m_objects.ClearDelete();
		m_objects.Finalize();
	}

	void GameObjectList::AddObject(GameObject* object, GameObject* parent)
	{
		FOO_ASSERT(object, "Adding null object to list");
		FOO_ASSERT(!m_objects.Full(), "Object list overflow");
		m_objects.Add(object);
		if (parent)
		{
			parent->m_transform.AddChild(&(object->m_transform));
		}
	}

	void GameObjectList::SetHitStop(int frames)
	{
		m_hit_stop = true;
		m_hit_stop_countdown = frames > m_hit_stop_countdown ? frames : m_hit_stop_countdown;
	}

	void GameObjectList::HandleRemove()
	{
		bool removed{ false };
		for (int i = 0; i < m_objects.GetCount(); i++)
		{
			GameObject* obj = (GameObject*)m_objects[i];
			if (obj->GetRemoved())
			{
				// object list manages memory, do not delete objects elsewhere
				obj->GetComponentList().Finalize();
				obj->Finalize();
				delete obj;
				m_objects.RemoveOrderedPend(i);
				removed = true;
			}
		}
		if (removed)
		{
			m_objects.RemoveOrderedApply();
		}
	}
}
