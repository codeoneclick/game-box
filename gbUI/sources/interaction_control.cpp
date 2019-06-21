#include "interaction_control.h"
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
		interaction_control::interaction_control(const scene_fabricator_shared_ptr& fabricator) :
			gb::ui::control(fabricator),
			m_dragged_callback_guid("")
		{
			ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
		}

		interaction_control::~interaction_control()
		{

		}

		void interaction_control::setup_components()
		{
			control::setup_components();
			auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
			bound_touch_component->enable(e_input_state_pressed, e_input_source_mouse_left, true);
			bound_touch_component->enable(e_input_state_released, e_input_source_mouse_left, true);
			bound_touch_component->add_callback(e_input_state_pressed, std::bind(&interaction_control::on_touched, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4));
			bound_touch_component->add_callback(e_input_state_released, std::bind(&interaction_control::on_touched, this,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				std::placeholders::_4));
		}

		void interaction_control::create()
		{
			control::create();
		}

		void interaction_control::on_touch_size_changed(const glm::vec2& size)
		{
			auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
			bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
		}

		void interaction_control::on_touched(const ces_entity_shared_ptr& entity,
											 const glm::vec2& touch_point,
											 e_input_source input_source,
											 e_input_state input_state)
		{
			auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
			if (input_state == e_input_state_pressed)
			{
				bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, true);
				m_dragged_callback_guid = bound_touch_component->add_callback(e_input_state_dragged, std::bind(&interaction_control::on_dragging, this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4));
			}
			else if (input_state == e_input_state_released)
			{
				bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, false);
				bound_touch_component->remove_callback(e_input_state_dragged, m_dragged_callback_guid);

				if (m_on_drag_ended_callback && m_is_dragging)
				{
					m_on_drag_ended_callback(shared_from_this(), touch_point);
				}
			}
			m_is_dragging = false;
		}

		void interaction_control::on_dragging(const ces_entity_shared_ptr& entity,
											  const glm::vec2& touch_point,
											  e_input_source input_source,
											  e_input_state input_state)
		{
			if (!m_is_dragging && m_on_drag_started_callback)
			{
				m_on_drag_started_callback(shared_from_this(), touch_point);
			}
			m_is_dragging = true;

			if (m_on_dragging_callback)
			{
				m_on_dragging_callback(shared_from_this(), touch_point);
			}
		}

		void interaction_control::set_on_drag_started_callback(const t_on_drag_started_callback& callback) {
			m_on_drag_started_callback = callback;
		}

		void interaction_control::set_on_dragging_callback(const t_on_dragging_callback& callback) {
			m_on_dragging_callback = callback;
		}

		void interaction_control::set_on_drag_ended_callback(const t_on_drag_started_callback& callback) {
			m_on_drag_ended_callback = callback;
		}
	}
}
