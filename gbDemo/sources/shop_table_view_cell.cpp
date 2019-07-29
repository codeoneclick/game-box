//
//  shop_table_view_cell.cpp
//  gbDemo
//
//  Created by serhii.s on 6/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "shop_table_view_cell.h"
#include "button.h"
#include "textfield.h"
#include "sprite.h"
#include "image_button.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "material.h"
#include "scene_fabricator.h"
#include "gameplay_ui_fabricator.h"

namespace game
{
    namespace ui
    {
        shop_table_view_cell_data::shop_table_view_cell_data()
        {
            id.getter([=]() {
                return m_id;
            });
            
            id.setter([=](i32 value) {
                m_id = value;
            });
            
            product_name.getter([=]() {
                return m_product_name;
            });
            
            product_name.setter([=](const std::string& value) {
                m_product_name = value;
            });
            
            product_description.getter([=]() {
                return m_product_description;
            });
            
            product_description.setter([=](const std::string& value) {
                m_product_description = value;
            });
            
            product_price.getter([=]() {
                return m_product_price;
            });
            
            product_price.setter([=](f32 value) {
                m_product_price = value;
            });
            
            is_bought.getter([=]() {
                return m_is_bought;
            });
            
            is_bought.setter([=](bool value) {
                m_is_bought = value;
            });
            
            is_restore.getter([=]() {
                return m_is_restore;
            });
            
            is_restore.setter([=](bool value) {
                m_is_restore = value;
            });
        }
        
        const std::string shop_table_view_cell::k_buy_product_button_id = "buy_product_button";
        const std::string shop_table_view_cell::k_product_name_label_id = "product_name_label";
        const std::string shop_table_view_cell::k_product_description_label_id = "product_description_label";
        const std::string shop_table_view_cell::k_product_price_label_id = "product_price_label";
        const std::string shop_table_view_cell::k_restore_products_button_id = "restore_products_button";
        
        shop_table_view_cell::shop_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier) :
        gb::ui::table_view_cell(fabricator, index, identifier),
        m_buy_product_button_callback(nullptr)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<gb::ces_bound_touch_component>();
                bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                m_elements[control::k_background_element_name]->size = size;
            });
        }
        
        void shop_table_view_cell::setup_components()
        {
            gb::ui::table_view_cell::setup_components();
        }
        
        void shop_table_view_cell::create()
        {
            gb::ui::table_view_cell::create();
            set_background_color(gameplay_ui_fabricator::k_control_background_color);
            
            const auto buy_product_button = gb::ces_entity::construct<gb::ui::image_button>(control::get_fabricator());
            buy_product_button->create("ui_cart.png");
            buy_product_button->size = glm::vec2(32.f, 32.f);
            buy_product_button->position = glm::vec2(220.f, 54.f);
            buy_product_button->set_image_color(gameplay_ui_fabricator::k_control_image_color);
            buy_product_button->set_background_color(gameplay_ui_fabricator::k_control_background_color);
            buy_product_button->set_foreground_color(gameplay_ui_fabricator::k_control_foreground_color);
            m_elements[k_buy_product_button_id] = buy_product_button;
            add_child(buy_product_button);
            
            const auto restore_products_button = gb::ces_entity::construct<gb::ui::button>(control::get_fabricator());
            restore_products_button->create();
            restore_products_button->size = glm::vec2(128.f, 32.f);
            restore_products_button->position = glm::vec2(124.f, 54.f);
            restore_products_button->set_text("RESTORE");
            restore_products_button->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            restore_products_button->set_background_color(gameplay_ui_fabricator::k_control_background_color);
            restore_products_button->visible = false;
            m_elements[k_restore_products_button_id] = restore_products_button;
            add_child(restore_products_button);
            
            const auto product_name_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            product_name_label->create();
            product_name_label->size = glm::vec2(180.f, 24.f);
            product_name_label->set_text("PRODUCT");
            product_name_label->set_font_mode(gb::ces_font_component::e_font_mode_regular);
            product_name_label->position = glm::vec2(8.f, 8.f);
            product_name_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            product_name_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_left);
            product_name_label->set_visible_edges(false);
            m_elements[k_product_name_label_id] = product_name_label;
            add_child(product_name_label);
            
            const auto product_description_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            product_description_label->create();
            product_description_label->size = glm::vec2(225.f, 24.f);
            product_description_label->set_text("DESCRIPTION");
            product_description_label->set_font_mode(gb::ces_font_component::e_font_mode_regular);
            product_description_label->set_font_name("spincycle.otf");
            product_description_label->position = glm::vec2(8.f, 40.f);
            product_description_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            product_description_label->set_font_size(18.f);
            product_description_label->set_multiline(true);
            product_description_label->set_visible_edges(false);
            m_elements[k_product_description_label_id] = product_description_label;
            add_child(product_description_label);
            
            const auto product_price_label = gb::ces_entity::construct<gb::ui::textfield>(control::get_fabricator());
            product_price_label->create();
            product_price_label->size = glm::vec2(24.f, 24.f);
            product_price_label->set_text("0.99$");
            product_price_label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
            product_price_label->set_font_name("spincycle.otf");
            product_price_label->position = glm::vec2(180.f, 60.f);
            product_price_label->set_text_color(gameplay_ui_fabricator::k_control_text_color);
            product_price_label->set_visible_edges(false);
            m_elements[k_product_price_label_id] = product_price_label;
            add_child(product_price_label);
            
            enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                const auto material_component = child->get_component<gb::ces_material_component>();
                if (material_component)
                {
                    const auto material = material_component->get_material("ws.ui");
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_ref_value(1);
                    }
                }
                
                child->enumerate_children([=](const gb::ces_entity_shared_ptr &child) {
                    const auto material_component = child->get_component<gb::ces_material_component>();
                    const auto material = material_component->get_material("ws.ui");
                    if (material)
                    {
                        material->set_stencil_test(true);
                        material->set_stencil_function(gb::gl::constant::equal);
                        material->set_stencil_ref_value(1);
                    }
                });
            });
        }
        
        void shop_table_view_cell::set_buy_product_button_callback(const buy_product_button_callback_t &callback)
        {
            m_buy_product_button_callback = callback;
            if(!std::static_pointer_cast<gb::ui::image_button>(m_elements[k_buy_product_button_id])->is_pressed_callback_exist())
            {
                std::static_pointer_cast<gb::ui::image_button>(m_elements[k_buy_product_button_id])->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                    if(m_buy_product_button_callback)
                    {
                        m_buy_product_button_callback(m_index);
                    }
                });
            }
        }
        
        void shop_table_view_cell::set_product_name(const std::string &value)
        {
            const auto product_name_label = get_element_as<gb::ui::textfield>(k_product_name_label_id);
            product_name_label->set_text(value);
        }
        
        void shop_table_view_cell::set_product_description(const std::string &value)
        {
            const auto product_description_label = get_element_as<gb::ui::textfield>(k_product_description_label_id);
            product_description_label->set_text(value);
        }
        
        void shop_table_view_cell::set_product_price(f32 value)
        {
            const auto product_price_label = get_element_as<gb::ui::textfield>(k_product_price_label_id);
            std::stringstream price_strstream;
            price_strstream<<value<<"$";
            product_price_label->set_text(price_strstream.str());
        }
        
        void shop_table_view_cell::set_is_bought(bool value)
        {
            const auto buy_product_button = get_element_as<gb::ui::button>(k_buy_product_button_id);
            buy_product_button->visible = !value;
            
            const auto product_price_label = get_element_as<gb::ui::textfield>(k_product_price_label_id);
            product_price_label->visible = !value;
            product_price_label->set_visible_edges(false);
        }
        
        void shop_table_view_cell::set_is_restore(bool value)
        {
            const auto restore_products_button = get_element_as<gb::ui::button>(k_restore_products_button_id);
            restore_products_button->visible = value;
            
            const auto buy_product_button = get_element_as<gb::ui::image_button>(k_buy_product_button_id);
            buy_product_button->visible = !value;
            
            const auto product_price_label = get_element_as<gb::ui::textfield>(k_product_price_label_id);
            product_price_label->visible = !value;
            product_price_label->set_visible_edges(false);
        }
        
        void shop_table_view_cell::set_restore_products_button_callback(const restore_products_button_callback_t& callback)
        {
            m_restore_products_button_callback = callback;
            if(!std::static_pointer_cast<gb::ui::button>(m_elements[k_restore_products_button_id])->is_pressed_callback_exist())
            {
                std::static_pointer_cast<gb::ui::button>(m_elements[k_restore_products_button_id])->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                    if(m_restore_products_button_callback)
                    {
                        m_restore_products_button_callback(m_index);
                    }
                });
            }
        }
    }
}
