#include "frame_buffer.h"

#include "platform/graphics_gl.h"
#include "platform/assert.h"

namespace foo
{
	void FrameBuffer::Initialize(unsigned int width, unsigned int height)
	{
        m_width = width;
        m_height = height;

        // Create color texture
        glCreateTextures(GL_TEXTURE_2D, 1, &m_color_texture);
        glTextureStorage2D(m_color_texture, 1, GL_RGBA8, width, height);
        glTextureParameteri(m_color_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_color_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_color_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_color_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create depth render buffer
        glCreateRenderbuffers(1, &m_rbo_depth);
        glNamedRenderbufferStorage(m_rbo_depth, GL_DEPTH_COMPONENT24, width, height);

        // Create framebuffer
        glCreateFramebuffers(1, &m_fbo);
        glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, m_color_texture, 0);
        glNamedFramebufferRenderbuffer(m_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_depth);

        // Check status
        GLenum status = glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            Finalize();
            FOO_ASSERT(false, "failed to create frame buffer");
        }
	}

	void FrameBuffer::Finalize()
	{
        if (m_fbo)
        {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
        if (m_color_texture)
        {
            glDeleteTextures(1, &m_color_texture);
            m_color_texture = 0;
        }
        if (m_rbo_depth)
        {
            glDeleteRenderbuffers(1, &m_rbo_depth);
            m_rbo_depth = 0;
        }
	}
    void FrameBuffer::Clear(const Vector4& color) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

