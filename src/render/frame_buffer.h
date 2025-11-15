#pragma once
#include "math/vector4.h"

namespace foo
{
    class FrameBuffer
    {
    public:
        void Initialize(unsigned int width, unsigned int height);
        void Finalize();
        void Clear(const Vector4& color) const;

        unsigned int GetFbo() const { return m_fbo; }
        unsigned int GetColorTexture() const { return m_color_texture; }
        unsigned int GetRboDepth() const { return m_rbo_depth; }
        unsigned int GetWidth() const { return m_width; }
        unsigned int GetHeight() const { return m_height; }
    private:
        unsigned int m_fbo{ 0 };
        unsigned int m_color_texture{ 0 };
        unsigned int m_rbo_depth{ 0 };
        unsigned int m_width{ 0 };
        unsigned int m_height{ 0 };
    };
}

