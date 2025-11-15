#pragma once
#include <string>

namespace foo
{
	enum class BgmName
	{
		BGM_TITLE,
		BGM_STAGE,
		BGM_RAIN,
		MAX
	};
	enum class SeName
	{
		CLICK,
		ITEM_GET,
		HIT,
		DIE,
		MAX
	};
	
	class SoundManager
	{
	public:
		static std::wstring GetBgmPath(BgmName name);
		static std::wstring GetSePath(SeName name);
	};
}