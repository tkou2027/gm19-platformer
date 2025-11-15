#pragma once
#include "object/game_object.h"
#include "render/camera.h"
namespace foo
{
	class GameObject;
	class Scene
	{
	public:
		virtual ~Scene() = default;
		virtual void Initialize() {};
		virtual void Update() { m_objects.Update(); };
		virtual void Draw() {};
		virtual void Finalize();
		GameObjectList& GetGameObjects() { return m_objects; }
		void SetNamedObject(ObjectName name, GameObject* object)
		{
			m_named_objects[static_cast<size_t>(name)] = object;
		}
		GameObject* GetNamedObject(ObjectName name) const
		{
			return m_named_objects[static_cast<size_t>(name)];
		}
		const Camera& GetCameraMain() const {
			return m_camera_main;
		};
		Camera& GetCameraMain() {
			return m_camera_main;
		};
	protected:
		// object pool
		static constexpr int OBJECT_MAX{ 128 };
		GameObjectList m_objects;
		// special (global) game objects
		GameObject* m_named_objects[static_cast<size_t>(ObjectName::MAX)];
		Camera m_camera_main;
	};

	class SceneManager
	{
	public:
		enum class SceneName
		{
			SCENE_NONE,
			SCENE_TITLE,
			SCENE_GAME,
			SCENE_ENDING
		};
		void Initialize();
		void Finalize();
		void Update();
		void Draw();
		Scene* GetCurrentScene() { return m_scene_current; };
		void SetNextScene(SceneName next);
	private:
		void UpdateTransfer();

		SceneName m_scene_name_current{ SceneName::SCENE_TITLE };
		SceneName m_scene_name_next{ SceneName::SCENE_NONE };
		Scene* m_scene_current;
	};
}