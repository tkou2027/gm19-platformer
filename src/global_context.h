#pragma once
namespace foo
{
	class Sound;
	class System;
	class RenderSystem;
	class ShaderManager;
	class Texture;
	class PhysicsSystem;
	class Controller;
	class Timer;

	class TileManager;
	class SceneManager;
	class GameStatus;

	class GlobalContext
	{
	public:
		void StartSystems();
		void ShutdownSystems();

		Sound* m_sound{ nullptr };
		System* m_system{ nullptr };
		RenderSystem* m_render_system{ nullptr };
		ShaderManager* m_shader_manager{ nullptr };
		Texture* m_texture{ nullptr };
		PhysicsSystem* m_physics_system{ nullptr };
		Controller* m_controller{ nullptr };
		Timer* m_timer{ nullptr };
		TileManager* m_tile_manager{ nullptr };
		SceneManager* m_scene_manager{ nullptr };
		GameStatus* m_game_status{ nullptr };
	};

	extern GlobalContext g_global_context;
}