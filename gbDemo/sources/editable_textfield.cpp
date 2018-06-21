#include "editable_textfield.h"
#include "ces_transformation_extension.h"
#include "glm_extensions.h"
#include "ces_bound_touch_component.h"
#include "sprite.h"
#include "label.h"
#include "scene_fabricator.h"

namespace game
{
	editable_textfield::editable_textfield(const gb::scene_fabricator_shared_ptr& fabricator) : gb::ui::textfield(fabricator)
	{
		size.setter([=](const glm::vec2& size) {

			m_size = size;
			auto bound_touch_component = gb::ces_entity::get_component<gb::ces_bound_touch_component>();
			bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));

			std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
			std::static_pointer_cast<gb::sprite>(m_elements[k_foreground_element_name])->size = glm::vec2(size.x - 4.f, size.y - 4.f);
			std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = size.y * .75f;

			control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
			control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);

			control::set_element_horizontal_aligment(m_elements[k_textfield_resize_element_name], gb::ui::e_element_horizontal_aligment_right);
			control::set_element_vertical_aligment(m_elements[k_textfield_resize_element_name], gb::ui::e_element_vertical_aligment_down);

			control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], gb::ui::e_element_horizontal_aligment_center);
			control::set_element_vertical_aligment(m_elements[k_foreground_element_name], gb::ui::e_element_vertical_aligment_center);
		});
	}

	editable_textfield::~editable_textfield()
	{

	}

	void editable_textfield::on_touched(const gb::ces_entity_shared_ptr& entity,
		const glm::vec2& touch_point,
		gb::e_input_source input_source,
		gb::e_input_state input_state)
	{
		textfield::on_touched(entity, touch_point, input_source, input_state);
		if (input_state == gb::e_input_state_pressed)
		{
			glm::mat4 mat_m = gb::ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
			glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
				mat_m);
			m_touch_offset = touch_point - min_bound;
			m_is_resizing = false;

			mat_m = gb::ces_transformation_extension::get_absolute_transformation_in_ws(m_elements[k_textfield_resize_element_name]);
			min_bound = glm::transform(glm::vec2(0.f),
				mat_m);
			glm::vec2 max_bound = glm::transform(glm::vec2(8.f),
				mat_m);
			glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
			if (glm::intersect(bound, touch_point))
			{
				m_previous_resize_point = touch_point;
				m_is_resizing = true;
			}
		}
	}

	void editable_textfield::on_dragging(const gb::ces_entity_shared_ptr& entity,
		const glm::vec2& touch_point,
		gb::e_input_source input_source,
		gb::e_input_state input_state)
	{
		glm::vec2 size = control::size;

		if (m_is_resizing)
		{
			glm::vec2 delta = touch_point - m_previous_resize_point;
			editable_textfield::size = glm::vec2(std::max(size.x + delta.x, k_button_min_size.x), std::max(size.y + delta.y, k_button_min_size.y));
			m_previous_resize_point = touch_point;
		}
		else
		{
			glm::mat4 mat_m = gb::ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
			glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
				mat_m);
			glm::vec2 max_bound = glm::transform(size,
				mat_m);
			glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);

			if (glm::intersect(bound, touch_point))
			{
				textfield::position = glm::vec2(touch_point.x - m_touch_offset.x,
												touch_point.y - m_touch_offset.y);
			}
		}
	}

	void editable_textfield::create() {

		gb::sprite_shared_ptr textfield_resize_element = gb::ui::control::get_fabricator()->create_sprite("textfield_background.xml");
		textfield_resize_element->position = glm::vec2(0.f, 0.f);
		textfield_resize_element->size = glm::vec2(8.f);
		m_elements[k_textfield_resize_element_name] = textfield_resize_element;

		textfield::create();

		textfield::add_child(textfield_resize_element);
	}
}