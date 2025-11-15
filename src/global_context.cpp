#include "global_context.h"
#include "main.h"
// systems ====
#include "platform/sound.h"
#include "platform/system.h"
// render system
#include "render/render_system.h"
#include "render/shaders.h"
#include "render/texture.h"
// physics system
#include "physics/physics_system.h"
#include "platform/controller.h"
#include "platform/timer.h"
// game ====
#include "resource/tile.h"
#include "scene/scene.h"
#include "object/game_status.h"

namespace foo
{
	GlobalContext g_global_context;

	void GlobalContext::StartSystems()
	{
		m_sound = new Sound();
		m_sound->Initialize();

		m_system = new System();
		m_system->Initialize();

		m_render_system = new RenderSystem();
		m_render_system->Initialize();

		m_shader_manager = new ShaderManager();
		m_shader_manager->Initialize();

		m_texture = new Texture();
		m_texture->Initialize();

		m_physics_system = new PhysicsSystem();
		m_physics_system->Initialize();

		m_controller = new Controller();
		m_controller->Initialize();

		m_timer = new Timer();
		m_timer->Initialize();

		m_tile_manager = new TileManager();
		m_tile_manager->Initialize();

		m_scene_manager = new SceneManager();
		m_scene_manager->Initialize();

		m_game_status = new GameStatus();
		m_game_status->Initialize();
	}
	void GlobalContext::ShutdownSystems()
	{
		m_game_status->Finalize();
		delete m_game_status;

		m_scene_manager->Finalize();
		delete m_scene_manager;

		m_tile_manager->Finalize();
		delete m_tile_manager;

		m_controller->Finalize();
		delete m_controller;

		m_physics_system->Finalize();
		delete m_physics_system;

		m_texture->Finalize();
		delete m_texture;

		m_shader_manager->Finalize();
		delete m_shader_manager;

		m_render_system->Finalize();
		delete m_render_system;

		m_system->Finalize();
		delete m_system;

		m_sound->Finalize();
		delete m_sound;
	}
}