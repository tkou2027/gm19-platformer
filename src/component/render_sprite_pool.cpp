#include "render_sprite_pool.h"
#include "object/object_pool.h"

namespace foo
{
	RenderSpritePool::RenderSpritePool(List& objects) : m_objects(objects) {}

	void RenderSpritePool::UpdateRenderData()
	{
		for (int i = 0; i < m_objects.GetCount(); i++)
		{
			ObjectLightWeight* object = (ObjectLightWeight*)m_objects[i];
			object->UpdateRenderData();
		}
	}
}