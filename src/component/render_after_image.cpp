#include "render_after_image.h"

#include "global_context.h"
#include "render/texture.h"
#include "platform/timer.h"
#include "object/game_object.h"

namespace foo
{
	
	RenderAfterImage::RenderAfterImage(const AfterimageConfig& config) : m_config{ config }
	{
		m_active = false;
	}

	void RenderAfterImage::Update()
	{
		if (!m_active)
		{
			return;
		}
		// record every time delay
		if (Math::IsZero(m_countdown))
		{
			// record
			m_records[m_next_record_index] = {
				m_object->GetTransform().GetMatrix(),
				m_config.render_sprite->GetSpriteConfigOutput(), // TODO: order?
				true
			};
			// update countdown and index
			m_countdown = m_config.time_delay;
			m_next_record_index = (m_next_record_index + 1) % m_config.count;
			return;
		}
		// otherwise, update countdown
		m_countdown -= g_global_context.m_timer->GetFixedDeltaTime();
	}

	void RenderAfterImage::UpdateRenderData()
	{
		if (!m_active)
		{
			return;
		}
		RenderSystem* render_system = g_global_context.m_render_system;
		for (int i = 0; i < m_config.count; i++)
		{
			int record_index = (m_next_record_index - 1 - i + m_config.count) % m_config.count;
			if (!m_records[record_index].valid)
			{
				break; // the following won't be valid as well
			}
			SpriteConfigOutput output{ m_records[record_index].sprite_config_output };
			output.color = m_config.color;
			render_system->GetRenderData().Add(
				output,
				m_records[record_index].transform_mat
			);
		}	
	}
	void RenderAfterImage::SetActive(bool active)
	{
		if (!m_active && active)
		{
			ClearRecords(); // Clear previous data?
		}
		m_active = active;
	}
	void RenderAfterImage::ClearRecords()
	{
		for (int i = 0; i < m_config.count; i++)
		{
			m_records[i].valid = false;
		}
		m_next_record_index = 0; // no need actually
	}
}

