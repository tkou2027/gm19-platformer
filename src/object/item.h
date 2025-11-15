#pragma once
#include "game_object.h"
#include "render/sprite_config.h"
#include "util/yoyo.h"

namespace foo
{
	class ItemManager;
	class Player;
	class Hint;
	class Item : public GameObject
	{
	public:
		Item(char code, int scope_id);
		void Update() override;
		void SetActive() override;
	private:
		enum class ItemState
		{
			WAIT,
			GETTING,
			GET
		};
		void Decode(char code);
		bool m_sword{ false };
		int m_index{ 0 };
		int m_scope_id{ -1 };
		SpriteState m_sprite;
		Transform m_sprite_transform;
		Yoyo m_yoyo{ 60 };
		ItemState m_state{ ItemState::WAIT };
		ItemManager* m_item_manager;
		Player* m_player;
		Hint* m_hint;
		int m_countdown{ 0 };
		Vector2 m_base_size{};
	};

	class ItemManager : public GameObject
	{
	public:
		ItemManager();
		void Update() override;
		bool IfTaken(int index) const
		{
			return m_taken[index];
		}
		void SetTaken(int index);
	private:
		static constexpr int NUM_ITEMS{ 4 };
		bool m_taken[NUM_ITEMS];
		bool m_animation{ false };
		SpriteState m_sprites[3];
		Transform m_sprite_transform;
		int m_countdown{ 0 };
	};
}