#include "application.h"

#include "global_context.h"
#include "platform/sound.h"
#include "platform/system.h"
#include "platform/controller.h"
#include "platform/timer.h"
#include "platform/log.h"
#include "render/render_system.h"
#include "physics/physics_system.h"
#include "scene/scene.h"

namespace foo
{
	void Application::Initialize()
	{
		g_global_context.StartSystems();

		g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_TITLE);
	}

	void Application::Run()
	{
		while (!g_global_context.m_system->ShouldClose())
		{
			if (g_global_context.m_timer->CheckUpdateNextFrame()) // 60fps
			{
				Update();
				Draw();
			}
		}
	}

	void Application::Finalize()
	{
		g_global_context.ShutdownSystems();
	}

	void Application::Update()
	{
		auto start = std::chrono::high_resolution_clock::now();

		g_global_context.m_sound->Update();
		g_global_context.m_controller->Update();

		// game update
		g_global_context.m_scene_manager->Update(); // object scripts
		g_global_context.m_physics_system->Update(); // solve collisions

		auto finish = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
		FOO_LOG("upadte took %d ms\n", duration.count());
	}

	void Application::Draw()
	{
		auto start = std::chrono::high_resolution_clock::now();

		g_global_context.m_render_system->ClearBuffers();
		g_global_context.m_render_system->Draw();
		g_global_context.m_system->SwapBuffers();

		auto finish = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
		FOO_LOG("render took %d ms\n", duration.count());
	}
}