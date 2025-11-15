#pragma once
#include "math/vector2.h"
#include "util/list.h"
#include "resource/trail.h"
namespace foo
{
	class RenderDataTrail
	{
	public:
		void Initialize();
		void Finalize();
		void Clear();

		void Add(const Trail& trail);
		void Draw();
	private:
		static constexpr int MAX_TRAILS{ 16 };
		Trail m_trails[16];
		int m_trail_cnt{ 0 };
		unsigned int m_vao{ 0 };
	};
}