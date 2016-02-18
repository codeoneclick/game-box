//
//  tree_view_cell.cpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "tree_view_cell.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "text_label.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_component.h"
#include "ces_action_component.h"
#include "ces_material_component.h"
#include "game_command.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        tree_view_cell_data::tree_view_cell_data(const std::string& description) :
        m_description(description)
        {
            parent.getter([=]() {
                return m_parent.lock();
            });
            
            children.getter([=]() {
                return m_ordered_children;
            });
            
            tree_view_cell_data::description.getter([=]() {
                return m_description;
            });
        }
        
        void tree_view_cell_data::add_child(const tree_view_cell_data_shared_ptr& child)
        {
            if(m_children.count(child) != 0)
            {
                assert(false);
                std::cout<<"error. can't add same child"<<std::endl;
            }
            else
            {
                tree_view_cell_data_shared_ptr parent = tree_view_cell_data::parent;
                if(parent)
                {
                    parent->remove_child(child);
                }
                child->m_parent = shared_from_this();
                
                m_children.insert(child);
                m_ordered_children.push_back(child);
            }
        }
        
        void tree_view_cell_data::remove_child(const tree_view_cell_data_shared_ptr& child)
        {
            if(m_children.count(child) != 0)
            {
                m_children.erase(child);
                m_ordered_children.erase(std::find(m_ordered_children.begin(), m_ordered_children.end(), child));
            }
        }
        
        tree_view_cell::tree_view_cell(const scene_fabricator_shared_ptr& fabricator, const tree_view_cell_data_shared_ptr& data) :
        gb::ui::control(fabricator),
        m_data(data),
        m_text_horizontal_aligment(e_element_horizontal_aligment_left),
        m_text_vertical_aligment(e_element_vertical_aligment_center)
        {
            size.setter([=](const glm::vec2& size) {
                control::set_size(size);
                std::static_pointer_cast<gb::sprite>(m_elements["tree_view_cell_background"])->size = size;
                std::static_pointer_cast<gb::text_label>(m_elements["tree_view_cell_label"])->font_height = size.y * .5f;
            });
            size.getter([=]() {
                return control::get_size();
            });
            
            text.setter([=](const std::string& text) {
                std::static_pointer_cast<gb::text_label>(m_elements["tree_view_cell_label"])->text = text;
            });
            text.getter([=]() {
                return std::static_pointer_cast<gb::text_label>(m_elements["tree_view_cell_label"])->text;
            });
        }
        
        tree_view_cell::~tree_view_cell()
        {
            
        }
        
        void tree_view_cell::create()
        {
            gb::sprite_shared_ptr tree_view_cell_background =
            control::get_fabricator()->create_sprite("tree_view_cell_background.xml");
            m_elements["tree_view_cell_background"] = tree_view_cell_background;
            ces_entity::add_child(tree_view_cell_background);
            
            gb::text_label_shared_ptr tree_view_cell_label = control::get_fabricator()->create_text_label("tree_view_cell_label.xml");
            m_elements["tree_view_cell_label"] = tree_view_cell_label;
            ces_entity::add_child(tree_view_cell_label);
            
            gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<geometry_on_mesh_updated::t_command>>(std::bind(&tree_view_cell::on_text_mesh_updated, this));
            tree_view_cell_label->get_component(e_ces_component_type_geometry)->add_event_listener(geometry_on_mesh_updated::guid, command);
            
            command = std::make_shared<gb::game_command<text_on_text_updated::t_command>>(std::bind(&tree_view_cell::on_text_updated, this));
            tree_view_cell_label->get_component(e_ces_component_type_text)->add_event_listener(text_on_text_updated::guid, command);
            tree_view_cell_label->text_color = control::k_white_color;
            
            ces_material_component* material_component = unsafe_get_material_component(tree_view_cell_background);
            material_component->set_custom_shader_uniform(control::k_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void tree_view_cell::on_text_mesh_updated()
        {
            control::set_element_horizontal_aligment(m_elements["tree_view_cell_label"], m_text_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements["tree_view_cell_label"], m_text_vertical_aligment);
        }
        
        void tree_view_cell::on_text_updated()
        {
            control::set_element_horizontal_aligment(m_elements["tree_view_cell_label"], m_text_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements["tree_view_cell_label"], m_text_vertical_aligment);
        }
    };
};