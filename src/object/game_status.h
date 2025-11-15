#pragma once

namespace foo
{
	// static status across scenes
	class GameStatus
	{
	public:
		static constexpr size_t ITEM_NUM{ 3 };
		void Initialize()
		{
			for (auto& item_get : m_items_get)
			{
				item_get = false;
			}
		}
		void Finalize() {}
		void SetItemGet(size_t index)
		{
			assert(index < ITEM_NUM);
			m_items_get[index] = true;
		}
		bool GetItemGet(size_t index) const
		{
			assert(index < ITEM_NUM);
			return m_items_get[index];
		}
	private:
		bool m_items_get[ITEM_NUM]{ false, false, false };
	};
}