#pragma once
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "camera.h"
#include "render_data.h"
#include "render_data_tilemap.h"
#include "render_data_trail.h"
#include "render_data_space.h"
#include "render_data_particle.h"
#include "frame_buffer.h"
#include "sprite_config.h"
#include "reflection_config.h"

namespace foo
{
	class RenderSystem
	{
	public:
		void Initialize();
		void Finalize();
		void ClearBuffers();
		void Draw();
		void ResetCamera();

		Camera& GetCamera() { return m_camera; }
		RenderData& GetRenderData() { return m_batch_data; }
		RenderDataTileMap& GetTileMapData() { return m_tile_map_data; }
		RenderDataTrail& GetTrailData() { return m_trail_data; }
		RenderDataSpace& GetSpaceData() { return m_space_data; }
		RenderDataParticle& GetParticleData() { return m_particle_data; }
		// helper for loading textures ====
		void LoadSpriteConfig(SpriteConfigInput& input) const;
	private:
		void UpdateDrawData();

		void DrawPassTrail();
		void DrawPassDefault();
		void DrawPassCombineReflection();
		void DrawPassPostProcess();

		Camera m_camera;
		RenderData m_batch_data;
		RenderDataTileMap m_tile_map_data;
		RenderDataTrail m_trail_data;
		RenderDataSpace m_space_data;
		RenderDataParticle m_particle_data;

		unsigned int m_default_vao;
		FrameBuffer m_frame_buffer;
		FrameBuffer m_frame_buffer_trail;
		FrameBuffer m_frame_buffer_reflection;
		FrameBuffer m_frame_buffer_screen;
		ReflectionConfig m_reflection_config;
	};
}