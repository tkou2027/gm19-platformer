#include "stage.h"
// managed objects
#include "player.h"
#include "sword.h"
#include "room.h"
#include "parallax.h"
#include "fan.h"
#include "item.h"
#include "fade.h"
#include "hint.h"
#include "rain.h" 
#include "enemy.h"
#include "space_block.h"
#include "player_attack.h"
#include "trail_manager.h"
// global systems
#include "global_context.h"
#include "platform/sound.h"
#include "scene/scene.h"
#include "object/game_status.h"

namespace foo
{
	Stage::Stage(const StageConfig& stage_config)
		: m_stage_config{ stage_config }
	{
		m_player = (Player*)GetNamedObject(ObjectName::PLAYER);

		Sword* sword = new Sword();
		sword->SetPlayer(m_player);
		AddObject(sword);

		// background
		m_parallax = new Parallax();
		AddObject(m_parallax);

		m_fade = new Fade({});
		AddObject(m_fade);

		// item manager
		FanManager* fan_manager = new FanManager();
		AddObject(fan_manager);
		ItemManager* item_manager = new ItemManager();
		AddObject(item_manager);

		m_hint = new Hint();
		AddObject(m_hint);

		// rooms
		LoadStageConfig();

		// set named object
		m_name = ObjectName::STAGE;
		SetNamedObject(m_name, this);
	}

	void Stage::Initialize()
	{
		// first is 10
		m_initial_room_id = 10;
		// activate rooms
		PreSetActiveRoom(m_initial_room_id);
		PostSetActiveRoom(m_initial_room_id);
		Camera& camera = g_global_context.m_render_system->GetCamera();
		camera.SetOffset(m_room_bbox.GetCenter());
		m_player->InitializeRevivePosition(m_rooms[m_initial_room_id]->GetSavePosition());
		m_player->SetIdle();

		// enter fade
		m_fade->StartFadeIn();
		m_state = StageState::FADE;
	}

	void Stage::Update()
	{
		// camera follow
		switch (m_state)
		{
		case StageState::ACTIVE:
		{
			UpdateCheckRoomTransfer();
			break;
		}
		case StageState::TRANSFER:
		{
			UpdateCameraTransfer();
			break;
		}
		case StageState::FADE:
		{
			UpdateFade();
			break;
		}
		case StageState::EXIT:
		{
			if (m_exit_countdown > 0)
			{
				m_exit_countdown--;
			}
			else if (m_exit_countdown == 0)
			{
				m_fade->StartFadeOut();
				m_exit_countdown = -1;
			}
			else
			{
				if (m_fade->IsIdle())
				{
					EnterEnd();
				}
			}
			break;
		}
		}
	}

	void Stage::EnterFadeReset()
	{
		m_fade->StartFadeIn();
		m_player->SetIdle();
		m_state = StageState::FADE;
	}

	void Stage::EnterExit()
	{
		if (m_state == StageState::EXIT || m_state == StageState::END)
		{
			return;
		}
		m_exit_countdown = 60.0f;
		m_state = StageState::EXIT;
	}

	void Stage::EnterEnd()
	{
		if (m_last_room_rain)
		{
			g_global_context.m_sound->StopBgm();
		}
		g_global_context.m_sound->StopBgm();
		g_global_context.m_scene_manager->SetNextScene(SceneManager::SceneName::SCENE_ENDING);
		g_global_context.m_sound->StopBgm();
		m_state = StageState::END;
	}

	void Stage::LoadStageConfig()
	{
		// init
		for (int i = 0; i < StageConfig::GRID_CNT; i++)
		{
			m_rooms[i] = nullptr;
			m_room_states[i] = RoomState::DEAD;
		}
	}
	void Stage::SetActiveRoom(int room_id)
	{
		if (GetScopeId(room_id) == GetScopeId(m_active_room_id))
		{
			PreSetActiveRoom(room_id);
			PostSetActiveRoom(room_id);
		}
		else
		{
			PreSetActiveRoom(room_id);
			m_state = StageState::TRANSFER;
		}
	}
	void Stage::PreSetActiveRoom(int room_id)
	{
		SetRoomActive(room_id);
		m_enter_room_id = m_active_room_id;
		m_active_room_id = room_id;
		m_room_bbox = m_rooms[room_id]->GetBoundingBox();
		m_view_bbox = GetViewBoundingBox(m_active_room_id, m_enter_room_id);
		m_player->SetRevivePosition(m_rooms[room_id]->GetSavePosition());
	}

	void Stage::PostSetActiveRoom(int room_id)
	{
		for (int i = 0; i < StageConfig::GRID_CNT; i++)
		{
			if (i == room_id)
			{
				continue;
			}
			// loading neighbours
			int dist = GetRoomDistance(room_id, i);
			if (GetScopeId(room_id) == GetScopeId(i))
			{
				if (m_room_states[i] != RoomState::ACTIVE)
				{
					SetRoomActive(i);
				}
			}
			else if (dist <= 1)
			{
				SetRoomIdle(i);
			}
			else if (dist >= 2 && m_room_states[i] != RoomState::DEAD)
			{
				SetRoomDead(i);
			}
		}
		UpdateRoomBgm();
	}

	int Stage::GetRoomDistance(int id_a, int id_b)
	{
		int row_a = id_a / StageConfig::GRID_COLS;
		int col_a = id_a % StageConfig::GRID_COLS;
		int row_b = id_b / StageConfig::GRID_COLS;
		int col_b = id_b % StageConfig::GRID_COLS;
		return abs(row_a - row_b) + abs(col_a - col_b);
	}

	int Stage::GetScopeId(int id)
	{
		return m_stage_config.grids[id].scope_id;
	}

	void Stage::SetRoomIdle(int room_id)
	{
		switch (m_room_states[room_id])
		{
		case RoomState::DEAD:
		{
			m_rooms[room_id] = new Room(m_stage_config, room_id);
			AddObject(m_rooms[room_id]);
			m_rooms[room_id]->Initialize();
			break;
		}
		case RoomState::ACTIVE:
		{
			m_rooms[room_id]->SetIdle();
			break;
		}
		}
		m_room_states[room_id] = RoomState::IDLE;
	}

	void Stage::SetRoomActive(int room_id)
	{
		switch (m_room_states[room_id])
		{
		case RoomState::DEAD:
		{
			SetRoomIdle(room_id);
			// no break here // break;
		}
		case RoomState::IDLE:
		{
			m_rooms[room_id]->SetActive();
			break;
		}
		}
		m_room_states[room_id] = RoomState::ACTIVE;
	}

	void Stage::SetRoomDead(int room_id)
	{
		switch (m_room_states[room_id])
		{
		case RoomState::ACTIVE:
		{
			SetRoomIdle(room_id);
			// no break here // break;
		}
		case RoomState::IDLE:
		{
			m_rooms[room_id]->SetRemoved(); // to be removed by game object list
			m_rooms[room_id] = nullptr;
			break;
		}
		}
		m_room_states[room_id] = RoomState::DEAD;
	}

	void Stage::UpdateCheckRoomTransfer()
	{
		const Vector2& player_pos = m_player->GetTransform().GetTransform2D().position;

		UpdateCameraFollow();

		// test room transfer
		int curr_col{ m_active_room_id % StageConfig::GRID_COLS };
		int curr_row{ m_active_room_id / StageConfig::GRID_COLS };
		if (player_pos.x > m_room_bbox.GetRight() && curr_col < StageConfig::GRID_COLS - 1)
		{
			SetActiveRoom(m_active_room_id + 1);
		}
		else if (player_pos.x < m_room_bbox.GetLeft() && curr_col > 0)
		{
			SetActiveRoom(m_active_room_id - 1);
		}
		else if (player_pos.y > m_room_bbox.GetBottom() && curr_row < StageConfig::GRID_ROWS - 1)
		{
			SetActiveRoom(m_active_room_id + StageConfig::GRID_COLS);
		}
		else if (player_pos.y < m_room_bbox.GetTop() && curr_row > 0)
		{
			SetActiveRoom(m_active_room_id - StageConfig::GRID_COLS);
		}
	}
	void Stage::UpdateCameraFollow()
	{
		Camera& camera = g_global_context.m_render_system->GetCamera();
		const auto camera_half_size = camera.GetBoundingBox().GetHalfSize();
		const Vector2& player_pos = m_player->GetTransform().GetTransform2D().position;
		Vector2 target_focus = player_pos;

		bool vertical = m_view_bbox.GetHalfSize().y > camera_half_size.y * 1.5f;
		float vertical_border = m_stage_config.TILE_SIZE.y * 0.25f;
		if (vertical)
		{
			target_focus.y = player_pos.y - 150.0f;
			target_focus.y = Math::Max(target_focus.y, m_view_bbox.GetTop() + camera_half_size.y + vertical_border);
			target_focus.y = Math::Min(target_focus.y, m_view_bbox.GetBottom() - camera_half_size.y - vertical_border);
			target_focus.x = m_view_bbox.GetCenter().x;
			camera.MoveTo(target_focus);
		}
		else
		{
			target_focus.x = Math::Max(target_focus.x, m_view_bbox.GetLeft() + camera_half_size.x);
			target_focus.x = Math::Min(target_focus.x, m_view_bbox.GetRight() - camera_half_size.x);
			target_focus.y = m_view_bbox.GetCenter().y;
			camera.SetOffset(target_focus);
		}
	}

	void Stage::UpdateCameraTransfer()
	{
		Camera& camera = g_global_context.m_render_system->GetCamera();
		const Vector2& camera_pos = camera.GetOffsetNoShake();
		const Vector2 target_focus = m_room_bbox.GetCenter(); // TODO;
		const float dist = (camera_pos - target_focus).Length();
		if (Math::IsZero(dist))
		{
			camera.SetOffset(target_focus);
			PostSetActiveRoom(m_active_room_id);
			m_state = StageState::ACTIVE;
			return;
		}
		camera.MoveTo(target_focus, Math::Max(40.0f, dist * 0.4f));
	}

	void Stage::UpdateFade()
	{
		if (m_fade->IsIdle())
		{
			m_state = StageState::ACTIVE;
			ResetRoom();
			if (m_active_room_id == m_initial_room_id)
			{
				m_hint->ShowHint(Hint::HintType::MOVE, m_room_bbox.GetCenter());
			}
		}
	}

	void Stage::UpdateRoomBgm()
	{
		// play and stop rain bgm
		if (m_rooms[m_active_room_id]->GetHasRain())
		{
			if (!m_last_room_rain)
			{
				g_global_context.m_sound->PlayBgm(BgmName::BGM_RAIN);
			}
			m_last_room_rain = true;
		}
		else
		{
			if (m_last_room_rain)
			{
				g_global_context.m_sound->StopBgm();
			}
			m_last_room_rain = false;
		}
	}
	void Stage::ResetRoom()
	{
		m_player->SetActive();
		for (int i = 0; i < StageConfig::GRID_CNT; i++)
		{
			if (m_room_states[i] == RoomState::ACTIVE)
			{
				m_rooms[i]->SetActive();
			}
		}
	}
	int Stage::GetJoinRoomId(int active_room_id, int enter_room_id)
	{
		// auto ===
		int join_cnt{ 0 };
		int join_id{ -1 };
		const int offset[]{ 0, -1, 0, 1, 0 }; // four neighbour directions
		const int active_scope_id{ GetScopeId(active_room_id) };
		for (int i = 0; i < 4; i++)
		{
			int next_col = active_room_id % StageConfig::GRID_COLS + offset[i];
			int next_row = active_room_id / StageConfig::GRID_COLS + offset[i + 1];
			if (next_col < 0 || next_col >= StageConfig::GRID_COLS
				|| next_row < 0 || next_row >= StageConfig::GRID_ROWS)
			{
				continue;
			}
			const int next_id{ next_row * StageConfig::GRID_COLS + next_col };
			if (GetScopeId(next_id) == active_scope_id)
			{
				join_cnt++;
				join_id = next_id;
			}
		}
		// one or none found, return id
		if (join_cnt <= 1)
		{
			return join_id;
		}
		// multiple connected
		// enter room id
		if (GetScopeId(enter_room_id) == active_scope_id)
		{
			return enter_room_id;
		}
		// hard coding ===
		if (active_room_id == 15)
		{
			if (enter_room_id == 14)
			{
				return 14;
			}
			else if (enter_room_id == 16 || enter_room_id == 6)
			{
				return 6;
			}
		}
		// undefined
		return join_id;
	}

	Rect Stage::GetRoomBoundingBox(int id)
	{
		int col = id % StageConfig::GRID_COLS;
		int row = id / StageConfig::GRID_COLS;
		const Vector2 position{ col * StageConfig::GRID_SIZE.x, row * StageConfig::GRID_SIZE.y };
		return Rect{ StageConfig::GRID_SIZE, position };
	}

	Rect Stage::GetViewBoundingBox(int active_room_id, int enter_room_id)
	{
		int join_room_id = GetJoinRoomId(active_room_id, enter_room_id);

		const Rect active_bbox = GetRoomBoundingBox(active_room_id);
		if (join_room_id < 0)
		{
			return active_bbox;
		}
		const Rect join_bbox = GetRoomBoundingBox(join_room_id);
		return active_bbox.GetCombine(join_bbox);
	}
}
