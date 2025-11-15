#pragma once
#include "game_object.h"
#include "util/list.h"
#include "math/circle.h"
#include "physics/collision_trigger.h"
#include "render/sprite_config.h"

namespace foo
{
	class ObjectLightWeight
	{
	public:
		virtual ~ObjectLightWeight() = default;
		virtual void Update() {} // = 0;
		virtual bool IfDead() const { return false; } // = 0;
		// render
		virtual void UpdateRenderData() {} // = 0;
		// physics
		// virtual const Shape* GetShapePhysics() const { return nullptr; } // = 0;
		virtual bool HandleOverlap(
			const Rect& rect,
			CollisionTestLayer other_layer,
			GameObject* other_obj = nullptr
		) {
			return false;
		}

		// helper for updating and removing dead objects
		static void UpdateList(List& objects)
		{
			for (int i = 0; i < objects.GetCount(); i++)
			{
				ObjectLightWeight* object = (ObjectLightWeight*)objects[i];
				object->Update();
				if (object->IfDead())
				{
					delete object;
					objects.RemoveOrderedPend(i); // TODO: ordererd?
				}
			}
			objects.RemoveOrderedApply();
		}

		static void ClearList(List& objects)
		{
			for (int i = 0; i < objects.GetCount(); i++)
			{
				ObjectLightWeight* object = (ObjectLightWeight*)objects[i];
				delete object;
				objects.RemoveOrderedPend(i);
			}
			objects.RemoveOrderedApply();
		}
	};
}