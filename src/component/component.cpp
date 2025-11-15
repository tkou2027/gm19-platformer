#include "component.h"
#include "platform/assert.h"

namespace foo
{
	ComponentList::ComponentList(GameObject* object) : m_object{ object }
	{
		for (int i = 0; i < COMPONENT_MAX; ++i)
		{
			m_components[i] = nullptr;
		}
	}

	ComponentList::~ComponentList() {
		Finalize();
		for (int i = 0; i < COMPONENT_MAX; ++i) {
			delete m_components[i];
		}
	}

	void ComponentList::Update()
	{
		// assume components won't be deleted
		for (int i = 0; i < m_component_num; i++)
		{
			m_components[i]->Update();
		}
	}

	void ComponentList::Finalize()
	{
		// assume components won't be deleted
		for (int i = m_component_num - 1; i >= 0; i--)
		{
			m_components[i]->Finalize();
		}
	}

	void ComponentList::AddComponent(Component* component)
	{
		FOO_ASSERT(m_component_num < COMPONENT_MAX - 1, "[ComponentList] Cannot add more components");
		component->SetGameObject(m_object);
		m_components[m_component_num++] = component;
	}

	Component* ComponentList::GetComponentOfType(ComponentType type)
	{
		for (int i = 0; i < m_component_num; ++i) {
			if (m_components[i] && m_components[i]->GetType() == type) {
				return m_components[i];
			}
		}
		return nullptr;
	}

	const Component* ComponentList::GetComponentOfType(ComponentType type) const
	{
		for (int i = 0; i < m_component_num; ++i) {
			if (m_components[i] && m_components[i]->m_type == type) {
				return m_components[i];
			}
		}
		return nullptr;
	}
	void ComponentList::SetGameObject(GameObject* object)
	{
		m_object = object;
		for (int i = 0; i < m_component_num; ++i) {
			if (m_components[i]) {
				m_components[i]->m_object = object;
			}
		}
	}
}