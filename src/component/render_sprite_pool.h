#pragma once
#include "render_component.h"
#include "util/list.h"
namespace foo
{
	class RenderSpritePool : public RenderComponent
	{
	public:
		RenderSpritePool(List& objects);
		void UpdateRenderData() override;
	private:
		List& m_objects;
	};
}