#pragma once

#include "component.h"
#include "render/render_system.h"
#include "resource/render_layer.h"

namespace foo
{
	class RenderComponent : public Component
	{
	public:
		RenderComponent() { m_type = ComponentType::SPRITE; }
		virtual ~RenderComponent() = default;
		virtual void UpdateRenderData() {};
	};

	// multiple
	class RenderComponentList : public RenderComponent
	{
	public:
		~RenderComponentList()
		{
			Finalize();
		}
		void Add(RenderComponent* comp)
		{
			comp->SetGameObject(m_object);
			m_list[m_count] = comp;
			m_count++;
		}
		void UpdateRenderData() override
		{
			for (int i = 0; i < m_count; i++)
			{
				m_list[i]->UpdateRenderData();
			}
		}
		void Update() override
		{
			for (int i = 0; i < m_count; i++)
			{
				m_list[i]->Update();
			}
		}
		void Finalize() override
		{
			for (int i = m_count - 1; i >= 0; i--)
			{
				m_list[i]->Finalize();
				delete m_list[i];
			}
			m_count = 0;
		}
		void SetGameObject(GameObject* object) override
		{
			m_object = object;
			for (int i = 0; i < m_count; i++)
			{
				m_list[i]->SetGameObject(m_object);
			}
		}
		RenderComponent* operator[](size_t i)
		{
			if (i >= m_count)
			{
				return nullptr;
			}
			return m_list[i];
		}
		const RenderComponent* operator[](size_t i) const
		{
			if (i >= m_count)
			{
				return nullptr;
			}
			return m_list[i];
		}
	private:
		static constexpr int MAX_COMPONENTS{ 4 };
		RenderComponent *m_list[MAX_COMPONENTS];
		int m_count{ 0 };
	};
}
