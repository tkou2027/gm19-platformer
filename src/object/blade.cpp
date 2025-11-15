#include "blade.h"

#include "component/render_sprites.h"
#include "component/render_after_image.h" // TODO should be trail
#include "trail_manager.h"
#include "player_attack.h"
#include "global_context.h"
#include "render/render_system.h"
#include "resource/tile.h"

namespace foo
{
	Blade::Blade(SpaceId space_id, const Vector2& size) : m_space_id(space_id), m_size(size)
	{
		RenderComponentList* render_components = new RenderComponentList();

		TileManager* tile_manager = g_global_context.m_tile_manager;
		m_sprite.Initialize(tile_manager->LoadSpriteConfig(SpritePresetName::DEFAULT));
		m_sprite.size = m_size;// Vector2{ 40.0f, 40.0f };
		m_sprite.layer = RenderLayer::PLAYER;
		m_sprite.color = g_space_config.GetSpaceProp(m_space_id).color;
		m_sprite_renderer = new RenderSprites(&m_sprite);

		// render components
		render_components->Add(m_sprite_renderer);

		SetRenderComponent(render_components);

		// center
		const Vector3 center_offset{ m_size.x / 2.0f, 0.0f, 0.0f };
		m_transform.AddChild(&m_center_transform);
		m_transform.SetPosition(center_offset);
		m_center_transform.SetPosition(center_offset * -1.0f);

		// trails
		m_trail_manager = (TrailManager *)GetNamedObject(ObjectName::TRAIL_MANAGER);
	}

	void Blade::Update()
	{
		if (m_swing_active)
		{
			UpdateSwing();
		}
	}

	void Blade::StartSwing()
	{
		//m_swing_start = m_center_transform.GetTransform2D();
		//m_swing_end = m_center_transform.GetTransform2D();

		//m_swing_active = true;
		////if (m_trail_object)
		////{
		////	m_trail_object->SetIdle();
		////}
		//TrailObjectCircle::TrailObjectCircleConfig trail_config
		//{
		//	 m_swing_end.position,
		//	 m_size.x
		//};
		//m_trail_object = new TrailObjectCircle(trail_config);
		////m_trail_manager->Create(Shape::ShapeType::PIE);
		//m_trail_object->SetSpaceId(m_space_id);
		//m_trail_manager->Add(m_trail_object);

		//PlayerAttackManager * player_attacks = (PlayerAttackManager *)GetNamedObject(ObjectName::PLAYER_ATTACK_MANAGER);
		//player_attacks->Add(new PlayerAttack({
		//	m_swing_end.position,
		//	 m_size.x
		//}));

		//TrailShape shape = m_trail_object->GetShape();
		//shape.center = m_swing_end.position;
		//shape.radius = m_size.x;
		//shape.rotation = m_swing_end.rotation;
		//shape.rotation_delta = Math::TWO_PI;//m_swing_start.rotation - m_swing_end.rotation;
		//m_trail_object->SetShape(shape);

		// TODO
		m_sprite.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	void Blade::EndSwing()
	{
		m_swing_active = false;
		//if (m_trail_object)
		//{
		//	m_trail_object->SetIdle();
		//	m_trail_object = nullptr;
		//}
		m_sprite.color = g_space_config.GetSpaceProp(m_space_id).color;
	}
	void Blade::SetSize(const Vector2 size)
	{
		m_size = size;
		// render
		m_sprite.size = size;
		// position
		const Vector3 center_offset{ m_size.x / 2.0f, 0.0f, 0.0f };
		m_transform.SetPosition(center_offset);
		m_center_transform.SetPosition(center_offset * -1.0f);
	}
	void Blade::UpdateSwing()
	{
		m_swing_end = m_center_transform.GetTransform2D();
		
		//TrailShape shape = m_trail_object->GetShape();
		//shape.center = m_swing_end.position;
		//shape.radius = m_size.x;
		//shape.rotation = m_swing_end.rotation;
		//shape.rotation_delta = Math::TWO_PI;//m_swing_start.rotation - m_swing_end.rotation;
		//m_trail_object->SetShape(shape);

		//Trail trail = m_trail_renderer->GetTrail();
		//trail.shape.center = m_swing_end.position;
		//trail.shape.radius = m_size.x;
		//trail.shape.rotation = m_swing_end.rotation;
		//trail.shape.rotation_delta = m_swing_start.rotation - m_swing_end.rotation;
		//m_trail_renderer->SetTrail(trail);
		//m_trail_physics->SetTrail(trail);
	}
}

