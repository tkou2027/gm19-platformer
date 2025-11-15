#pragma once

#include "scene.h"

namespace foo
{
	class SceneTitle : public Scene
	{
	public:
		void Initialize() override;
		void Update() override;
		void Finalize() override;
	};
}