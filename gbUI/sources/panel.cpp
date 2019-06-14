#include "panel.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_font_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"
#include "ces_sound_component.h"

namespace gb
{
	namespace ui
	{
		panel::panel(const scene_fabricator_shared_ptr& fabricator) :
			gb::ui::interaction_control(fabricator),
			m_horizontal_aligment(e_element_horizontal_aligment_center),
			m_vertical_aligment(e_element_vertical_aligment_center),
			m_background_color(control::k_light_gray_color),
			m_header_color(control::k_dark_gray_color),
			m_header_height(16.f)
		{
			ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();

			size.setter([=](const glm::vec2& size) {

				m_size = size;
				interaction_control::on_touch_size_changed(m_size);

				std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
				std::static_pointer_cast<gb::sprite>(m_elements[k_header_element_name])->size = glm::vec2(size.x, std::min(size.y - size.y * .1f, m_header_height));
				std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = std::max(16.f * .75f, std::min(size.y * .75f, 32.f * .75f));

				control::set_element_horizontal_aligment(m_elements[k_header_element_name], e_element_horizontal_aligment::e_element_horizontal_aligment_center);
				control::set_element_vertical_aligment(m_elements[k_header_element_name], e_element_vertical_aligment::e_element_vertical_aligment_top);

				control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment, std::static_pointer_cast<gb::sprite>(m_elements[k_header_element_name])->size);
				control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment, std::static_pointer_cast<gb::sprite>(m_elements[k_header_element_name])->size);
			});
		}

		panel::~panel()
		{

		}

		void panel::setup_components()
		{
			interaction_control::setup_components();
		}

		void panel::create()
		{
			gb::sprite_shared_ptr background_element = control::get_fabricator()->create_sprite("button_background.xml");
			m_elements[k_background_element_name] = background_element;
			control::set_color(k_background_element_name, m_background_color);
			panel::add_child(background_element);

			gb::sprite_shared_ptr header_element = control::get_fabricator()->create_sprite("button_background.xml");
			m_elements[k_header_element_name] = header_element;
			control::set_color(k_header_element_name, m_header_color);
			panel::add_child(header_element);

			gb::label_shared_ptr label_element = control::get_fabricator()->create_label_2d("button_label.xml");
			m_elements[k_label_element_name] = label_element;
			panel::add_child(label_element);

			interaction_control::create();

			control::set_element_horizontal_aligment(m_elements[k_header_element_name], e_element_horizontal_aligment::e_element_horizontal_aligment_center);
			control::set_element_vertical_aligment(m_elements[k_header_element_name], e_element_vertical_aligment::e_element_vertical_aligment_top);

			control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
			control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
		}

		void panel::on_touched(const ces_entity_shared_ptr& entity,
							   const glm::vec2& touch_point,
							   e_input_source input_source,
							   e_input_state input_state)
		{
			interaction_control::on_touched(entity, touch_point, input_source, input_state);
			auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
			if (input_state == e_input_state_pressed)
			{
				glm::mat4 mat_m = gb::ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
				glm::vec2 min_bound = glm::transform(glm::vec2(0.f), mat_m);
				m_touch_offset = touch_point - min_bound;
			}
			else if (input_state == e_input_state_released)
			{

			}
		}

		void panel::on_dragging(const ces_entity_shared_ptr& entity,
								const glm::vec2& touch_point,
								e_input_source input_source,
								e_input_state input_state)
		{
			interaction_control::on_dragging(entity, touch_point, input_source, input_state);

			glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
			glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
				mat_m);
			glm::vec2 header_size = m_elements[k_header_element_name]->size;
			glm::vec2 max_bound = glm::transform(header_size, mat_m);
			glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);

			if (glm::intersect(bound, touch_point))
			{
				if (glm::intersect(bound, touch_point))
				{
					panel::position = glm::vec2(touch_point.x - m_touch_offset.x,
						touch_point.y - m_touch_offset.y);
				}
			}
		}

		void panel::set_text(const std::string& text)
		{
			std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text = text;

			control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
			control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
		}

		std::string panel::get_text()
		{
			return std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
		}

		void panel::set_text_horizontal_aligment(e_element_horizontal_aligment aligment)
		{
			control::set_element_horizontal_aligment(m_elements[k_label_element_name], aligment);
			m_horizontal_aligment = aligment;
		}

		void panel::set_text_vertical_aligment(e_element_vertical_aligment aligment)
		{
			control::set_element_vertical_aligment(m_elements[k_label_element_name], aligment);
			m_vertical_aligment = aligment;
		}

		void panel::set_background_color(const glm::u8vec4& color)
		{
			m_background_color = color;
			control::set_background_color(color);
		}

		void panel::set_header_color(const glm::u8vec4& color)
		{
			m_header_color = color;
			control::set_color(k_header_element_name, color);
		}
	}
}
