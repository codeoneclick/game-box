#pragma once
//
//  button.h
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "input_context.h"

namespace gb
{
	namespace ui
	{
		class panel : public interaction_control
		{
		public:

			std::string k_header_element_name = "ui_header_element";

		private:

			glm::u8vec4 m_background_color;
			glm::u8vec4 m_header_color;
			f32 m_header_height;

			glm::vec2 m_touch_offset = glm::vec2(0.f);

		protected:

			virtual void on_touched(const ces_entity_shared_ptr& entity,
									const glm::vec2& touch_point,
									e_input_source input_source,
									e_input_state input_state) override;

			virtual void on_dragging(const ces_entity_shared_ptr&,
									 const glm::vec2& touch_point,
									 e_input_source input_source,
									 e_input_state input_state) override;

			e_element_horizontal_aligment m_horizontal_aligment;
			e_element_vertical_aligment m_vertical_aligment;

		public:

			panel(const scene_fabricator_shared_ptr& fabricator);
			virtual ~panel();

			virtual void setup_components() override;

			virtual void create() override;

			void set_text(const std::string& text);
			std::string get_text();

			void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
			void set_text_vertical_aligment(e_element_vertical_aligment aligment);

			void set_background_color(const glm::u8vec4& color) override;
			void set_header_color(const glm::u8vec4& color);
		};
	};
};
