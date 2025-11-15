#include "scene_game.h"

#include "main.h"
#include "global_context.h"
#include "platform/sound.h"
#include "platform/controller.h"
#include "object/game_status.h"

//#include "object/game_object.h"
//#include "object/background.h"
#include "object/player.h"
#include "object/particle_system.h"
#include "object/trail_manager.h"
// object pools
#include "object/player_attack.h"
#include "object/enemy_attack.h"


// #include "object/tilemap.h"
// #include "object/room.h"
#include "object/stage.h"
#include "resource/stage_config.h"

namespace foo
{
	void SceneGame::Initialize()
	{
		// reset game state
		g_global_context.m_game_status->Initialize();

		g_global_context.m_sound->PlayBgm(BgmName::BGM_STAGE);

		ParticleSystem* particle_system = new ParticleSystem();
		m_objects.AddObject(particle_system);

		PlayerAttackManager* player_attack_manager = new PlayerAttackManager();
		m_objects.AddObject(player_attack_manager);

		EnemyAttackManager* enemy_attack_manager = new EnemyAttackManager();
		m_objects.AddObject(enemy_attack_manager);

		TrailManager* trail_manager = new TrailManager();
		m_objects.AddObject(trail_manager);

		Player* player = new Player();
		m_objects.AddObject(player);

		// background
		// m_objects.AddObject(new Background("bg_01.tga"));
		Stage* stage = new Stage(g_stage_config);
		m_objects.AddObject(stage);
		stage->Initialize();
		player->SetStage(stage);
	}
	void SceneGame::Update()
	{
		Scene::Update();
	}
	void SceneGame::Finalize()
	{
		Scene::Finalize();
	}
}
