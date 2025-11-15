#pragma once

#include "math/trail_shape.h"
#include "resource/space_config.h"
#include "component/transform.h"

namespace foo
{
	class ParticleSystem;
	class Camera;
	class TrailObject
	{
	public:
		TrailObject(SpaceId space_id, int scope_id)
			: m_space_id(space_id), m_scope_id(scope_id) {};
		virtual ~TrailObject() = default;
		virtual void Update() = 0;
		virtual const TrailShape& GetShapeRender() = 0;
		virtual const TrailShape& GetShapePhysics() = 0;
		int GetScopeId() const { return m_scope_id; }
		SpaceId GetSpaceId() const { return m_space_id; }
		virtual void Destroy() = 0;
		virtual bool IfDead() const = 0;

		//const TrailShape& GetShape() const { return m_shape; }
		//void SetShape(const TrailShape& shape) { m_shape = shape; }
		void SetScopeId(int scope_id) { m_scope_id = scope_id; }
		void SetSpaceId(SpaceId space_id) { m_space_id = space_id; }
		void SetParticleSystem(ParticleSystem* particle_system) { m_particle_system = particle_system; }
	protected:
		//TrailShape m_shape{ Shape::ShapeType::SHAPE };
		int m_scope_id{ -1 };
		SpaceId m_space_id{ SpaceId::SPACE_DEFAULT };
		ParticleSystem* m_particle_system{ nullptr };
	};

	class TrailObjectCircle : public TrailObject
	{
	public:
		struct TrailObjectCircleConfig
		{
			// shape
			Vector2 center{};
			float radius{ 0.0f };
			float expand_radius{ 0.0f };
			float min_radius{ 40.0f };
			// states
			int expand_frames{ 2 };
			int yoyo_frames{ 20 };
			int shrink_frames{ 30 };
		};
		TrailObjectCircle(
			SpaceId space_id, int scope_id,
			const TrailObjectCircleConfig& config);
		void Update() override;
		bool IfDead() const override { return m_state == CircleTrailState::DEAD; }
		void Destroy() override;
		const TrailShape& GetShapeRender() override;
		const TrailShape& GetShapePhysics() override;
	private:
		enum class CircleTrailState
		{
			EXPAND,
			YOYO,
			SHRINK,
			DEAD
		};
		TrailShape m_shape{ Shape::ShapeType::CIRCLE };
		int m_countdown{ 0 }; // TODO
		CircleTrailState m_state{ CircleTrailState::EXPAND };
		TrailObjectCircleConfig m_config{};
	};

	class TrailObjectLine : public TrailObject
	{
	public:
		struct TrailObjectLineConfig
		{
			// shape
			Vector2 center{ 0.0f, 0.0f };
			Vector2 normal{ 1.0f, 0.0f };
		};
		TrailObjectLine(
			SpaceId space_id, int scope_id,
			const TrailObjectLineConfig& config, Transform* transform_parent = nullptr);
		void Update() override;
		const TrailShape& GetShapeRender() override;
		const TrailShape& GetShapePhysics() override;
		bool IfDead() const override { return m_dead; }
		void SetNormal(const Vector2& normal) { m_shape.normal = normal; }
		void SetLife(int life) { m_life_countdown = life; }
		void Destroy() override;
		void GenDestroyParticle();
	private:
		Transform* m_transform_parent{ nullptr };
		TrailShape m_shape{ Shape::ShapeType::LINE };
		TrailObjectLineConfig m_config{};

		int m_life_countdown{ 300 }; // TODO
		bool m_dead{ false };
	};
}