#pragma once
#include <string>
#include "game_object.h"

namespace foo
{
	class Background : public GameObject
	{
	public:
		Background(const std::string& texture);
	};
}