#include "render_trail.h"

#include "global_context.h"
#include "render/render_system.h"

#include "object/game_object.h"

#include "object/trail_manager.h"

namespace foo
{
	RenderTrail::RenderTrail(List& trails) : m_trails{trails} {}

	void RenderTrail::UpdateRenderData()
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		for (int i = 0; i < m_trails.GetCount(); i++)
		{
			TrailObject* trail = (TrailObject*)m_trails[i];
			render_system->GetTrailData().Add(Trail{
				trail->GetShapeRender(),
				trail->GetSpaceId()
			});
		}
	}
}