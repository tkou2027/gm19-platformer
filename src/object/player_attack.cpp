#include "player_attack.h"

#include "global_context.h"
#include "resource/tile.h"
#include "physics/physics_system.h"
#include "component/render_sprite_pool.h"
#include "enemy_attack.h"

namespace foo
{
	PlayerAttack::PlayerAttack(const Circle& circle)
		: m_shape(circle),
		m_sprite_config_input(g_global_context.m_tile_manager->LoadSpriteConfig(SpritePresetName::DEFAULT))
	{
		//m_sprite_state.Initialize(m_sprite_config_input);
	}

	bool PlayerAttack::HandleOverlap(const Rect& other_shape, CollisionTestLayer other_layer, GameObject* other_obj)
	{
		bool hit = m_shape.IfOverlap(other_shape);
		// TODO
		return hit;
	}

	void PlayerAttack::UpdateRenderData()
	{
		//SpriteConfigOutput output{};
		//m_sprite_state.GetConfig(output);
		//// TODO
		//output.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//// output.render_layer
		//g_global_context.m_render_system->GetRenderData().Add(
		//	output, Matrix4x4{ m_transform }
		//);
	}

	PlayerAttackManager::PlayerAttackManager() : m_collision_trigger(m_attacks)
	{
		// pool objects
		m_attacks.Initialize(MAX_ATTACKS);

		// render
		SetRenderComponent(new RenderSpritePool(m_attacks));

		// physics
		g_global_context.m_physics_system->AddCollisionTrigger(
			&m_collision_trigger, CollisionTriggerLayer::PLAYER_ATTACK
		);

		// set named object
		m_name = ObjectName::PLAYER_ATTACK_MANAGER;
		SetNamedObject(m_name, this);
	}

	void PlayerAttackManager::Add(PlayerAttack* attack)
	{
		FOO_ASSERT(!m_attacks.Full(), "Player attack list overflow");
		m_attacks.Add(attack);
	}

	void PlayerAttackManager::Update()
	{
		// update objects
		ObjectLightWeight::UpdateList(m_attacks);
	}

	void PlayerAttackManager::Clear()
	{
		ObjectLightWeight::ClearList(m_attacks);
	}

	bool PlayerAttackManager::PlayerAttackCollisionTrigger::HandleOverlap(const Rect& rect, CollisionTestLayer other_layer, GameObject* other_obj)
	{
		bool collides{ false };
		for (int i = 0; i < m_attacks.GetCount(); i++)
		{
			PlayerAttack* attack = (PlayerAttack*)m_attacks[i];
			collides |= attack->HandleOverlap(rect, other_layer, other_obj);
		}
		return collides;
	}
}

