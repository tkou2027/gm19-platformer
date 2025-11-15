#pragma once
#include "component/transform.h"
#include "component/component.h"
#include "util/list.h"
#include "object_name.h"

namespace foo
{
	class RenderComponent;
	class PhysicsComponent;
	class Scene;
	class GameObject
	{
		friend class GameObjectList;
	public:
		enum class ObjectState
		{
			ACTIVE,
			IDLE,
			REMOVED
		};
		GameObject()
		{
			m_transform.SetGameObject(this);
			m_components.SetGameObject(this);
		}
		virtual ~GameObject() = default;
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Finalize() {}
		// deleting objects
		virtual void SetActive() { m_object_state = ObjectState::ACTIVE; }
		virtual void SetIdle() { m_object_state = ObjectState::IDLE; }
		virtual void SetRemoved() { m_object_state = ObjectState::REMOVED; }
		bool GetActive() const { return m_object_state == ObjectState::ACTIVE; }
		bool GetRemoved() const { return m_object_state == ObjectState::REMOVED; }
		// components
		ComponentList& GetComponentList() { return m_components; }
		RenderComponent* GetRenderComponent() { return m_render_component; }
		PhysicsComponent* GetPhysicsComponent() { return m_physics_component; }
		void SetRenderComponent(RenderComponent* comp) {
			m_render_component = comp;
			m_components.AddComponent((Component*)comp);
		}
		void SetPhysicsComponent(PhysicsComponent* comp) {
			m_physics_component = comp;
			m_components.AddComponent((Component*)comp);
		}
		// component transform
		const Transform& GetTransform() const { return m_transform; }
		Transform& GetTransform() { return m_transform; }
		void SetTransform(const Transform& transform) { m_transform = transform; }
		// scene
		void AddObject(GameObject* object, GameObject* parent = nullptr);
		void SetHitStop(int frames);
	protected:
		// scene
		void SetNamedObject(ObjectName name, GameObject* object);
		GameObject* GetNamedObject(ObjectName name) const;

		int m_id{ -1 };
		ObjectState m_object_state{ ObjectState::ACTIVE };
		// common components
		Transform m_transform;
		RenderComponent* m_render_component{ nullptr };
		PhysicsComponent* m_physics_component{ nullptr };
		// other components (not used for now)
		ComponentList m_components;
		ObjectName m_name{ ObjectName::NONE };
	};

	class GameObjectList
	{
	public:
		GameObjectList();
		void Update();
		void Finalize();
		void AddObject(GameObject* object, GameObject* parent = nullptr);
		List& GetObjectList() { return m_objects; }
		void SetHitStop(int frames);
	private:
		static constexpr int OBJECT_MAX{ 256 };

		void HandleRemove();
		List m_objects{};
		// hit stop
		bool m_hit_stop{ false };
		int m_hit_stop_countdown{ 0 };
	};
}