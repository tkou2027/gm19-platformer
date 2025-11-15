#include "enemy_attack.h"

#include "global_context.h"
#include "physics/physics_system.h"
#include "component/render_sprite_pool.h"
#include "resource/tile.h"
#include "player.h"

namespace foo
{

	EnemyAttackRay::EnemyAttackRay(const Vector2& origin, const Vector2& target, SpaceId space_id)
		: m_physics_shape(origin, target, 2000.0f, 8.0f), m_space_id(space_id)
	{
		m_transform.SetPosition(m_physics_shape.GetCenter());
		const Vector2& from_origin{ target - origin };
		float rotation = atan2(from_origin.y, from_origin.x);
		m_transform.SetRotation(rotation);

		// render
		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprite.Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::ENEMY_SHOOTER_ATTACK));
		m_sprite.size = m_physics_shape.GetHalfSize() * 2;
		m_sprite.space_id = space_id;
		// m_sprite.color = g_space_config.GetSpaceProp(m_space_id).color;// Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		m_sprite.transform = &m_transform;
	}

	void EnemyAttackRay::UpdateRenderData()
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		render_system->GetRenderData().Add(m_sprite.GetConfig(), m_transform.GetMatrix());
	}

	void EnemyAttackRay::SetShape(const Vector2& origin, const Vector2& target)
	{
		m_physics_shape = RectOriented(origin, target, 2000.0f, 8.0f);

		m_transform.SetPosition(m_physics_shape.GetCenter());
		const Vector2& from_origin{ target - origin };
		float rotation = atan2(from_origin.y, from_origin.x);
		m_transform.SetRotation(rotation);

		m_sprite.size = m_physics_shape.GetHalfSize() * 2;
	}

	bool EnemyAttackRay::HandleOverlap(const Rect& other_shape, CollisionTestLayer other_layer, GameObject* other_obj)
	{
		bool hit = m_state == EnemyAttackRayState::ACTIVE
			&& m_physics_shape.IfOverlap(other_shape);
		if (other_layer != CollisionTestLayer::PLAYER)
		{
			return hit; // TODO
		}
		Player* player = (Player*)other_obj;
		hit = hit && player->GetInSpaceId() != m_space_id; // space
		 // TODO
		if (hit)
		{
			player->GetCollisionInfo().hurt = true;

			m_sprite.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			g_space_config.GetSpaceProp(m_space_id).color;
			// m_sprite.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return hit;
	}

	void EnemyAttackRay::Update()
	{
		//if (m_active)
		//{
		//	if (attack_countdown <= 0)
		//	{

		//	}
		//}
		if (m_state == EnemyAttackRayState::ACTIVE)
		{
			if (attack_countdown <= 0)
			{
				attack_countdown = 0;
				m_state = EnemyAttackRayState::DEAD;
			}
			attack_countdown--;
		}
	}

	bool EnemyAttackRay::IfDead() const
	{
		return m_state == EnemyAttackRayState::DEAD;
	}

	void EnemyAttackRay::SetActive()
	{
		m_state = EnemyAttackRayState::ACTIVE;
		m_physics_shape.SetThickness(64);
		m_sprite.size.y = { 64.0f };
	}

	// Rain =====
	bool EnemyAttackRain::HandleOverlap(const Rect& other_shape, CollisionTestLayer other_layer, GameObject* other_obj)
	{
		bool hit = m_bbox.IfOverlap(other_shape);
		if (other_layer != CollisionTestLayer::PLAYER)
		{
			return hit; // TODO
		}
		Player* player = (Player*)other_obj;
		hit = hit && player->GetInSpaceId() != m_space_id; // space
		if (hit)
		{
			player->GetCollisionInfo().in_rain = true;
		}
		return hit;
	}

	// Manager =======
	EnemyAttackManager::EnemyAttackManager() : m_collision_trigger(m_attacks)
	{
		// pool objects
		m_attacks.Initialize(MAX_ATTACKS);

		// render
		SetRenderComponent(new RenderSpritePool(m_attacks));

		// physics
		g_global_context.m_physics_system->AddCollisionTrigger(
			&m_collision_trigger, CollisionTriggerLayer::ENEMY_ATTACK
		);

		// set named object
		m_name = ObjectName::ENEMY_ATTACK_MANAGER;
		SetNamedObject(m_name, this);
	}

	void EnemyAttackManager::Update()
	{
		// update objects
		ObjectLightWeight::UpdateList(m_attacks);
	}

	void EnemyAttackManager::Add(ObjectLightWeight* attack)
	{
		FOO_ASSERT(!m_attacks.Full(), "Enemy attack list overflow");
		m_attacks.Add(attack);
	}

	bool EnemyAttackManager::EnemyAttackCollisionTrigger::HandleOverlap(const Rect& rect, CollisionTestLayer other_layer, GameObject* other_obj)
	{
		bool collides{ false };
		for (int i = 0; i < m_attacks.GetCount(); i++)
		{
			ObjectLightWeight* attack = (ObjectLightWeight*)m_attacks[i];
			collides |= attack->HandleOverlap(rect, other_layer, other_obj);

		}
		return collides;
	}
}