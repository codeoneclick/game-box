#include "island_demo_scene.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "camera_2d.h"
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
            const float m_radius = 245.f;
            glm::vec3 m_target = glm::vec3(0.f, 6.f, 0.f);

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
                               const terrain_mesh_data& surface, float water_y) :
                m_camera(camera), m_surface(std::make_shared<terrain_mesh_data>(surface)), m_water_y(water_y)
            {
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
            return gb::mesh_3d::construct(guid,vertices_buffer,indices_buffer);
        }
    }

    island_demo_scene::island_demo_scene(const gb::game_transition_shared_ptr& transition) : gb::scene_graph(transition) {}

    island_demo_scene::~island_demo_scene()
    {
        if (const auto input_context = m_input_context.lock())
        {
            input_context->remove_listener(m_orbit_listener);
        }
    }

    void island_demo_scene::create()
    {
        gb::scene_graph::create();
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
        const auto add_shape=[&](const terrain_mesh_data& data,const std::string& guid) {
            auto shape=gb::ces_entity::construct<gb::shape_3d>();
            shape->tag=guid;
            shape->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(data,guid));
            get_fabricator()->add_materials(shape,configuration->get_materials_configurations());
            add_child(shape);
        };
        add_shape(terrain_mesh,"island.terrain");
        add_shape(terrain_mesh_builder::build_water(config),"island.water");

        const glm::ivec2 resolution=get_transition()->get_window()->get_resolution_size_in_points();
        set_camera_2d(std::make_shared<gb::camera_2d>(resolution.x,resolution.y));
        // A distant overview needs a suitably distant near plane for depth precision.
        auto camera=std::make_shared<gb::camera_3d>(60.f,10.f,600.f,glm::ivec4(0,0,resolution.x,resolution.y),true);
        set_camera_3d(camera);
        m_orbit_listener = std::make_shared<orbit_camera_input>(camera, terrain_mesh, config.water_y);
        const auto input_context = get_transition()->get_input_context();
        m_input_context = input_context;
        input_context->add_listener(m_orbit_listener);
        std::cout << "Island seed=" << config.seed << " layers=" << config.layers_count
                  << " vertices=" << terrain_mesh.vertices.size() << " triangles=" << terrain_mesh.indices.size()/3
                  << " generation_ms=" << std::chrono::duration<double,std::milli>(generated-start).count()
                  << " mesh_ms=" << std::chrono::duration<double,std::milli>(built-generated).count() << std::endl;
    }
}
