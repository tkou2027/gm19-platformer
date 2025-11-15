#pragma once

namespace foo
{
	class GameObject; // parent game object

	enum class ComponentType
	{
		NONE,
		SPRITE,
		PHYSICS,
		CUSTOM
	};
	class Component
	{
		friend class ComponentList;
	public:
		virtual ~Component() = default;
		virtual void Update() {};
		virtual void Finalize() {};
		ComponentType GetType() const { return m_type; }
		virtual void SetGameObject(GameObject* object) { m_object = object; }
		virtual bool GetActive() const { return m_active; }
		virtual void SetActive(bool active) { m_active = active; }
	protected:
		ComponentType m_type{ ComponentType::NONE };
		GameObject* m_object{ nullptr };
		bool m_active{ true };
	};

	class ComponentList
	{
	public:
		ComponentList(GameObject* object = nullptr);
		~ComponentList();
		void Update();
		void Finalize();
		void AddComponent(Component* component);
		Component* GetComponentOfType(ComponentType type);
		const Component* GetComponentOfType(ComponentType type) const;
		void SetGameObject(GameObject* object);
	private:
		static constexpr int COMPONENT_MAX{ 8 };
		int m_component_max{ 2 };
		int m_component_num{ 0 };
		Component* m_components[COMPONENT_MAX];
		GameObject* m_object{ nullptr };
	};
}