#pragma once
#include "game_object.h"
#include "resource/stage_config.h"
#include "math/rect.h"

namespace foo
{
	class Player;
	class Room;
	class Parallax;
	class Fade;
	class Hint;
	class Stage : public GameObject
	{
	public:
		enum class StageState
		{
			ACTIVE,
			TRANSFER,
			FADE,
			EXIT,
			END
		};
		Stage(const StageConfig& stage_config);
		void Initialize() override;
		void Update() override;
		bool GetStageActive() const { return m_state == StageState::ACTIVE; }
		void EnterFadeReset();
		void EnterExit();
	private:
		static constexpr int MAX_ROOM_LOADED{};
		static constexpr int MAX_ROOM_ACTIVE{ 2 };
		enum class RoomState
		{
			DEAD, // nullptr
			IDLE, // created, not added to object list
			ACTIVE // added to object list
		};
		void LoadStageConfig();
		// room loading controller
		void SetActiveRoom(int room_id);
		void PreSetActiveRoom(int room_id);
		void PostSetActiveRoom(int room_id);
		// room loading utils
		int GetRoomDistance(int id_a, int id_b);
		int GetScopeId(int id);
		void SetRoomIdle(int room_id);
		void SetRoomActive(int room_id);
		void SetRoomDead(int room_id);
		// state updates
		void UpdateCheckRoomTransfer();
		void UpdateCameraFollow();
		void UpdateCameraTransfer();
		void UpdateFade();
		void UpdateRoomBgm();
		// reset
		void ResetRoom();
		// state
		void EnterEnd();
		// hard coding
		int GetJoinRoomId(int active_room_id, int enter_room_id);
		Rect GetRoomBoundingBox(int id);
		Rect GetViewBoundingBox(int active_room_id, int enter_room_id);
		const StageConfig& m_stage_config;
		// camera follow
		Rect m_room_bbox{};
		Rect m_view_bbox{};
		// room loading
		int m_initial_room_id{ -1 };
		int m_active_room_id{ -1 };
		int m_enter_room_id{ -1 };
		Room* m_rooms[StageConfig::GRID_CNT]{};
		RoomState m_room_states[StageConfig::GRID_CNT]{};
		Player* m_player{ nullptr };
		Parallax* m_parallax{ nullptr };
		Fade* m_fade{ nullptr };
		Hint* m_hint{ nullptr };
		bool m_last_room_rain{ false };
		// states
		StageState m_state{ StageState::ACTIVE };
		int m_exit_countdown{ 0 };
	};
}