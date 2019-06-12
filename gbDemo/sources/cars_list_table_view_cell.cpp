//
//  cars_list_table_view_cell.cpp
//  gbDemo
//
//  Created by serhii.s on 5/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "cars_list_table_view_cell.h"
#include "button.h"
#include "textfield.h"
#include "sprite.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "material.h"
#include "scene_fabricator.h"
#include "ces_bound_touch_component.h"

namespace game
{
    namespace ui
    {
        cars_list_table_view_cell_data::cars_list_table_view_cell_data()
        {
            id.getter([=]() {
                return m_id;
            });
            
            id.setter([=](const std::string& value) {
                m_id = value;
            });
            
            name.getter([=]() {
                return m_name;
            });
            
            name.setter([=](const std::string& value) {
                m_name = value;
            });
            
            place.getter([=]() {
                return m_place;
            });
            
            place.setter([=](i32 value) {
                m_place = value;
            });
            
            drift_time.getter([=]() {
                return m_drift_time;
            });
            
            drift_time.setter([=](f32 value) {
                m_drift_time = value;
            });
        }
        
        const std::string cars_list_table_view_cell::k_name_label_id = "name_label";
        const std::string cars_list_table_view_cell::k_place_label_id = "place_label";
        const std::string cars_list_table_view_cell::k_drift_time_label_id = "drift_time_label";
        const std::string cars_list_table_view_cell::k_drift_time_value_label_id = "drift_time_value_label";
        
        cars_list_table_view_cell::cars_list_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::table_view_cell(fabricator, index, identifier)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements[control::k_background_element_name]->size = size;
            });
            
            name.getter([=]() {
                return "";
            });
            
            name.setter([=](const std::string& value) {
                std::static_pointer_cast<gb::ui::textfield>(m_elements[k_name_label_id])->set_text(value);
            });
            
            place.getter([=]() {
                return -1;
            });
            
            place.setter([=](i32 value) {
                std::stringstream place_str;
                place_str<<value<<".";
                std::static_pointer_cast<gb::ui::textfield>(m_elements[k_place_label_id])->set_text(place_str.str());
            });
            
            drift_time.getter([=]() {
                return -1.f;
            });
            
            drift_time.setter([=](f32 value) {
                i32 seconds = value / 1000;
                f32 f_milliseconds = value / 1000 - seconds;
                i32 milliseconds = f_milliseconds * 10;
                
                std::stringstream drift_value_string_stream;
                drift_value_string_stream<<(seconds < 10 ? "0" : "")<<seconds<<":"<<milliseconds<<"0";
                std::static_pointer_cast<gb::ui::textfield>(m_elements[k_drift_time_value_label_id])->set_text(drift_value_string_stream.str());
            });
        }
        
        void cars_list_table_view_cell::setup_components()
        {
            gb::ui::table_view_cell::setup_components();
            remove_component(gb::ces_bound_touch_component::class_guid());
        }
        
        void cars_list_table_view_cell::create()
        {
            gb::ui::table_view_cell::create();
            set_background_color(glm::u8vec4(96, 96, 96, 96));

            const auto name_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            name_label->create();
            name_label->size = glm::vec2(128.f, 14.f);
            name_label->set_text("Racer");
            name_label->position = glm::vec2(32.f, 0.f);
            name_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
            name_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            name_label->set_visible_edges(false);
            m_elements[k_name_label_id] = name_label;
            add_child(name_label);
            
            const auto place_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            place_label->create();
            place_label->size = glm::vec2(18.f, 18.f);
            place_label->set_text("1");
            place_label->position = glm::vec2(4.f, 0.f);
            place_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
            place_label->set_visible_edges(false);
            place_label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
            m_elements[k_place_label_id] = place_label;
            add_child(place_label);
            
            const auto drift_time_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            drift_time_label->create();
            drift_time_label->size = glm::vec2(38.f, 12.f);
            drift_time_label->set_text("DRIFT TIME:");
            drift_time_label->position = glm::vec2(38.f, 16.f);
            drift_time_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
            drift_time_label->set_visible_edges(false);
            m_elements[k_drift_time_label_id] = drift_time_label;
            add_child(drift_time_label);
            
            const auto drift_time_value_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            drift_time_value_label->create();
            drift_time_value_label->size = glm::vec2(24.f, 12.f);
            drift_time_value_label->set_text("00:00");
            drift_time_value_label->position = glm::vec2(90.f, 16.f);
            drift_time_value_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
            drift_time_value_label->set_visible_edges(false);
            drift_time_value_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            m_elements[k_drift_time_value_label_id] = drift_time_value_label;
            add_child(drift_time_value_label);
            
            enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                const auto material_component = child->get_component<gb::ces_material_component>();
                if (material_component)
                {
                    const auto material = material_component->get_material("ws.ui", 0);
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_mask_parameter(1);
                    }
                }
                
                child->enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                    const auto material_component = child->get_component<gb::ces_material_component>();
                    const auto material = material_component->get_material("ws.ui", 0);
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_mask_parameter(1);
                    }
                });
            });
        }
    }
}
