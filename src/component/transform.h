#pragma once
#include "main.h"
#include "math/matrix4x4.h"
#include "math/transform2d.h"

namespace foo
{
	class GameObject;
	class Transform {
	public:
		// setters
		void SetPosition(const Vector3& pos) { m_position = pos; MarkDirty(); }
		void SetScale(const Vector3& scale) { m_scale = scale; MarkDirty(); }
		void SetRotation(float rot) { m_rotation = rot; MarkDirty(); }
		void SetGameObject(GameObject* object) { m_object = object; };
		// getters
		const Vector3& GetPosition() const { return m_position; }
		const Vector3& GetScale() const { return m_scale; }
		float GetRotation() const { return m_rotation; }
		// matrix
		const Matrix4x4& GetMatrix() {
			if (m_dirty) {
				RecalculateCached();
			}
			return m_matrix_cache;
		}
		const Transform2D& GetTransform2D()
		{
			if (m_dirty) {
				RecalculateCached();
			};
			return m_transform2d_cache;
		}
		// tree
		void AddChild(Transform* child);
		const Transform* GetTransform() const { return m_parent; }
		Transform* GetTransform() { return m_parent; }
		Transform* GetChild() { return m_child_first; }
		Transform* GetSibling() { return m_sibling; }
		GameObject* GetGameObject() { return m_object; }
	private:
		void MarkDirty();
		void RecalculateCached();
		void RecalculateTransform2D();

		Vector3 m_position{ 0.0f, 0.0f, 0.0f };
		Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
		float m_rotation{ 0.0f };

		bool m_dirty{ true };
		Matrix4x4 m_matrix_cache;
		Transform2D m_transform2d_cache;
		// tree
		Transform* m_parent{ nullptr };
		Transform* m_sibling{ nullptr };
		Transform* m_child_first{ nullptr };
		Transform* m_child_last{ nullptr };
		// game object
		GameObject* m_object{ nullptr };
	};
}