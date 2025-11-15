#pragma once

#include "math/vector2.h"

class Ray
{
public:
    Ray() {}
    Ray(const Vector2& origin, const Vector2& direction) : m_origin(origin), m_dir(direction) {}

    const Vector2& GetOrigin() const { return m_origin; }
    const Vector2& GetDirection() const { return m_dir; }

    Vector2 At(float t) const
    {
        return m_origin + m_dir * t;
    }

private:
    Vector2 m_origin;
    Vector2 m_dir;
};