#include "scene_ending.h"

#include "main.h"
#include "global_context.h"
#include "platform/sound.h"
#include "platform/controller.h"
#include "object/background.h"
#include "object/ending.h"

#include "object/player.h"
#include "object/particle_system.h"
#include "object/trail_manager.h"
// object pools
#include "object/player_attack.h"
#include "object/enemy_attack.h"

namespace foo
{
	void SceneEnding::Initialize()
	{
		ParticleSystem* particle_system = new ParticleSystem();
		m_objects.AddObject(particle_system);

		PlayerAttackManager* player_attack_manager = new PlayerAttackManager();
		m_objects.AddObject(player_attack_manager);

		EnemyAttackManager* enemy_attack_manager = new EnemyAttackManager();
		m_objects.AddObject(enemy_attack_manager);

		TrailManager* trail_manager = new TrailManager();
		m_objects.AddObject(trail_manager);

		//Player* player = new Player();
		//m_objects.AddObject(player);


		m_objects.AddObject(new Ending());
		g_global_context.m_sound->PlayBgm(BgmName::BGM_TITLE);

	}
	void SceneEnding::Update()
	{
		// UpdateTitlebg();
		//if (g_global_context.m_controller->
		//	GetControllerTrigger(Button::BUTTON_PLUS))
		//{
		//	g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_GAME);

		//	g_global_context.m_sound->StopBgm();
		//	g_global_context.m_sound->PlaySe(WSD_SE_CLICK); // スタート音再生
		//}
		Scene::Update();
	}
	void SceneEnding::Finalize()
	{
		// FinalizeTitlebg();
		Scene::Finalize();
	}
}

