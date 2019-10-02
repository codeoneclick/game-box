#pragma once

#include "control.h"
#include "input_context.h"

namespace gb
{
	namespace ui
	{
		class interaction_control : public control
		{
		public:

            typedef std::function<void(const ces_entity_shared_ptr&)> t_on_pressed_callback;
			typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&)> t_on_drag_started_callback;
			typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&)> t_on_dragging_callback;
			typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&)> t_on_drag_ended_callback;

		private:

		protected:

			std::string m_dragged_callback_guid;
			bool m_is_dragging = false;

            t_on_pressed_callback m_on_pressed_callback = nullptr;
			t_on_drag_started_callback m_on_drag_started_callback = nullptr;
			t_on_dragging_callback m_on_dragging_callback = nullptr;
			t_on_drag_ended_callback m_on_drag_ended_callback = nullptr;

			void on_touch_size_changed(const glm::vec2& size);

			virtual void on_touched(const ces_entity_shared_ptr&,
									const glm::vec2& touch_point,
									e_input_source input_source,
									e_input_state input_state);

			virtual void on_dragging(const ces_entity_shared_ptr&,
									 const glm::vec2& touch_point,
									 e_input_source input_source,
									 e_input_state input_state);

		public:

			interaction_control(const scene_fabricator_shared_ptr& fabricator);
			virtual ~interaction_control();

			virtual void setup_components() override;

			virtual void create() override;

			void set_on_drag_started_callback(const t_on_drag_started_callback& callback);
			void set_on_dragging_callback(const t_on_dragging_callback& callback);
			void set_on_drag_ended_callback(const t_on_drag_started_callback& callback);
            
            virtual void set_on_pressed_callback(const t_on_pressed_callback& callback);
            virtual bool is_pressed_callback_exist() const;
            
            virtual void disable(bool value) override;
		};
	};
};
