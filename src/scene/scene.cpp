#include "scene.h"

#include "scene_title.h"
#include "scene_game.h"
#include "scene_ending.h"

#include "global_context.h"
#include "physics/physics_system.h"
#include "render/render_system.h"

namespace foo
{
	void Scene::Finalize()
	{
		m_objects.Finalize();
		g_global_context.m_physics_system->Clear();
		g_global_context.m_render_system->ResetCamera();
	}

	void SceneManager::Initialize()
	{
		// set empty scene
		m_scene_current = new Scene();
		m_scene_current->Initialize();
	}

	void SceneManager::Finalize()
	{
		m_scene_current->Finalize();
	}

	void SceneManager::Update()
	{
		UpdateTransfer();
		m_scene_current->Update();
	}

	void SceneManager::Draw()
	{
		m_scene_current->Draw();
	}

	void SceneManager::SetNextScene(SceneName next)
	{
		m_scene_name_next = next;
	}
	void SceneManager::UpdateTransfer()
	{
		if (m_scene_name_next == SceneName::SCENE_NONE)
		{
			return;
		}
		// finalize current scene
		m_scene_current->Finalize();
		delete m_scene_current;
		
		// initialize next scene
		switch (m_scene_name_next)
		{
		case SceneName::SCENE_TITLE:
		{
			m_scene_current = new SceneTitle();
			break;
		}
		case SceneName::SCENE_GAME:
		{
			m_scene_current = new SceneGame();
			break;
		}
		case SceneName::SCENE_ENDING:
		{
			m_scene_current = new SceneEnding(); // TODO: TODO
			break;
		}
		}
		m_scene_current->Initialize();
		m_scene_name_current = m_scene_name_next;
		m_scene_name_next = SceneName::SCENE_NONE;
	}
}

