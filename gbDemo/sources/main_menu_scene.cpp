#include "main_menu_scene.h"

#include "camera_2d.h"
#include "camera_3d.h"
#include "ces_geometry_3d_component.h"
#include "configuration_accessor.h"
#include "game_transition.h"
#include "input_context.h"
#include "mesh_constructor.h"
#include "mesh_2d.h"
#include "mesh_3d.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include "window_impl.h"

namespace game
{
    namespace
    {
        gb::mesh_3d_shared_ptr create_ground_mesh()
        {
            // The built-in 3D plane uses batch-only buffers; this quad has bindable buffers.
            auto quad = gb::mesh_constructor::create_shape_quad();
            auto vertices = quad->get_vbo()->lock<gb::vbo::vertex_attribute_PTNTC>();
            const auto normal = glm::packSnorm4x8(glm::vec4(0.f, 0.f, -1.f, 0.f));
            for (int index = 0; index < 4; ++index)
            {
                vertices[index].m_normal = normal;
            }
            quad->get_vbo()->unlock();
            return gb::mesh_3d::construct("demo.ground", quad->get_vbo(), quad->get_ibo());
        }

        class orbit_camera_input final : public gb::input_context_listener
        {
        private:
            std::weak_ptr<gb::camera_3d> m_camera;
            bool m_is_dragging = false;
            float m_yaw = 35.f;
            float m_pitch = 30.f;
            const float m_radius = 7.f;
            const glm::vec3 m_target = glm::vec3(0.f, .6f, 0.f);

            void update_camera()
            {
                if (const auto camera = m_camera.lock())
                {
                    const float pitch_radians = glm::radians(m_pitch);
                    const float horizontal_radius = m_radius * std::cos(pitch_radians);
                    camera->set_rotation(m_yaw);
                    camera->set_look_at(m_target);
                    camera->set_distance_to_look_at(glm::vec3(horizontal_radius,
                                                               m_target.y + m_radius * std::sin(pitch_radians),
                                                               horizontal_radius));
                }
            }

        public:
            explicit orbit_camera_input(const std::shared_ptr<gb::camera_3d>& camera) : m_camera(camera)
            {
                update_camera();
            }

            void on_gr_pressed(const glm::ivec2&, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (source == gb::e_input_source_mouse_left && index == 0)
                {
                    m_is_dragging = true;
                }
            }

            void on_gr_released(const glm::ivec2&, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (source == gb::e_input_source_mouse_left && index == 0)
                {
                    m_is_dragging = false;
                }
            }

            void on_gr_moved(const glm::ivec2&, const glm::ivec2&, const glm::ivec2&, ui32) override {}

            void on_gr_dragged(const glm::ivec2&, const glm::ivec2&, const glm::ivec2& delta,
                               gb::e_input_source source, ui32 index) override
            {
                if (m_is_dragging && source == gb::e_input_source_mouse_left && index == 0)
                {
                    m_yaw += static_cast<float>(delta.x) * .25f;
                    m_pitch = glm::clamp(m_pitch - static_cast<float>(delta.y) * .25f, 10.f, 80.f);
                    update_camera();
                }
            }

            void on_key_up(i32) override {}
            void on_key_down(i32) override {}
            void on_virtual_keyboard_input(const std::string&) override {}
            void on_virtual_keyboard_backspace() override {}
            void on_virtual_keyboard_hidden() override {}
        };
    }

    main_menu_scene::main_menu_scene(const gb::game_transition_shared_ptr& transition) : gb::scene_graph(transition) {}

    main_menu_scene::~main_menu_scene()
    {
        if (const auto input_context = m_input_context.lock())
        {
            input_context->remove_listener(m_orbit_listener);
        }
    }

    void main_menu_scene::create()
    {
        gb::scene_graph::create();

        const glm::ivec2 resolution = get_transition()->get_window()->get_resolution_size_in_points();
        set_camera_2d(std::make_shared<gb::camera_2d>(resolution.x, resolution.y));
        auto camera = std::make_shared<gb::camera_3d>(45.f, .1f, 128.f,
                                                      glm::ivec4(0, 0, resolution.x, resolution.y), true);
        set_camera_3d(camera);

        const auto ground_configuration = std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("demo_ground.xml"));
        auto ground = gb::ces_entity::construct<gb::shape_3d>();
        ground->get_component<gb::ces_geometry_3d_component>()->set_mesh(create_ground_mesh());
        get_fabricator()->add_materials(ground, ground_configuration->get_materials_configurations());
        ground->position = glm::vec3(0.f);
        ground->rotation = glm::vec3(90.f, 0.f, 0.f);
        ground->scale = glm::vec3(8.f, 8.f, 1.f);
        add_child(ground);

        auto cube = gb::ces_entity::construct<gb::shape_3d>();
        cube->get_component<gb::ces_geometry_3d_component>()->set_mesh(gb::mesh_constructor::create_shared_box());
        const auto cube_configuration = std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("demo_cube.xml"));
        get_fabricator()->add_materials(cube, cube_configuration->get_materials_configurations());
        cube->position = glm::vec3(0.f, .6f, 0.f);
        cube->scale = glm::vec3(1.2f);
        add_child(cube);

        m_orbit_listener = std::make_shared<orbit_camera_input>(camera);
        const auto input_context = get_transition()->get_input_context();
        m_input_context = input_context;
        input_context->add_listener(m_orbit_listener);
    }
}
