#include "item.h"
#include "component/render_sprites.h"
#include "global_context.h"
#include "resource/tile.h"
#include "platform/sound.h"
#include "game_status.h"
#include "hint.h"
#include "player.h"

namespace foo
{
	Item::Item(char code, int scope_id) : m_scope_id(scope_id)
	{
		Decode(code);

		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprite.Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_SQUARE));
		m_sprite.transform = &m_sprite_transform;
		m_transform.AddChild(&m_sprite_transform);
		if (m_sword)
		{
			m_sprite.color = g_space_config.GetSpaceProp(SpaceId::SPACE_A).color;
		}
		else
		{
			m_sprite_transform.SetRotation(Math::PI * 0.25f);
		}
		m_sprite.size = m_base_size;
		m_sprite_transform.SetPosition({ 0.0f, -50.0f, });

		SetRenderComponent(new RenderSprites(&m_sprite));

		m_item_manager = (ItemManager*)GetNamedObject(ObjectName::ITEM_MANAGER);
		m_player = (Player*)GetNamedObject(ObjectName::PLAYER);
		m_hint = (Hint*)GetNamedObject(ObjectName::HINT);

		bool taken = m_item_manager->IfTaken(m_index);
		if (taken)
		{
			SetIdle();
		}
	}

	void Item::Update()
	{
		switch (m_state)
		{
		case ItemState::WAIT:
		{
			m_yoyo.Update();
			m_sprite.offset.y = m_yoyo.GetT() * -50.0f;
			m_sprite.size.x = Math::Max(10.0f, m_yoyo.GetT() * m_base_size.x);
			if (!m_sword)
			{
				m_sprite.size.y = Math::Max(10.0f, m_yoyo.GetT() * m_base_size.y);
			}
			Vector2 position = m_sprite_transform.GetTransform2D().position;
			if (m_player->IfPlayerInDistance(position, 48.0f))
			{
				m_state = ItemState::GETTING;
				m_sprite.size = m_base_size * 0.8f;
				m_countdown = 60;
				m_player->TakeItem(m_sword);
				if (m_sword)
				{
					m_hint->ShowHint(Hint::HintType::ATTACK, position + Vector2{ 0.0f, -80.0f });
				}
				g_global_context.m_sound->PlaySe(SeName::ITEM_GET);
			}
			break;
		}
		case ItemState::GETTING:
		{

			if (m_countdown == 0)
			{
				m_item_manager->SetTaken(m_index);
				m_state = ItemState::GET;
				m_sprite.active = false;
				SetIdle();
				break;
			}
			float t = 1.0f - (float)m_countdown / 60;
			m_sprite_transform.SetPosition({ 0.0f, -100.0f - 30.0f * t, 0.0f });
			Vector2 player_pos = m_player->GetTransform().GetTransform2D().position;
			Vector2 position = m_sprite_transform.GetTransform2D().position;
			m_sprite.offset.x = (player_pos.x - position.x);// *t* t;
			m_sprite.offset.y = m_yoyo.GetT() * -30.0f;
			m_sprite.color.w = 1.0f - t;
			// m_sprite.size.x = Math::Max(10.0f, m_yoyo.GetT() * m_base_size.x);
			m_countdown--;
			break;
		}
		}
	}

	void Item::SetActive()
	{
		bool taken = m_item_manager->IfTaken(m_index);
		if (taken)
		{
			GameObject::SetIdle();
		}
		else
		{
			GameObject::SetActive();
		}
		
	}

	void Item::Decode(char code)
	{
		// TODO
		switch (code)
		{
		case '$':
		{
			m_sword = true;
			m_index = 0;
			break;
		}
		case '%':
		{
			m_sword = false;
			m_index = 1;
			break;
		}
		case '^':
		{
			m_sword = false;
			m_index = 2;
			break;
		}
		case '&':
		{
			m_sword = false;
			m_index = 3;
			break;
		}
		}
		if (m_sword)
		{
			m_base_size = { 40.0f, 120.0f };
		}
		else
		{
			m_base_size = { 60.0f, 60.0f };
		}
	}

	ItemManager::ItemManager()
	{
		for (int i = 0; i < NUM_ITEMS; i++)
		{
			m_taken[i] = false;
		}
		m_name = ObjectName::ITEM_MANAGER;
		SetNamedObject(m_name, this);

		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprite_transform.SetRotation(Math::PI * 0.25f);
		m_transform.AddChild(&m_sprite_transform);
		for (int i = 0; i < 3; i++)
		{
			m_sprites[i].Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_BORDER));
			m_sprites[i].offset = { SCREEN_WIDTH * 0.5f - 200.0f + i * 45.0f, -SCREEN_HEIGHT * 0.5f + 40.0f};
			m_sprites[i].layer = RenderLayer::UI;
			m_sprites[i].transform = &m_sprite_transform;
		}

		SetRenderComponent(new RenderSprites(m_sprites, 3));

		m_render_component->SetActive(false);
	}

	void ItemManager::Update()
	{
		if (!m_animation)
		{
			return;
		}

		Vector2 camera_offset = g_global_context.m_render_system->GetCamera().GetOffset();
		m_transform.SetPosition({ camera_offset });
		for (int i = 0; i < 3; i++)
		{
			float t = (m_countdown - 15.0f * i) / 60.0f;
			t = t < 0 ? 0 : (t > 1 ? 1 : t);
			m_sprites[i].size = Vector2{ 30.0f, 30.0f } *Tween::EaseFunc(Tween::TWEEN_FUNCTION::EASE_IN_OUT_BACK, t);
		}
		m_countdown++;
		if (m_countdown > 150)
		{
			m_animation = false;
			m_render_component->SetActive(false);
		}
	}

	void ItemManager::SetTaken(int index)
	{
		m_taken[index] = true;
		if (index > 0)
		{
			g_global_context.m_game_status->SetItemGet(static_cast<size_t>(index - 1));
			TileManager* tile_manager = g_global_context.m_tile_manager;
			m_sprites[index - 1].SetConfigInput(tile_manager->LoadSpriteConfig(SpritePresetName::ITEM_SQUARE));
			m_countdown = 0;
			m_animation = true;
			m_render_component->SetActive(true);
		}
	}
}
