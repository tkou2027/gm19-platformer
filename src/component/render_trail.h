#pragma once
#include "render_component.h"
#include "render/render_data_trail.h"
#include "resource/trail.h"

namespace foo
{
	class Space;
	class RenderTrail : public RenderComponent
	{
	public:
		RenderTrail(List& trails);
		void UpdateRenderData() override;

		//void SetTrail(const Trail& trail_data);
		//const  Trail& GetTrail() const;
	private:
		List& m_trails;
	};
}