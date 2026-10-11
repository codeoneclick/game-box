#include "island_demo_scene.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "ocean_mesh_builder.h"
#include "ocean_shader_uniforms.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_ocean_system.h"
#include "ces_transformation_3d_component.h"
#include "ocean_demo_props.h"
#include "ces_buoyancy_component.h"
#include "island_resident_demo.h"
#include "ces_island_resident_system.h"
#include "camera_2d.h"
#include "label.h"
#include "ces_action_component.h"
#include "ces_transformation_2d_component.h"
#include "render_statistics.h"
#include <sstream>
#include <iomanip>
#include "camera_3d.h"
#include "ces_geometry_3d_component.h"
#include "configuration_accessor.h"
#include "game_transition.h"
#include "input_context.h"
#include "glm_extensions.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include "window_impl.h"
#include <chrono>
#include <cstdlib>

namespace game
{
    namespace
    {
        class orbit_camera_input final : public gb::input_context_listener
        {
        private:
            std::weak_ptr<gb::camera_3d> m_camera;
            bool m_is_dragging = false;
            bool m_is_panning = false;
            glm::vec3 m_pan_anchor;
            std::shared_ptr<const terrain_mesh_data> m_surface;
            float m_water_y;
            float m_yaw = 45.f;
            float m_pitch = 55.f;
            float m_radius = 245.f;
            float m_min_radius = 15.f;
            float m_max_radius = 700.f;
            glm::vec3 m_target = glm::vec3(-24.f, 6.f, 24.f);

            bool cursor_ray(const glm::ivec2& point, glm::ray& ray)
            {
                const auto camera = m_camera.lock();
                if (!camera) return false;
                const auto viewport = camera->get_viewport();
                if (viewport.z <= 0 || viewport.w <= 0) return false;
                const glm::vec2 ndc(2.f * (point.x - viewport.x) / viewport.z - 1.f,
                                   1.f - 2.f * (point.y - viewport.y) / viewport.w);
                const auto inverse_vp = glm::inverse(camera->get_mat_p() * camera->get_mat_v());
                glm::vec4 far_point = inverse_vp * glm::vec4(ndc, 1.f, 1.f);
                if (std::abs(far_point.w) < .000001f) return false;
                const glm::vec3 origin = camera->get_position();
                const glm::vec3 direction = glm::normalize(glm::vec3(far_point) / far_point.w - origin);
                ray.set_origin(origin);
                ray.set_direction(direction);
                return true;
            }

            bool cursor_on_plane(const glm::ivec2& point, float height, glm::vec3& hit)
            {
                glm::ray ray;
                if (!cursor_ray(point, ray) || std::abs(ray.get_direction().y) < .00001f) return false;
                const float distance = (height - ray.get_origin().y) / ray.get_direction().y;
                if (distance <= 0.f || !std::isfinite(distance)) return false;
                hit = ray.get_origin() + ray.get_direction() * distance;
                return true;
            }

            bool pick_surface(const glm::ivec2& point, glm::vec3& hit)
            {
                glm::ray ray;
                if (!cursor_ray(point, ray)) return false;
                bool found = cursor_on_plane(point, m_water_y, hit);
                float nearest = found ? glm::length(hit - ray.get_origin()) : std::numeric_limits<float>::max();
                const auto position = [](const terrain_vertex& vertex) {
                    return glm::vec3(vertex.position[0], vertex.position[1], vertex.position[2]);
                };
                for (size_t i = 0; i < m_surface->indices.size(); i += 3)
                {
                    glm::vec3 candidate;
                    if (glm::triangle_intersection(position(m_surface->vertices[m_surface->indices[i]]),
                        position(m_surface->vertices[m_surface->indices[i + 1]]),
                        position(m_surface->vertices[m_surface->indices[i + 2]]), ray, &candidate))
                    {
                        const float distance = glm::dot(candidate - ray.get_origin(), ray.get_direction());
                        if (distance > 0.f && distance < nearest)
                        {
                            nearest = distance;
                            hit = candidate;
                            found = true;
                        }
                    }
                }
                return found;
            }

            void update_camera()
            {
                if (const auto camera = m_camera.lock())
                {
                    const float pitch_radians = glm::radians(m_pitch);
                    const float horizontal_radius = m_radius * std::cos(pitch_radians);
                    camera->set_rotation(m_yaw);
                    camera->set_look_at(m_target);
                    // camera_3d rotates the X/Z radius and uses absolute Y.
                    camera->set_distance_to_look_at(glm::vec3(horizontal_radius,
                        m_target.y + m_radius * std::sin(pitch_radians), horizontal_radius));
                }
            }

        public:
            orbit_camera_input(const std::shared_ptr<gb::camera_3d>& camera,
                               const terrain_mesh_data& surface, float water_y, const glm::vec3& resident_focus) :
                m_camera(camera), m_surface(std::make_shared<terrain_mesh_data>(surface)), m_water_y(water_y)
            {
                if(const char* inspect=std::getenv("GB_OCEAN_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=glm::vec3(-85.f,0.f,100.f);m_radius=85.f;m_pitch=35.f;}
                }
                if(const char* inspect=std::getenv("GB_RESIDENT_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=resident_focus;m_radius=3.5f;m_min_radius=2.5f;m_pitch=12.f;m_yaw=90.f;}
                }
                update_camera();
            }

            void on_gr_pressed(const glm::ivec2& point, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (index != 0) return;
                if (source == gb::e_input_source_mouse_left && !m_is_panning) m_is_dragging = true;
                if (source == gb::e_input_source_mouse_right)
                {
                    m_is_dragging = false;
                    m_is_panning = pick_surface(point, m_pan_anchor);
                }
            }

            void on_gr_released(const glm::ivec2&, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (source == gb::e_input_source_mouse_left && index == 0) m_is_dragging = false;
                if (source == gb::e_input_source_mouse_right && index == 0) m_is_panning = false;
            }

            void on_gr_moved(const glm::ivec2&, const glm::ivec2&, const glm::ivec2&, ui32) override {}

            void on_gr_dragged(const glm::ivec2& point, const glm::ivec2&, const glm::ivec2& delta,
                               gb::e_input_source source, ui32 index) override
            {
                if (m_is_dragging && source == gb::e_input_source_mouse_left && index == 0)
                {
                    m_yaw += static_cast<float>(delta.x) * .25f;
                    m_pitch = glm::clamp(m_pitch - static_cast<float>(delta.y) * .25f, 10.f, 80.f);
                    update_camera();
                }
                if (m_is_panning && source == gb::e_input_source_mouse_right && index == 0)
                {
                    glm::vec3 cursor_world;
                    if (cursor_on_plane(point, m_pan_anchor.y, cursor_world))
                    {
                        // Translate opposite the cursor motion on the grabbed surface.
                        const glm::vec3 translation = m_pan_anchor - cursor_world;
                        m_target.x += translation.x;
                        m_target.z += translation.z;
                        update_camera();
                    }
                }
            }

            void on_gr_scrolled(const glm::ivec2& point, const glm::ivec2&, f32 delta) override
            {
                if (!std::isfinite(delta) || delta==0.f) return;
                m_radius=glm::clamp(m_radius*std::exp(-delta*.12f), m_min_radius, m_max_radius);
                update_camera();
                // Rebase an active grab after zoom so the next drag cannot jump.
                if(m_is_panning) {
                    glm::vec3 hit;
                    if(cursor_on_plane(point,m_pan_anchor.y,hit)) m_pan_anchor=hit;
                }
            }

            void on_key_up(i32) override {}
            void on_key_down(i32) override {}
            void on_virtual_keyboard_input(const std::string&) override {}
            void on_virtual_keyboard_backspace() override {}
            void on_virtual_keyboard_hidden() override {}
        };

        gb::mesh_3d_shared_ptr upload_mesh(const terrain_mesh_data& data, const std::string& guid)
        {
            auto declaration = std::make_shared<gb::vbo::vertex_declaration_PTNTC>(static_cast<ui32>(data.vertices.size()));
            auto vertices_buffer = std::make_shared<gb::vbo>(declaration, gb::gl::constant::static_draw);
            auto vertices = vertices_buffer->lock<gb::vbo::vertex_attribute_PTNTC>();
            for (size_t i=0; i<data.vertices.size(); ++i)
            {
                const auto& vertex=data.vertices[i];
                vertices[i] = {};
                vertices[i].m_position=glm::vec3(vertex.position[0],vertex.position[1],vertex.position[2]);
                vertices[i].m_normal=glm::packSnorm4x8(glm::vec4(vertex.normal[0],vertex.normal[1],vertex.normal[2],0.f));
                vertices[i].m_tangent=glm::packSnorm4x8(glm::vec4(1.f,0.f,0.f,0.f));
                vertices[i].m_texcoord=glm::packHalf2x16(glm::vec2(0.f));
                vertices[i].m_color=glm::u8vec4(vertex.color[0],vertex.color[1],vertex.color[2],vertex.color[3]);
            }
            vertices_buffer->unlock();
            auto indices_buffer=std::make_shared<gb::ibo>(static_cast<ui32>(data.indices.size()),gb::gl::constant::static_draw);
            std::copy(data.indices.begin(),data.indices.end(),indices_buffer->lock());
            indices_buffer->unlock();
            // mesh_3d's legacy automatic bounds assume PT4B, while this upload is PTNTC.
            glm::vec3 min_bound(std::numeric_limits<float>::max()),max_bound(std::numeric_limits<float>::lowest());
            for(const auto& vertex:data.vertices){
                const glm::vec3 p(vertex.position[0],vertex.position[1],vertex.position[2]);
                min_bound=glm::min(min_bound,p);max_bound=glm::max(max_bound,p);
            }
            // Include water displacement so its rest plane isn't culled at the frustum edge.
            if(guid.find("island.water")==0){min_bound.y=-1.f;max_bound.y=1.f;}
            return gb::mesh_3d::construct(guid,vertices_buffer,indices_buffer,max_bound,min_bound);
        }
    }

    island_demo_scene::island_demo_scene(const gb::game_transition_shared_ptr& transition) : gb::scene_graph(transition) {}

    island_demo_scene::~island_demo_scene()
    {
        if (const auto input_context = m_input_context.lock())
        {
            input_context->remove_listener(m_orbit_listener);
            if(m_resident_listener)input_context->remove_listener(m_resident_listener);
        }
    }

    void island_demo_scene::create()
    {
        gb::scene_graph::create();
        auto technique_uniforms=add_component<gb::ces_render_technique_uniforms_component>();
        auto field_debug=technique_uniforms->construct_uniforms<ocean_field_debug_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"compose");
        glm::vec4 ss_debug(0.f);
        if(const char* value=std::getenv("GB_OCEAN_FIELD_VIEW"))ss_debug.x=static_cast<float>(std::atoi(value));
        if(const char* value=std::getenv("GB_SSAO_DEBUG"))ss_debug.y=static_cast<float>(std::atoi(value));
        field_debug->set(ss_debug,"field_debug");
        auto ao=technique_uniforms->construct_uniforms<ssao_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"ss.ao");
        float ao_strength=1.2f;
        if(const char* value=std::getenv("GB_SSAO"))if(std::atoi(value)==0)ao_strength=0.f;
        ao->set(glm::vec4(3.f,.06f,ao_strength,1.f),"ao_params");
        terrain_config config;
        const char* debug=std::getenv("GB_ISLAND_LAYER_COLORS");
        config.debug_layer_colors=debug && std::string(debug)=="1";
        const auto start=std::chrono::steady_clock::now();
        m_terrain=terrain_generator::generate(config);
        const auto generated=std::chrono::steady_clock::now();
        const auto terrain_mesh=terrain_mesh_builder::build(m_terrain);
        const auto built=std::chrono::steady_clock::now();
        const auto configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("island_terrain.xml"));
        ocean_settings ocean;
        ocean.mean_level=config.water_y;
        auto ocean_system=std::make_shared<ces_ocean_system>(ocean);
        get_transition()->add_system(ocean_system);
        const auto water_configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("ocean.xml"));
        const auto prop_configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("ocean_prop.xml"));
        const auto add_shape=[&](const terrain_mesh_data& data,const std::string& guid,bool water=false) {
            auto shape=gb::ces_entity::construct<gb::shape_3d>();
            shape->tag=guid;
            shape->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(data,guid));
            const auto material_configuration=water?water_configuration:
                (guid.find("ocean.prop.")==0?prop_configuration:configuration);
            get_fabricator()->add_materials(shape,material_configuration->get_materials_configurations());
            add_child(shape);
            if(water)ocean_system->add_surface(shape);else {
                ocean_system->add_receiver(shape);
                auto mask=gb::ces_entity::construct<gb::shape_3d>();
                auto mg=mask->get_component<gb::ces_geometry_3d_component>();mg->set_mesh(shape->get_component<gb::ces_geometry_3d_component>()->get_mesh()->as_3d());mg->bounding_mode=gb::ces_geometry_component::e_none;
                const auto mc=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration(guid.find("island.")==0?"water_land.xml":"water_mask.xml"));
                get_fabricator()->add_materials(mask,mc->get_materials_configurations());shape->add_child(mask);ocean_system->add_interaction(shape,mask);
            }
            return shape;
        };
        terrain_mesh_data field_quad;
        field_quad.vertices={{{{-1,-1,0}},{{0,0,1}},{{255,255,255,255}}},{{{1,-1,0}},{{0,0,1}},{{255,255,255,255}}},{{{-1,1,0}},{{0,0,1}},{{255,255,255,255}}},{{{1,1,0}},{{0,0,1}},{{255,255,255,255}}}};
        field_quad.indices={0,1,2,1,3,2};
        for(const auto& name:{"water_seed","water_jfa0","water_jfa1","water_jfa2","water_jfa3","water_jfa4","water_jfa5","water_jfa6","water_jfa7","water_jfa8","water_jfa9","water_jfa10","water_jfa11","water_distance"}){
            auto quad=gb::ces_entity::construct<gb::shape_3d>();auto geometry=quad->get_component<gb::ces_geometry_3d_component>();
            geometry->set_mesh(upload_mesh(field_quad,std::string("field.")+name));geometry->bounding_mode=gb::ces_geometry_component::e_none;
            auto c=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration(std::string(name)+".xml"));
            get_fabricator()->add_materials(quad,c->get_materials_configurations());add_child(quad);
        }
        add_shape(terrain_mesh,"island.terrain");
        add_shape(terrain_mesh_builder::build_seabed(config),"island.seabed");
        const auto water_patches=ocean_mesh_builder::build(config.water_size,384);
        for(size_t i=0;i<water_patches.size();++i)add_shape(water_patches[i],"island.water."+std::to_string(i),true);
        add_shape(ocean_mesh_builder::build_horizon(config.water_size,4096.f,384),"island.water.horizon",true);
        const auto props=build_ocean_demo_props(ocean);
        for(size_t i=0;i<props.size();++i){
            const auto& p=props[i];auto shape=add_shape(p.mesh,"ocean.prop."+std::to_string(i));
            auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
            geometry->bounding_mode=gb::ces_geometry_component::e_radius;
            float radius=0;for(const auto& vertex:p.mesh.vertices)radius=std::max(radius,glm::length(glm::vec3(vertex.position[0],vertex.position[1],vertex.position[2])));
            geometry->set_bounding_radius(radius);
            auto transform=shape->get_component<gb::ces_transformation_3d_component>();
            transform->set_position(glm::vec3(p.position.x,p.position.y,p.position.z));transform->set_rotation(glm::vec3(0,p.yaw,0));
            if(p.floating){auto buoyancy=shape->add_component<ces_buoyancy_component>();buoyancy->settings=p.buoyancy;buoyancy->probes=p.probes;buoyancy->state.position=p.position;buoyancy->state.yaw=p.yaw;}
        }

        glm::vec3 resident_focus(-62.f,1.45f,74.f);
        try {
            auto resident=create_island_resident_demo(get_fabricator(),terrain_mesh,config);
            resident_focus=resident.focus;for(const auto& shape:resident.shapes)add_child(shape);
            get_transition()->add_system(resident.system);
            m_resident_listener=resident.input;
        } catch(const std::exception& error){std::cerr<<"Resident setup failed: "<<error.what()<<std::endl;}
        const glm::ivec2 resolution=get_transition()->get_window()->get_resolution_size_in_points();
        set_camera_2d(std::make_shared<gb::camera_2d>(resolution.x,resolution.y));
        // Native engine labels: screen-space HUD, updated at 4 Hz to avoid
        // rebuilding the dynamic font meshes every frame.
        auto hud=gb::ces_entity::construct<gb::ces_entity>();
        hud->tag="island.statistics";
        std::array<gb::label_shared_ptr,3> labels;
        const std::array<std::string,3> initial={"FPS: --", "Polygons/frame: --", "Draw calls/frame: --"};
        for(size_t i=0;i<labels.size();++i){
            labels[i]=get_fabricator()->create_label_2d("button_label.xml");
            labels[i]->font_name="boogie_nights.TTF";
            labels[i]->font_size=18.f;
            labels[i]->font_color=glm::u8vec4(255);
            labels[i]->text=initial[i];
            // The renderer keys 2D entities by their local z order. Keep each
            // label unique and inside the camera's depth range.
            labels[i]->get_component<gb::ces_transformation_2d_component>()->set_z_order(-.5f+i*.001f);
            labels[i]->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
            labels[i]->position=glm::vec2(resolution.x-18.f-labels[i]->get_content_size().x,18.f+i*25.f);
            hud->add_child(labels[i]);
        }
        auto action=hud->add_component<gb::ces_action_component>();
        action->set_update_callback([labels,resolution,elapsed=0.f](const gb::ces_entity_shared_ptr&,float dt) mutable {
            elapsed+=dt;
            if(elapsed<.25f)return;
            elapsed=0.f;
            const auto stats=gb::render_statistics::last_frame();
            std::ostringstream fps;fps<<"FPS: "<<std::fixed<<std::setprecision(1)<<stats.fps;
            const std::array<std::string,3> lines={fps.str(),"Polygons/frame: "+std::to_string(stats.triangles),"Draw calls/frame: "+std::to_string(stats.draw_calls)};
            for(size_t i=0;i<labels.size();++i){
                if(static_cast<std::string>(labels[i]->text)!=lines[i])labels[i]->text=lines[i];
                labels[i]->position=glm::vec2(resolution.x-18.f-labels[i]->get_content_size().x,18.f+i*25.f);
            }
            if(std::getenv("GB_HUD_DEBUG"))std::cout<<"HUD fps="<<stats.fps<<" triangles="<<stats.triangles<<" draws="<<stats.draw_calls<<std::endl;
        });
        add_child(hud);
        // Keep foreground terrain visible at minimum orbit radius and low pitch.
        const bool resident_inspect=std::getenv("GB_RESIDENT_INSPECT")&&std::string(std::getenv("GB_RESIDENT_INSPECT"))=="1";
        auto camera=std::make_shared<gb::camera_3d>(60.f,resident_inspect?.1f:.5f,1200.f,glm::ivec4(0,0,resolution.x,resolution.y),true);
        set_camera_3d(camera);
        m_orbit_listener = std::make_shared<orbit_camera_input>(camera, terrain_mesh, config.water_y,resident_focus);
        const auto input_context = get_transition()->get_input_context();
        m_input_context = input_context;
        input_context->add_listener(m_orbit_listener);
        if(m_resident_listener)input_context->add_listener(m_resident_listener);
        std::cout << "Island seed=" << config.seed << " layers=" << config.layers_count
                  << " vertices=" << terrain_mesh.vertices.size() << " triangles=" << terrain_mesh.indices.size()/3
                  << " generation_ms=" << std::chrono::duration<double,std::milli>(generated-start).count()
                  << " mesh_ms=" << std::chrono::duration<double,std::milli>(built-generated).count() << std::endl;
        std::cout<<"Ocean patches="<<water_patches.size()<<" terraces="<<config.layers_count<<"+"<<config.underwater_layers_count<<std::endl;
    }
}
