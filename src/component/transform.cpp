#include "transform.h"

namespace foo
{
	void Transform::AddChild(Transform* child)
	{
		if (!child)
		{
			return;
		}
		if (!m_child_first)
		{
			m_child_first = child;
			m_child_last = child;
		}
		else
		{
			m_child_last->m_sibling = child;
			m_child_last = child;
		}
		child->m_parent = this;
		child->MarkDirty();
	}

	void Transform::MarkDirty()
	{
		if (m_dirty)
		{
			return;
		}
		m_dirty = true;

		Transform* child = m_child_first;
		while (child)
		{
			child->MarkDirty();
			child = child->m_sibling;
		}
	}

	void Transform::RecalculateCached()
	{
		Matrix4x4 local{ m_position, m_scale, m_rotation };
		if (m_parent)
		{
			m_matrix_cache = m_parent->GetMatrix() * local;
			RecalculateTransform2D();
		}
		else {
			m_matrix_cache = local;
			m_transform2d_cache = {
				Vector2{ m_position.x, m_position.y },
				Vector2{ m_scale.x, m_scale.y },
				m_rotation
			};
		}
		// RecalculateTransform2D();
		m_dirty = false;
	}

	//void Transform::RecalculateTransform2D()
	//{
	//    // no hierarchy 
	//    if (!m_parent)
	//    {
	//        m_transform2d_cache = {
	//            Vector2{ m_position.x, m_position.y },
	//            Vector2{ m_scale.x, m_scale.y },
	//            m_rotation
	//        };
	//        return;
	//    }
	//    // with hierarchy, get from matrix
	//    Vector2 x_axis{ m_matrix_cache[0][0], m_matrix_cache[1][0] };
	//    Vector2 y_axis{ m_matrix_cache[0][1], m_matrix_cache[1][1] };
	//    float scale_x = x_axis.Length();
	//    float scale_y = y_axis.Length();
	//    float rotation = atan2(x_axis.x, x_axis.y);
	//    m_transform2d_cache = {
	//        Vector2{ m_matrix_cache[0][3],  m_matrix_cache[1][3] },
	//        Vector2{ scale_x, scale_y },
	//        rotation
	//    };
	//}
	void Transform::RecalculateTransform2D()
	{
		// use matrix for position
		// avoid calculating arctan
		const Transform2D& parent_trans2d = m_parent->GetTransform2D();
		m_transform2d_cache = {
			Vector2{ m_matrix_cache[0][3],  m_matrix_cache[1][3] },
			Vector2{ parent_trans2d.scale.x * m_scale.x, parent_trans2d.scale.y * m_scale.y },
			parent_trans2d.rotation + m_rotation
		};
	}
}

