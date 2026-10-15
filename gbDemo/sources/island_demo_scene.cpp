#include "island_demo_scene.h"
#include "ces_sun_shadow_system.h"
#include "terrain_generator.h"
#include "terrain_mesh_builder.h"
#include "ocean_mesh_builder.h"
#include "ocean_shader_uniforms.h"
#include "stylized_post_process_uniforms.h"
#include "graphics_context.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_ocean_system.h"
#include "ces_transformation_3d_component.h"
#include "ocean_demo_props.h"
#include "ces_buoyancy_component.h"
#include "island_resident_demo.h"
#include "ces_island_resident_system.h"
#include "ces_island_resident_component.h"
#include "grass_interaction.h"
#include "orbit_zoom.h"
#include "terrain_editor_state.h"
#include "terrain_editor_ui.h"
#include "terrain_editor_input.h"
#include "terrain_editor_render_cache.h"
#include "editor_building_renderer.h"
#include "ces_textedit_system.h"
#include "file_dialog.h"
#include <deque>
#include <map>
#include <future>
#include <thread>
#include "ces_material_component.h"
#include "material.h"
#include "material_configuration.h"
#include "camera_2d.h"
#include "label.h"
#include "ces_action_component.h"
#include "ces_transformation_2d_component.h"
#include "render_statistics.h"
#include <sstream>
#include <iomanip>
#include <limits>
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
        // Keep the native text editor, but dispatch keys only to an active
        // field. The legacy system otherwise retains unfocused key events.
        class editor_textedit_system final : public gb::ui::ces_textedit_system {
            std::weak_ptr<TerrainEditorUI> ui_;
            bool captures_keyboard() const {
                const auto ui=ui_.lock();return ui&&ui->keyboard_captured();
            }
        public:
            explicit editor_textedit_system(const std::shared_ptr<TerrainEditorUI>& ui):ui_(ui){}
            void on_key_down(i32 key)override{if(captures_keyboard())gb::ui::ces_textedit_system::on_key_down(key);}
            void on_key_up(i32 key)override{if(captures_keyboard())gb::ui::ces_textedit_system::on_key_up(key);}
            void on_virtual_keyboard_input(const std::string& text)override{if(captures_keyboard())gb::ui::ces_textedit_system::on_virtual_keyboard_input(text);}
            void on_virtual_keyboard_backspace()override{if(captures_keyboard())gb::ui::ces_textedit_system::on_virtual_keyboard_backspace();}
            // Focus changes on clicks, not when the cursor leaves the field.
            void on_gr_moved(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,ui32)override{}
            void on_gr_dragged(const glm::ivec2&,const glm::ivec2&,const glm::ivec2&,gb::e_input_source,ui32)override{}
        };
        struct prepared_editor_mesh {
            std::vector<gb::vbo::vertex_attribute_PTNTC> vertices;
            glm::vec3 minimum=glm::vec3(std::numeric_limits<float>::max()),maximum=glm::vec3(std::numeric_limits<float>::lowest());
        };
        std::string editor_batch_guid(const EditorRenderBatch& batch){
            return (batch.kind==EditorRenderBatch::Grass?"island.grass.editor.":batch.kind==EditorRenderBatch::Trees?"island.trees.editor.":"island.editor.terrain.")+batch.key;
        }
        prepared_editor_mesh prepare_editor_mesh(const terrain_mesh_data& data,const std::string& guid,float water_level=0.f){
            prepared_editor_mesh result;result.vertices.resize(data.vertices.size());
            for(size_t i=0;i<data.vertices.size();++i){
                const auto& source=data.vertices[i];auto& vertex=result.vertices[i];vertex={};
                vertex.m_position=glm::vec3(source.position[0],source.position[1],source.position[2]);
                vertex.m_normal=glm::packSnorm4x8(glm::vec4(source.normal[0],source.normal[1],source.normal[2],0.f));
                vertex.m_tangent=glm::packSnorm4x8(glm::vec4(1.f,0.f,0.f,0.f));
                if(source.grass_bend[1]>0.f)vertex.m_tangent=glm::packSnorm4x8(glm::vec4(source.grass_bend[0],source.grass_bend[1],1.f,0.f));
                else if(source.tree_bend[1]>0.f)vertex.m_tangent=glm::packSnorm4x8(glm::vec4(source.tree_bend[0],source.tree_bend[1],0.f,1.f));
                vertex.m_texcoord=glm::packHalf2x16(glm::vec2(source.texcoord[0],source.texcoord[1]));
                if(source.editor_palette[0]>=0.f){vertex.m_tangent=glm::packSnorm4x8(glm::vec4(0,0,0,-1));vertex.m_texcoord=glm::packHalf2x16(glm::vec2(source.editor_palette[0],source.editor_palette[1]));}
                vertex.m_color=glm::u8vec4(source.color[0],source.color[1],source.color[2],source.color[3]);
                if(source.outline_id){
                    if(source.outline_id>0x3fffffu||source.outline_id==0x3fc000u)throw std::overflow_error("Procedural outline identity exceeds packed vertex capacity");
                    const auto id=source.outline_id;
                    vertex.m_normal=glm::packSnorm4x8(glm::vec4(source.normal[0],source.normal[1],source.normal[2],float((id>>7)&127u)/127.f));
                    auto tangent=glm::unpackSnorm4x8(vertex.m_tangent);tangent.z=-float(id&127u)/127.f;
                    vertex.m_tangent=glm::packSnorm4x8(tangent);vertex.m_color.a=static_cast<uint8_t>(id>>14);
                }
                result.minimum=glm::min(result.minimum,vertex.m_position);result.maximum=glm::max(result.maximum,vertex.m_position);
            }
            if(guid.find("island.water")==0){result.minimum.y=water_level-1.f;result.maximum.y=water_level+1.f;}
            if(guid.find("island.grass.")==0){result.minimum-=glm::vec3(1.05f,.2f,1.05f);result.maximum+=glm::vec3(1.05f,.2f,1.05f);}
            if(guid.find("island.trees.")==0){result.minimum-=glm::vec3(.4f,0,.4f);result.maximum+=glm::vec3(.4f,0,.4f);}
            return result;
        }
        struct editor_pending_mesh {EditorRenderBatch batch;prepared_editor_mesh prepared;};
        struct editor_rebuild_result {
            std::vector<editor_pending_mesh> batches;
            std::vector<resident_navigation> navigation;
            TerrainEditorDirty dirty;
            std::shared_ptr<const BuildingSurfaceSupport> support;
            uint64_t support_revision=0;
            uint64_t generation=0,revision=0,terrain_revision=0;
            double mesh_ms=0,navigation_ms=0;
        };
        struct editor_rebuild_jobs {
            std::future<editor_rebuild_result> worker;
            TerrainEditorDirty pending;
            std::vector<TerrainEditorDirty> regions;
            std::map<std::string,editor_pending_mesh> uploads;
            std::vector<resident_navigation> navigation;
            uint64_t terrain_revision=0,navigation_revision=0,generation=0;
            bool needs_navigation=false;
            std::shared_ptr<const BuildingSurfaceSupport> support;
            uint64_t support_revision=0;
        };
        void merge_editor_dirty(TerrainEditorDirty& destination,const TerrainEditorDirty& source){
            if(source.empty())return;
            if(destination.empty())destination=source;
            else {
                destination.x0=std::min(destination.x0,source.x0);destination.z0=std::min(destination.z0,source.z0);
                destination.x1=std::max(destination.x1,source.x1);destination.z1=std::max(destination.z1,source.z1);
                destination.terrain|=source.terrain;destination.grass|=source.grass;
                destination.trees|=source.trees;destination.climate|=source.climate;destination.buildings|=source.buildings;
            }
        }
        class orbit_camera_input final : public gb::input_context_listener
        {
        private:
            std::weak_ptr<gb::camera_3d> m_camera;
            bool m_is_dragging = false;
            gb::e_input_source m_rotation_source = gb::e_input_source_none;
            bool m_is_panning = false;
            glm::vec3 m_pan_anchor;
            std::shared_ptr<const terrain_mesh_data> m_surface;
            float m_water_y;
            float m_yaw = 110.f;
            float m_pitch = 20.f;
            float m_radius = 180.f;
            orbit_zoom m_zoom;
            bool m_debug_zoom=std::getenv("GB_ZOOM_DEBUG")!=nullptr;
            float m_min_radius = 15.f;
            float m_max_radius = 700.f;
            glm::vec3 m_target = glm::vec3(0.f, 6.f, 0.f);
            glm::ivec2 m_pan_cursor=glm::ivec2(0);

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
            std::function<bool(glm::ivec2)> pointer_blocked;
            std::function<bool()> rotation_allowed;
            std::function<bool(glm::ivec2,glm::vec3&)> surface_picker;
            orbit_camera_input(const std::shared_ptr<gb::camera_3d>& camera,
                               const terrain_mesh_data& surface, float water_y, const glm::vec3& resident_focus,const glm::vec3& tree_focus) :
                m_camera(camera), m_surface(std::make_shared<terrain_mesh_data>(surface)), m_water_y(water_y)
            {
                if(const char* inspect=std::getenv("GB_OCEAN_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=glm::vec3(-85.f,0.f,100.f);m_radius=85.f;m_pitch=35.f;}
                }
                if(const char* inspect=std::getenv("GB_RESIDENT_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=resident_focus;m_radius=3.5f;m_min_radius=2.5f;m_pitch=12.f;m_yaw=90.f;}
                }
                if(const char* inspect=std::getenv("GB_GRASS_INSPECT")){
                    if(std::string(inspect)=="1"){
                        for(size_t i=0;i+2<surface.indices.size();i+=3){
                            const auto& a=surface.vertices[surface.indices[i]];
                            if(a.normal[1]>.99f && std::abs(a.position[1]-6.f)<.01f){
                                const auto& b=surface.vertices[surface.indices[i+1]];
                                const auto& c=surface.vertices[surface.indices[i+2]];
                                m_target=(glm::vec3(a.position[0],a.position[1],a.position[2])+
                                    glm::vec3(b.position[0],b.position[1],b.position[2])+
                                    glm::vec3(c.position[0],c.position[1],c.position[2]))/3.f;
                                m_target.y+=.6f;m_radius=10.f;m_min_radius=2.5f;m_pitch=15.f;m_yaw=90.f;
                                break;
                            }
                        }
                    }
                }
                if(const char* inspect=std::getenv("GB_GRASS_INTERACTION_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=resident_focus;m_radius=4.5f;m_min_radius=2.5f;m_pitch=25.f;m_yaw=90.f;}
                }
                if(const char* inspect=std::getenv("GB_HORIZON_INSPECT")){
                    if(std::string(inspect)=="1"){m_radius=420.f;m_pitch=30.f;}
                    if(std::string(inspect)=="low"){m_radius=420.f;m_pitch=12.f;}
                }
                if(std::getenv("GB_TREES_INSPECT")){
                    m_target=tree_focus;m_radius=11.f;m_min_radius=2.5f;m_pitch=18.f;m_yaw=60.f;
                }
                m_zoom.reset(m_radius,m_min_radius,m_max_radius);
                update_camera();
            }

            void update_zoom(float dt){
                if(!m_zoom.advance(dt))return;
                m_radius=m_zoom.radius;update_camera();
                if(m_debug_zoom)std::cout<<"Zoom radius="<<m_radius<<" target="<<m_zoom.target<<std::endl;
                if(m_is_panning){
                    glm::vec3 hit;
                    if(cursor_on_plane(m_pan_cursor,m_pan_anchor.y,hit))m_pan_anchor=hit;
                }
            }

            void on_gr_pressed(const glm::ivec2& point, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (index != 0) return;
                if(pointer_blocked&&pointer_blocked(point))return;
                if (!m_is_panning && (source == gb::e_input_source_mouse_middle ||
                    (source == gb::e_input_source_mouse_left && (!rotation_allowed||rotation_allowed()))))
                {
                    m_is_dragging = true;
                    m_rotation_source = source;
                }
                if (source == gb::e_input_source_mouse_right)
                {
                    m_is_dragging = false;
                    m_pan_cursor=point;
                    m_is_panning = surface_picker?surface_picker(point,m_pan_anchor):pick_surface(point, m_pan_anchor);
                }
            }

            void on_gr_released(const glm::ivec2&, const glm::ivec2&, gb::e_input_source source, ui32 index) override
            {
                if (source == m_rotation_source && index == 0) { m_is_dragging = false; m_rotation_source = gb::e_input_source_none; }
                if (source == gb::e_input_source_mouse_right && index == 0) m_is_panning = false;
            }

            void on_gr_moved(const glm::ivec2&, const glm::ivec2&, const glm::ivec2&, ui32) override {}

            void on_gr_dragged(const glm::ivec2& point, const glm::ivec2&, const glm::ivec2& delta,
                               gb::e_input_source source, ui32 index) override
            {
                if (m_is_dragging && source == m_rotation_source && index == 0)
                {
                    m_yaw += static_cast<float>(delta.x) * .25f;
                    m_pitch = glm::clamp(m_pitch - static_cast<float>(delta.y) * .25f, 10.f, 80.f);
                    update_camera();
                }
                if (m_is_panning && source == gb::e_input_source_mouse_right && index == 0)
                {
                    m_pan_cursor=point;
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
                if(pointer_blocked&&pointer_blocked(point))return;
                m_zoom.scroll(delta);
                if(m_debug_zoom)std::cout<<"Zoom scroll: current="<<m_radius<<" target="<<m_zoom.target<<std::endl;
                if(m_is_panning)m_pan_cursor=point;
            }

            void on_key_up(i32) override {}
            void on_key_down(i32) override {}
            void on_virtual_keyboard_input(const std::string&) override {}
            void on_virtual_keyboard_backspace() override {}
            void on_virtual_keyboard_hidden() override {}
        };

        gb::mesh_3d_shared_ptr upload_mesh(const terrain_mesh_data& data, const std::string& guid,float water_level=0.f,bool streaming=false,const prepared_editor_mesh* supplied=nullptr)
        {
            prepared_editor_mesh local;if(!supplied){local=prepare_editor_mesh(data,guid,water_level);supplied=&local;}
            auto declaration = std::make_shared<gb::vbo::vertex_declaration_PTNTC>(static_cast<ui32>(supplied->vertices.size()));
            const auto mode=streaming?gb::gl::constant::dynamic_draw:gb::gl::constant::static_draw;
            auto vertices_buffer = std::make_shared<gb::vbo>(declaration, mode);
            auto vertices = vertices_buffer->lock<gb::vbo::vertex_attribute_PTNTC>();
            std::copy(supplied->vertices.begin(),supplied->vertices.end(),vertices);
            vertices_buffer->unlock();
            auto indices_buffer=std::make_shared<gb::ibo>(static_cast<ui32>(data.indices.size()),mode);
            std::copy(data.indices.begin(),data.indices.end(),indices_buffer->lock());
            indices_buffer->unlock();
            // Explicit PTNTC bounds include animated foliage/water margins.
            return gb::mesh_3d::construct(guid,vertices_buffer,indices_buffer,supplied->maximum,supplied->minimum);
        }
        terrain_mesh_data editor_palette_mesh(terrain_mesh_data mesh){
            // Preserve the old smooth contour geometry, replacing RGB with lookup coordinates.
            std::array<std::array<float,4>,24> palette;
            for(int i=0;i<24;++i)palette[i]=editor_climate_color(ClimateType::Temperate,i<8?i-8:i-7);
            for(auto& v:mesh.vertices){
                float best=std::numeric_limits<float>::max(),entry=0;
                for(int i=0;i<23;++i){
                    float numerator=0,denominator=0;
                    for(int k=0;k<3;++k){const float delta=palette[i+1][k]-palette[i][k];numerator+=(v.color[k]/255.f-palette[i][k])*delta;denominator+=delta*delta;}
                    const float t=denominator>.000001f?std::max(0.f,std::min(1.f,numerator/denominator)):0.f;
                    float error=0;for(int k=0;k<3;++k){const float d=v.color[k]/255.f-(palette[i][k]+t*(palette[i+1][k]-palette[i][k]));error+=d*d;}
                    if(error<best){best=error;entry=i+t;}
                }
                v.editor_palette={{(entry+.5f)/24.f,.5f/7.f}};v.color={{255,255,255,255}};
            }return mesh;
        }
    }

    island_demo_scene::island_demo_scene(const gb::game_transition_shared_ptr& transition) : gb::scene_graph(transition) {}

    island_demo_scene::~island_demo_scene()
    {
        if (const auto input_context = m_input_context.lock())
        {
            input_context->remove_listener(m_orbit_listener);
            if(m_editor_listener)input_context->remove_listener(m_editor_listener);
            if(m_textedit_listener)input_context->remove_listener(m_textedit_listener);
            if(m_resident_listener)input_context->remove_listener(m_resident_listener);
        }
    }

    void island_demo_scene::create()
    {
        gb::scene_graph::create();
        auto technique_uniforms=add_component<gb::ces_render_technique_uniforms_component>();
        auto sun_shadow_system=std::make_shared<ces_sun_shadow_system>(get_fabricator());
        sun_shadow_system->initialize(shared_from_this());
        get_transition()->add_system(sun_shadow_system);
        auto post_settings=std::make_shared<StylizedPostProcessSettings>(stylized_post_settings_from_environment());
        auto post_final=technique_uniforms->construct_uniforms<stylized_post_process_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"final");
        auto post_output=technique_uniforms->construct_uniforms<stylized_post_process_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"demo");
        auto field_debug=technique_uniforms->construct_uniforms<ocean_field_debug_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"compose");
        glm::vec4 ss_debug(0.f);ss_debug.z=static_cast<float>(post_settings->implementationStage);
        ss_debug.w=0.f; // Sun shadow maps replace screen-space ray tracing.
        if(const char* value=std::getenv("GB_OCEAN_FIELD_VIEW"))ss_debug.x=static_cast<float>(std::atoi(value));
        if(const char* value=std::getenv("GB_SSAO_DEBUG"))ss_debug.y=static_cast<float>(std::atoi(value));
        field_debug->set(ss_debug,"field_debug");
        auto ao=technique_uniforms->construct_uniforms<ssao_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment,"ss.ao");
        ao->set(glm::vec4(3.f,.06f,post_settings->enableSSAO?post_settings->ssaoIntensity:0.f,1.f),"ao_params");
        terrain_config config;
        const char* debug=std::getenv("GB_ISLAND_LAYER_COLORS");
        config.debug_layer_colors=debug && std::string(debug)=="1";
        const auto start=std::chrono::steady_clock::now();
        const auto initial_terrain=terrain_generator::generate(config);
        const auto generated=std::chrono::steady_clock::now();
        const auto terrain_mesh=terrain_mesh_builder::build(initial_terrain);
        auto editor_state=std::make_shared<TerrainEditorState>();editor_state->initialize(initial_terrain);
        std::vector<terrain_mesh_data> editor_legacy_grass;
        std::vector<gb::ces_entity_shared_ptr> editor_legacy_entities;
        std::shared_ptr<ces_island_resident_system> editor_resident_system;

                    const auto built=std::chrono::steady_clock::now();
        const auto configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("island_terrain.xml"));
        ocean_settings ocean;
        ocean.mean_level=config.water_y;
        auto ocean_system=std::make_shared<ces_ocean_system>(ocean);
        ocean_system->set_terrain_dimensions(config.layer_height,config.shore_height);
        get_transition()->add_system(ocean_system);
        const auto water_configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("ocean.xml"));
        const auto prop_configuration=std::static_pointer_cast<gb::shape_3d_configuration>(
            get_transition()->get_configuration_accessor()->get_shape_3d_configuration("ocean_prop.xml"));
        const auto add_shape=[&](const terrain_mesh_data& data,const std::string& guid,bool water=false) {
            auto shape=gb::ces_entity::construct<gb::shape_3d>();
            shape->tag=guid;
            shape->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(data,guid,water?config.water_y:0.f));
            const auto material_configuration=water?water_configuration:
                (guid.find("ocean.prop.")==0?prop_configuration:configuration);
            auto materials=material_configuration->get_materials_configurations();
            if(guid=="island.water.horizon"){
                // Horizon starts at |XZ|=384; the interaction field ends at
                // 160. It cannot contain contact foam, so avoid its empty pass.
                materials.erase(std::remove_if(materials.begin(),materials.end(),[](const std::shared_ptr<gb::configuration>& c){
                    return std::static_pointer_cast<gb::material_configuration>(c)->get_technique_name()=="ws.ocean.foam";
                }),materials.end());
            }
            get_fabricator()->add_materials(shape,materials);
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
        auto sky=gb::ces_entity::construct<gb::shape_3d>();sky->tag="island.sky";
        auto sky_geometry=sky->get_component<gb::ces_geometry_3d_component>();
        sky_geometry->set_mesh(upload_mesh(field_quad,"island.sky"));sky_geometry->bounding_mode=gb::ces_geometry_component::e_none;
        auto sky_config=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("island_sky.xml"));
        get_fabricator()->add_materials(sky,sky_config->get_materials_configurations());
        ocean_system->add_receiver(sky);add_child(sky);
        for(const auto& name:{"water_seed","water_jfa0","water_jfa1","water_jfa2","water_jfa3","water_jfa4","water_jfa5","water_jfa6","water_jfa7","water_jfa8","water_jfa9","water_jfa10","water_jfa11","water_distance"}){
            auto quad=gb::ces_entity::construct<gb::shape_3d>();auto geometry=quad->get_component<gb::ces_geometry_3d_component>();
            geometry->set_mesh(upload_mesh(field_quad,std::string("field.")+name));geometry->bounding_mode=gb::ces_geometry_component::e_none;
            auto c=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration(std::string(name)+".xml"));
            get_fabricator()->add_materials(quad,c->get_materials_configurations());add_child(quad);
        }
        editor_legacy_entities.push_back(add_shape(editor_palette_mesh(terrain_mesh),"island.terrain"));
        // Grass receives ordinary lighting/SSAO but must not become a water
        // interaction mask: these dry blades cannot generate shoreline foam.
        const char* grass_enabled=std::getenv("GB_ISLAND_GRASS");
        if (!grass_enabled || std::string(grass_enabled)!="0")
        {
            const auto grass=terrain_mesh_builder::build_grass(terrain_mesh,config);
            editor_legacy_grass=grass;
            size_t triangles=0;
            for (size_t i=0; i<grass.size(); ++i)
            {
                auto shape=gb::ces_entity::construct<gb::shape_3d>();
                const auto guid="island.grass."+std::to_string(i);
                shape->tag=guid;
                shape->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(grass[i],guid));
                get_fabricator()->add_materials(shape,configuration->get_materials_configurations());
                add_child(shape);
                editor_legacy_entities.push_back(shape);
                ocean_system->add_receiver(shape);
                triangles+=grass[i].indices.size()/3;
            }
            std::cout<<"Island grass: terraces=4-7 (summit first), batches="<<grass.size()<<" triangles="<<triangles<<std::endl;
        }
        glm::vec3 tree_focus(-24,10,24);
        std::vector<glm::vec4> tree_shadows;
        const char* trees_enabled=std::getenv("GB_ISLAND_TREES");
        if(!trees_enabled||std::string(trees_enabled)!="0"){
            const auto trees=terrain_mesh_builder::build_trees(terrain_mesh,config);
            bool focused=false;size_t total_triangles=0,batches=0;
            for(int terrace=3;terrace<=7;++terrace){
                terrain_mesh_data batch;size_t count=0,pines=0;int part=0;
                auto flush_trees=[&](){if(!batch.vertices.empty()){
                    auto shape=gb::ces_entity::construct<gb::shape_3d>();const auto guid="island.trees."+std::to_string(terrace)+"."+std::to_string(part++);shape->tag=guid;
                    shape->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(batch,guid));
                    get_fabricator()->add_materials(shape,configuration->get_materials_configurations());add_child(shape);ocean_system->add_receiver(shape);
                    editor_legacy_entities.push_back(shape);
                    ++batches;total_triangles+=batch.indices.size()/3;
                    batch=terrain_mesh_data();
                }};

                for(const auto& tree:trees)if(tree.terrace==terrace){
                    EditorTree logical;logical.id=editor_state->level.next_tree_id++;logical.type=static_cast<TreeType>(tree.species);
                    logical.position=tree.position;logical.scale=tree.height/7.f;logical.yaw=tree.yaw;editor_state->level.trees.push_back(logical);
                    tree_shadows.emplace_back(tree.position[0],tree.position[1],tree.position[2],tree.height*.32f);
                    const char* inspect=std::getenv("GB_TREES_INSPECT");
                    const std::string species=inspect?inspect:"";
                    const std::array<const char*,5> names={{"pine","oak","birch","palm","thuja"}};
                    const bool named=std::find(names.begin(),names.end(),species)!=names.end();
                    if(!focused&&((named&&species==names[tree.species])||(!named&&!tree.conifer&&terrace==6))){
                        tree_focus=glm::vec3(tree.position[0],tree.position[1]+tree.height*.5f,tree.position[2]);focused=true;
                    }
                    if(batch.vertices.size()+tree.mesh.vertices.size()>65535)flush_trees();
                    const size_t base=batch.vertices.size();
                    batch.vertices.insert(batch.vertices.end(),tree.mesh.vertices.begin(),tree.mesh.vertices.end());
                    for(auto index:tree.mesh.indices)batch.indices.push_back(static_cast<ui16>(base+index));
                    ++count;pines+=tree.conifer?1:0;
                }
                flush_trees();
                std::cout<<"Forest terrace="<<terrace<<" trees="<<count<<" conifers="<<pines<<" deciduous="<<(count-pines)<<std::endl;
            }
            std::cout<<"Forest initialized: trees="<<trees.size()<<" batches="<<batches<<" triangles="<<total_triangles<<std::endl;
        }
        auto seabed=add_shape(terrain_mesh_builder::build_seabed(config),"island.seabed");
        const auto water_patches=ocean_mesh_builder::build(config.water_size,384);
        auto water_entities=std::make_shared<std::vector<gb::shape_3d_shared_ptr>>();
        for(size_t i=0;i<water_patches.size();++i)water_entities->push_back(add_shape(water_patches[i],"island.water."+std::to_string(i),true));
        auto horizon=add_shape(ocean_mesh_builder::build_horizon(config.water_size,4096.f,384),"island.water.horizon",true);
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
        std::vector<gb::ces_entity_weak_ptr> shadow_people;
        try {
            auto resident=create_island_resident_demo(get_fabricator(),terrain_mesh,config);
            resident_focus=resident.focus;for(const auto& shape:resident.shapes)add_child(shape);
            for(const auto& shape:resident.shapes)shadow_people.push_back(shape);
            if(std::getenv("GB_GRASS_INTERACTION_INSPECT"))for(auto shape:resident.shapes){
                auto person=shape->get_component<ces_island_resident_component>();
                if(person&&person->terrace_index==3){resident_focus=shape->get_component<gb::ces_transformation_3d_component>()->get_position()+glm::vec3(0,.8f,0);break;}
            }
            get_transition()->add_system(resident.system);
            editor_resident_system=resident.system;
            constexpr ui32 max_quads=3000;
            auto declaration=std::make_shared<gb::vbo::vertex_declaration_PTNTC>(max_quads*4);
            auto buffer=std::make_shared<gb::vbo>(declaration,gb::gl::constant::dynamic_draw);
            auto vertices=buffer->lock<gb::vbo::vertex_attribute_PTNTC>();
            for(ui32 i=0;i<max_quads*4;++i)vertices[i]={};
            buffer->unlock();
            auto indices=std::make_shared<gb::ibo>(max_quads*6,gb::gl::constant::static_draw);
            auto index_data=indices->lock();
            for(ui32 q=0;q<max_quads;++q){const ui16 base=static_cast<ui16>(q*4);const ui16 pattern[]={0,1,2,1,3,2};for(ui32 j=0;j<6;++j)index_data[q*6+j]=base+pattern[j];}
            indices->unlock();
            auto field=gb::ces_entity::construct<gb::shape_3d>();field->tag="grass.interaction";
            auto geometry=field->get_component<gb::ces_geometry_3d_component>();geometry->bounding_mode=gb::ces_geometry_component::e_none;
            geometry->set_mesh(gb::mesh_3d::construct("grass.interaction",buffer,indices,glm::vec3(1),glm::vec3(-1)));
            auto field_config=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("grass_interaction.xml"));
            get_fabricator()->add_materials(field,field_config->get_materials_configurations());add_child(field);
            std::vector<gb::ces_entity_weak_ptr> people;for(auto shape:resident.shapes)people.push_back(shape);
            auto history=std::make_shared<grass_interaction>();
            auto action=field->add_component<gb::ces_action_component>();
            // Resident positions change in on_feed_start; actions run in feed
            // before rendering, including residents outside the camera frustum.
            action->set_update_callback([buffer,people,history,max_quads,previous=std::vector<glm::vec3>(people.size(),glm::vec3(0)),initialized=false](const gb::ces_entity_shared_ptr&,float dt) mutable {
                std::vector<grass_impulse> current;current.reserve(people.size());
                const bool enabled=!std::getenv("GB_GRASS_INTERACTION") || std::string(std::getenv("GB_GRASS_INTERACTION"))!="0";
                for(size_t i=0;i<people.size();++i)if(auto person=people[i].lock()){
                    const auto p=person->get_component<gb::ces_transformation_3d_component>()->get_position();
                    const auto resident=person->get_component<ces_island_resident_component>();
                    glm::vec3 velocity=initialized&&dt>.00001f?(p-previous[i])/dt:glm::vec3(0);previous[i]=p;
                    const float speed=glm::length(velocity);if(speed>2.f)velocity*=2.f/speed;
                    if(enabled&&resident&&resident->state.available&&resident->terrace_index>=1&&resident->terrace_index<=4)
                        current.push_back({p.x,p.z,p.y,velocity.x*.5f,velocity.z*.5f,0});
                }
                initialized=true;history->update(dt,current);
                auto vertices=buffer->lock<gb::vbo::vertex_attribute_PTNTC>();
                ui32 count=0;
                const auto emit=[&](const grass_impulse& p){
                    if(count>=max_quads)return;
                    const int layer=static_cast<int>(std::round(p.y/2.f))-1;
                    if(layer<0||layer>3)return;
                    const float strength=enabled?grass_interaction::strength(p.age):0.f;
                    constexpr float radius=1.6f;
                    for(ui32 k=0;k<4;++k){
                        const float dx=(k%2)?1.f:-1.f,dz=(k/2)?1.f:-1.f;
                        const glm::vec2 uv=(glm::vec2(p.x+dx*radius,p.z+dz*radius)+160.f)/320.f;
                        const glm::vec2 atlas=(glm::vec2(layer%2,layer/2)+uv)*.5f;
                        auto& v=vertices[count*4+k];v={};v.m_position=glm::vec3(atlas.x*2.f-1.f,1.f-atlas.y*2.f,0);
                        v.m_texcoord=glm::packHalf2x16(glm::vec2(dx,dz));
                        v.m_normal=glm::packSnorm4x8(glm::vec4(p.dx,0,p.dz,0));
                        v.m_color=glm::u8vec4(255,255,255,static_cast<ui8>(strength*255.f));
                    }
                    ++count;
                };
                for(const auto& p:current)emit(p);
                for(const auto& p:history->trail())emit(p);
                // Zero padding prevents stale impulses when the population or
                // trail shrinks. Fixed indices keep this in one draw.
                for(ui32 i=count*4;i<max_quads*4;++i)vertices[i]={};
                buffer->unlock();
            });
            m_resident_listener=resident.input;
        } catch(const std::exception& error){std::cerr<<"Resident setup failed: "<<error.what()<<std::endl;}
        // A single dynamic quad batch projects all blobs onto visible terrace tops.
        const ui32 shadow_count=static_cast<ui32>(tree_shadows.size()+shadow_people.size());
        if(shadow_count){
            if(shadow_count>16383)throw std::runtime_error("Blob shadow batch exceeds 16-bit vertices");
            constexpr ui32 shadow_capacity=4096;
            auto declaration=std::make_shared<gb::vbo::vertex_declaration_PTNTC>(shadow_capacity*4);
            auto buffer=std::make_shared<gb::vbo>(declaration,gb::gl::constant::dynamic_draw);
            auto vertices=buffer->lock<gb::vbo::vertex_attribute_PTNTC>();
            for(ui32 i=0;i<shadow_capacity*4;++i)vertices[i]={};buffer->unlock();
            auto indices=std::make_shared<gb::ibo>(shadow_capacity*6,gb::gl::constant::static_draw);
            auto data=indices->lock();
            for(ui32 i=0;i<shadow_capacity;++i){const ui16 pattern[]={0,1,2,1,3,2};for(ui32 k=0;k<6;++k)data[i*6+k]=static_cast<ui16>(i*4+pattern[k]);}
            indices->unlock();
            auto shadows=gb::ces_entity::construct<gb::shape_3d>();shadows->tag="island.blob.shadows";
            auto geometry=shadows->get_component<gb::ces_geometry_3d_component>();geometry->bounding_mode=gb::ces_geometry_component::e_none;
            geometry->set_mesh(gb::mesh_3d::construct("island.blob.shadows",buffer,indices,glm::vec3(1),glm::vec3(-1)));
            auto shadow_config=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("blob_shadow.xml"));
            get_fabricator()->add_materials(shadows,shadow_config->get_materials_configurations());add_child(shadows);
            auto action=shadows->add_component<gb::ces_action_component>();
            action->set_update_callback([buffer,indices,editor_state,shadow_people,shadow_capacity](const gb::ces_entity_shared_ptr& entity,float){
                auto vertices=buffer->lock<gb::vbo::vertex_attribute_PTNTC>();ui32 count=0;
                const bool enabled=!std::getenv("GB_BLOB_SHADOWS")||std::string(std::getenv("GB_BLOB_SHADOWS"))!="0";
                const auto emit=[&](glm::vec4 p,float strength){
                    if(count>=shadow_capacity)return;
                    for(ui32 k=0;k<4;++k){
                        const float x=(k%2)?1.f:-1.f,z=(k/2)?1.f:-1.f;
                        auto& v=vertices[count*4+k];v={};
                        v.m_position=glm::vec3(p.x+x*p.w,p.y+.02f,p.z+z*p.w*.8f);
                        v.m_texcoord=glm::packHalf2x16(glm::vec2(x,z));
                        v.m_color=glm::u8vec4(255,255,255,static_cast<ui8>(enabled?strength*255.f:0.f));
                    }++count;
                };
                for(const auto& tree:editor_state->level.trees)emit(glm::vec4(tree.position[0],tree.position[1],tree.position[2],tree.scale*2.3f),.36f);
                for(const auto& weak:shadow_people)if(auto person=weak.lock()){
                    const auto p=person->get_component<gb::ces_transformation_3d_component>()->get_position();
                    emit(glm::vec4(p,.75f),.32f);
                }
                for(ui32 i=count*4;i<shadow_capacity*4;++i)vertices[i]={};buffer->unlock();indices->unlock(count*6,false);
                entity->visible=count>0;entity->visible_in_next_frame=count>0;
            });
            std::cout<<"Blob shadows: trees="<<tree_shadows.size()<<" residents="<<shadow_people.size()<<" draws=1"<<std::endl;
        }
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
            labels[i]->font_name="Vera.ttf";
            labels[i]->font_size=18.f;
            labels[i]->font_color=glm::u8vec4(246,244,225,255);
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
        auto camera=std::make_shared<gb::camera_3d>(60.f,resident_inspect?.1f:.5f,6000.f,glm::ivec4(0,0,resolution.x,resolution.y),true);
        set_camera_3d(camera);
        const bool manual_gamma=!gb::graphics_context::get_current_context()->is_output_srgb();
        std::cout<<"Island presentation sRGB="<<!manual_gamma<<std::endl;
        auto update_post=[post_settings,post_output,post_final,ao,camera,editor_state,manual_gamma](const gb::ces_entity_shared_ptr&,float){
            post_settings->sanitize();
            stylized_lighting_settings().sanitize();
            ao->set(glm::vec4(3.f,.06f,post_settings->enableSSAO?post_settings->ssaoIntensity:0.f,1.f),"ao_params");
            post_final->update(*post_settings,camera->get_position(),camera->get_look_at(),camera->get_mat_v(),camera->get_mat_p(),editor_state->level.terrain.config.water_y,manual_gamma);
            post_output->update(*post_settings,camera->get_position(),camera->get_look_at(),camera->get_mat_v(),camera->get_mat_p(),editor_state->level.terrain.config.water_y,manual_gamma);
        };

        auto orbit=std::make_shared<orbit_camera_input>(camera, terrain_mesh, config.water_y,resident_focus,tree_focus);
        update_post(nullptr,0);
        m_orbit_listener=orbit;
        auto zoom_update=gb::ces_entity::construct<gb::ces_entity>();
        zoom_update->tag="island.camera.zoom";
        std::weak_ptr<orbit_camera_input> weak_orbit=orbit;
        zoom_update->add_component<gb::ces_action_component>()->set_update_callback([weak_orbit,update_post](const gb::ces_entity_shared_ptr& entity,float dt){
            if(auto listener=weak_orbit.lock())listener->update_zoom(dt);
            // Capture the final camera pose for this frame, after the spring.
            update_post(entity,dt);
        });
        // Actions (order 2) precede ocean uniforms (6) and rendering (7).
        add_child(zoom_update);
        const auto input_context = get_transition()->get_input_context();
        m_input_context = input_context;
        input_context->add_listener(m_orbit_listener);
        if(m_resident_listener)input_context->add_listener(m_resident_listener);
        auto editor_ui=std::make_shared<TerrainEditorUI>(get_fabricator(),editor_state,resolution);
        editor_ui->bind_post_settings(post_settings);
        if(std::getenv("GB_POST_DEBUG_PANEL"))editor_ui->toggle_post_panel();
        add_child(editor_ui->root());
        auto editor_input=std::make_shared<TerrainEditorInput>(editor_state,editor_ui,camera);
        m_editor_listener=editor_input;input_context->add_listener(editor_input);
        auto textedit_system=std::make_shared<editor_textedit_system>(editor_ui);
        m_textedit_listener=textedit_system;input_context->add_listener(textedit_system);
        get_transition()->add_system(textedit_system);
        std::weak_ptr<TerrainEditorInput> weak_editor=editor_input;
        orbit->pointer_blocked=[weak_editor](glm::ivec2 p){auto input=weak_editor.lock();return input&&input->pointer_over_ui(p);};
        orbit->rotation_allowed=[weak_editor](){auto input=weak_editor.lock();return !input||input->camera_rotation_allowed();};
        orbit->surface_picker=[weak_editor](glm::ivec2 p,glm::vec3& hit){auto input=weak_editor.lock();return input&&input->pick_surface(p,hit);};
        auto cache=std::make_shared<TerrainEditorRenderCache>(editor_state);
        const bool seed_grass_enabled=!grass_enabled||std::string(grass_enabled)!="0";
        cache->set_seed_builder([seed_grass_enabled](const terrain_config& config){
            std::pair<terrain_mesh_data,std::vector<terrain_mesh_data>> seed;
            try{config.validate();}catch(const std::exception&){return seed;}
            const auto generated=terrain_generator::generate(config);
            auto surface=terrain_mesh_builder::build(generated);
            if(seed_grass_enabled)seed.second=terrain_mesh_builder::build_grass(surface,config);
            seed.first=editor_palette_mesh(std::move(surface));return seed;
        });
        cache->initialize(editor_palette_mesh(terrain_mesh),editor_legacy_grass);
        editor_state->consume_dirty();
        auto batch_entities=std::make_shared<std::map<std::string,gb::shape_3d_shared_ptr>>();
        auto retired=std::make_shared<std::deque<std::pair<int,gb::mesh_3d_shared_ptr>>>();
        const auto publish=[this,ocean_system,configuration,batch_entities,retired](const std::vector<EditorRenderBatch>& batches,bool streaming=true,const std::vector<prepared_editor_mesh>* prepared=nullptr){
            size_t index=0;
            for(const auto& batch:batches){
                const auto supplied=prepared?&prepared->at(index):nullptr;++index;
                auto found=batch_entities->find(batch.key);
                if(batch.mesh.indices.empty()){
                    if(found!=batch_entities->end()){
                        found->second->visible=false;found->second->visible_in_next_frame=false;
                        auto geometry=found->second->get_component<gb::ces_geometry_3d_component>();
                        if(auto old=geometry->get_mesh())retired->push_back({12,old->as_3d()});
                        geometry->set_mesh(nullptr);
                    }
                    continue;
                }
                gb::shape_3d_shared_ptr shape;
                if(found==batch_entities->end()){
                    shape=gb::ces_entity::construct<gb::shape_3d>();shape->tag=batch.key;
                    get_fabricator()->add_materials(shape,configuration->get_materials_configurations());add_child(shape);ocean_system->add_receiver(shape);
                    (*batch_entities)[batch.key]=shape;
                    if(batch.kind==EditorRenderBatch::Terrain){
                        auto mask=gb::ces_entity::construct<gb::shape_3d>();
                        mask->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
                        auto c=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("water_land.xml"));
                        get_fabricator()->add_materials(mask,c->get_materials_configurations());shape->add_child(mask);ocean_system->add_interaction(shape,mask);
                    }
                } else shape=found->second;
                auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
                if(auto old=geometry->get_mesh())retired->push_back({12,old->as_3d()});
                const auto guid=editor_batch_guid(batch);
                geometry->set_mesh(upload_mesh(batch.mesh,guid,0.f,streaming,supplied));shape->visible=true;shape->visible_in_next_frame=true;
            }
        };
        // Initial batches clip the existing smooth surfaces rather than regenerating them.
        publish(cache->rebuild(TerrainEditorDirty()),false);
        editor_state->building_surface=std::make_shared<const BuildingSurfaceSupport>(cache->terrain_parts());
        // Populate only the newly generated demo. Loading a level replaces this
        // state normally; authored/user levels are never injected with buildings.
        const char* reference_layout=std::getenv("GB_REFERENCE_COMPOSITION");
        if(!reference_layout||std::string(reference_layout)!="0"){
            const auto selected=editor_state->selected_building;
            const float yaw=editor_state->building_yaw;
            for(int house=0;house<2;++house){
                editor_state->selected_building=house==0?BuildingType::Forester:BuildingType::Woodcutter;
                editor_state->building_yaw=glm::radians(135.f);
                editor_state->building_surface_revision=editor_state->terrain_revision;
                const float desired_x=house==0?48.f:-8.f,desired_z=house==0?40.f:-5.f;
                float best=std::numeric_limits<float>::max(),chosen_x=0,chosen_z=0;
                for(int dz=-24;dz<=24;dz+=2)for(int dx=-24;dx<=24;dx+=2){
                    const float x=desired_x+dx,z=desired_z+dz;
                    const float score=float(dx*dx+dz*dz);
                    if(score>=best||editor_state->level_at(x,z)!=(house==0?4:8))continue;
                    if(editor_state->can_place_building(x,z)){best=score;chosen_x=x;chosen_z=z;}
                }
                if(best<std::numeric_limits<float>::max()&&editor_state->place_building(chosen_x,chosen_z))
                    std::cout<<"Reference building placed terrace="<<(house==0?4:8)<<" x="<<chosen_x<<" z="<<chosen_z<<std::endl;
            }
            editor_state->selected_building=selected;editor_state->building_yaw=yaw;
        }
        editor_state->building_surface_revision=editor_state->terrain_revision;
        auto buildings=std::make_shared<EditorBuildingRenderer>(get_fabricator(),editor_state);add_child(buildings->root());
        for(const auto& entity:editor_legacy_entities){entity->visible=false;entity->visible_in_next_frame=false;}
        terrain_mesh_data brush_quad;
        for(int i=0;i<4;++i){terrain_vertex v;v.position={{(i%2)?1.f:-1.f,0,(i/2)?1.f:-1.f}};v.normal={{0,1,0}};v.color={{255,255,255,255}};v.texcoord={{v.position[0],v.position[2]}};brush_quad.vertices.push_back(v);}
        brush_quad.indices={0,1,2,1,3,2};
        auto brush=gb::ces_entity::construct<gb::shape_3d>();brush->tag="editor.brush.preview";
        brush->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(brush_quad,"editor.brush.preview"));
        brush->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
        auto brush_config=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("editor_brush.xml"));
        get_fabricator()->add_materials(brush,brush_config->get_materials_configurations());brush->visible=false;add_child(brush);
        const auto save_level=[editor_state,weak_editor](){
            if(auto input=weak_editor.lock())input->finish_stroke();
            const auto chosen=gb::file_dialog::save_level();
            if(!chosen.error.empty())editor_state->error=chosen.error;
            else if(!chosen.cancelled&&editor_state->save(chosen.path))std::cout<<"Editor saved level"<<std::endl;
        };
        const auto load_level=[editor_state,weak_editor](){
            if(auto input=weak_editor.lock())input->finish_stroke();
            if(editor_state->is_dirty&&!gb::file_dialog::confirm_discard_level())return;
            const auto chosen=gb::file_dialog::open_level();
            if(!chosen.error.empty())editor_state->error=chosen.error;
            else if(!chosen.cancelled&&editor_state->load(chosen.path))std::cout<<"Editor loaded level"<<std::endl;
        };
        editor_ui->set_save_callback(save_level);editor_ui->set_load_callback(load_level);
        editor_input->set_save_callback(save_level);editor_input->set_load_callback(load_level);
        auto jobs=std::make_shared<editor_rebuild_jobs>();
        auto editor_update=gb::ces_entity::construct<gb::ces_entity>();editor_update->tag="island.terrain_editor.update";
        editor_update->add_component<gb::ces_action_component>()->set_update_callback([editor_input,editor_ui,editor_state,cache,jobs,publish,retired,brush,buildings,camera,editor_resident_system,ocean_system,water_entities,horizon,seabed,loaded_generation=uint64_t(0),loaded_config=config](const gb::ces_entity_shared_ptr&,float dt) mutable {
            editor_input->update(dt);
            const bool loaded=loaded_generation!=editor_state->load_generation;
            if(loaded){
                const auto& current=editor_state->level.terrain.config;
                ocean_system->set_mean_level(current.water_y);
                ocean_system->set_terrain_dimensions(current.layer_height,current.shore_height);
                const auto replace=[&](const gb::shape_3d_shared_ptr& shape,const terrain_mesh_data& data,const std::string& guid){
                    auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
                    if(auto old=geometry->get_mesh())retired->push_back({12,old->as_3d()});
                    geometry->set_mesh(upload_mesh(data,guid,current.water_y));
                };
                if(current.water_size!=loaded_config.water_size||current.water_y!=loaded_config.water_y){
                    const auto patches=ocean_mesh_builder::build(current.water_size,384);
                    for(size_t i=0;i<patches.size();++i)replace(water_entities->at(i),patches[i],"island.water."+std::to_string(i));
                    replace(horizon,ocean_mesh_builder::build_horizon(current.water_size,std::max(4096.f,current.water_size*2.f),384),"island.water.horizon");
                }
                if(current.layer_height!=loaded_config.layer_height||current.water_size!=loaded_config.water_size){
                    terrain_mesh_data bottom;const float s=current.water_size*.5f,y=-(current.underwater_layers_count+1)*current.layer_height;
                    for(const auto& p:{glm::vec3(-s,y,-s),glm::vec3(-s,y,s),glm::vec3(s,y,-s),glm::vec3(s,y,s)}){
                        terrain_vertex v;v.position={{p.x,p.y,p.z}};v.normal={{0,1,0}};v.color={{137,135,100,255}};v.editor_palette={{0,0}};bottom.vertices.push_back(v);
                    }
                    bottom.indices={0,1,2,2,1,3};replace(seabed,bottom,"island.seabed");
                }
                loaded_config=current;loaded_generation=editor_state->load_generation;
            }
            auto dirty=editor_state->consume_dirty();
            if(std::getenv("GB_EDITOR_PROFILE")&&(!dirty.empty()||jobs->worker.valid()||!jobs->uploads.empty()))
                std::cout<<"Editor frame_ms="<<dt*1000.f<<std::endl;
            if(loaded){jobs->pending=TerrainEditorDirty();jobs->regions.clear();jobs->uploads.clear();jobs->navigation.clear();jobs->generation=editor_state->load_generation;jobs->support.reset();}
            if(dirty.terrain||dirty.buildings){++jobs->terrain_revision;jobs->needs_navigation|=editor_resident_system!=nullptr;}
            merge_editor_dirty(jobs->pending,dirty);
            if(!dirty.empty()){
                auto region=dirty;
                for(auto it=jobs->regions.begin();it!=jobs->regions.end();){
                    if(region.x0<=it->x1+2&&region.x1+2>=it->x0&&region.z0<=it->z1+2&&region.z1+2>=it->z0){merge_editor_dirty(region,*it);it=jobs->regions.erase(it);}
                    else ++it;
                }
                jobs->regions.push_back(region);
            }
            if(jobs->worker.valid()&&jobs->worker.wait_for(std::chrono::seconds(0))==std::future_status::ready){
                try{
                    auto ready=jobs->worker.get();
                    if(ready.generation==editor_state->load_generation){
                        if(ready.support){jobs->support=std::move(ready.support);jobs->support_revision=ready.support_revision;}
                        for(auto& item:ready.batches)jobs->uploads[item.batch.key]=std::move(item);
                        if(!ready.navigation.empty()){jobs->navigation=std::move(ready.navigation);jobs->navigation_revision=ready.terrain_revision;}
                        std::cout<<"Editor rebuilt region="<<ready.dirty.x0<<","<<ready.dirty.z0<<".."<<ready.dirty.x1<<","<<ready.dirty.z1<<" revision="<<ready.revision
                            <<" flags="<<ready.dirty.terrain<<ready.dirty.grass<<ready.dirty.trees<<ready.dirty.climate
                            <<" worker_mesh_ms="<<ready.mesh_ms<<" worker_navigation_ms="<<ready.navigation_ms<<" queued="<<jobs->uploads.size()<<std::endl;
                    }
                }catch(const std::exception& e){editor_state->error=e.what();}
            }
            if(!jobs->worker.valid()&&(!jobs->pending.empty()||(jobs->needs_navigation&&!editor_state->stroke_active()))){
                auto snapshot=std::make_shared<TerrainEditorState>();snapshot->level=editor_state->level;
                snapshot->load_generation=editor_state->load_generation;snapshot->revision=editor_state->revision;snapshot->terrain_revision=editor_state->terrain_revision;
                const auto region=jobs->pending;const uint64_t terrain_revision=jobs->terrain_revision;
                const bool navigation=editor_resident_system&&jobs->needs_navigation&&!editor_state->stroke_active();
                if(navigation)jobs->needs_navigation=false;
                auto regions=std::move(jobs->regions);jobs->regions.clear();jobs->pending=TerrainEditorDirty();
                jobs->worker=std::async(std::launch::async,[cache,snapshot,region,regions,terrain_revision,navigation](){
                    editor_rebuild_result result;result.dirty=region;result.revision=snapshot->revision;result.generation=snapshot->load_generation;result.terrain_revision=terrain_revision;
                    const auto start=std::chrono::steady_clock::now();cache->set_state(snapshot);
                    std::map<std::string,EditorRenderBatch> batches;
                    for(const auto& area:regions)for(auto& batch:cache->rebuild(area))batches[batch.key]=std::move(batch);
                    for(auto& entry:batches){auto& batch=entry.second;
                        editor_pending_mesh item;item.prepared=prepare_editor_mesh(batch.mesh,editor_batch_guid(batch));
                        std::vector<terrain_vertex>().swap(batch.mesh.vertices);item.batch=std::move(batch);result.batches.push_back(std::move(item));
                    }
                    if(region.terrain){result.support=std::make_shared<const BuildingSurfaceSupport>(cache->terrain_parts());result.support_revision=snapshot->terrain_revision;}
                    const auto built=std::chrono::steady_clock::now();result.mesh_ms=std::chrono::duration<double,std::milli>(built-start).count();
                    if(navigation){
                        const auto parts=std::make_shared<const std::vector<terrain_mesh_data>>(cache->terrain_parts());
                        result.navigation.resize(8);std::vector<std::future<void>> workers;
                        const unsigned count=std::min(4u,std::max(1u,std::thread::hardware_concurrency()>1?std::thread::hardware_concurrency()-1:1));
                        for(unsigned thread=0;thread<count;++thread)workers.push_back(std::async(std::launch::async,[&,thread,count](){
                            for(unsigned i=thread;i<8;i+=count){
                                auto& nav=result.navigation[i];nav=build_resident_navigation(*parts,editor_level_height(snapshot->level.terrain.config,static_cast<int>(i)+1),1.f);
                                for(const auto& b:snapshot->level.buildings)if(b.terrace==static_cast<int>(i)+1){
                                    const auto& d=building_definition(b.type);resident_obstacle obstacle;
                                    obstacle.center={b.position[0],b.position[2]};obstacle.cosine=std::cos(b.yaw);obstacle.sine=std::sin(b.yaw);
                                    obstacle.half_width=d.half_width+building_clearance;obstacle.half_depth=d.half_depth+building_clearance;nav.obstacles.push_back(obstacle);
                                }
                                for(size_t cell=0;cell<nav.walkable.size();++cell)if(nav.walkable[cell]&&!nav.is_safe(nav.center(static_cast<int>(cell))))nav.walkable[cell]=0;
                            }
                        }));
                        for(auto& worker:workers)worker.get();
                    }
                    result.navigation_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-built).count();return result;
                });
            }
            // Coalesce repeated updates to each batch; upload at most one changed
            // batch per frame using existing fence-safe dynamic buffers.
            if(!jobs->uploads.empty()){
                const auto started=std::chrono::steady_clock::now();
                auto first=jobs->uploads.begin();std::vector<EditorRenderBatch> batch;std::vector<prepared_editor_mesh> prepared;
                batch.push_back(std::move(first->second.batch));prepared.push_back(std::move(first->second.prepared));jobs->uploads.erase(first);publish(batch,true,&prepared);
                if(std::getenv("GB_EDITOR_PROFILE"))std::cout<<"Editor upload_ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()<<std::endl;
            }
            if(!jobs->navigation.empty()&&jobs->navigation_revision==jobs->terrain_revision&&jobs->uploads.empty()&&!jobs->worker.valid()&&jobs->pending.empty()&&!editor_state->stroke_active()&&editor_resident_system){
                const auto started=std::chrono::steady_clock::now();editor_resident_system->apply_navigation(std::move(jobs->navigation));jobs->navigation.clear();
                if(std::getenv("GB_EDITOR_PROFILE"))std::cout<<"Editor apply_navigation_ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()<<std::endl;
            }
            if(jobs->support&&jobs->support_revision==editor_state->terrain_revision&&jobs->uploads.empty()&&!jobs->worker.valid()&&jobs->pending.empty()){
                editor_state->building_surface=std::move(jobs->support);editor_state->building_surface_revision=jobs->support_revision;
            }
            buildings->update(*editor_input,camera->get_mat_v());
            for(auto& old:*retired)--old.first;
            while(!retired->empty()&&retired->front().first<=0)retired->pop_front();
            const bool show=editor_input->brush_visible();brush->visible=show;brush->visible_in_next_frame=show;
            if(show){auto t=brush->get_component<gb::ces_transformation_3d_component>();t->set_position(editor_input->brush_position()+glm::vec3(0,.025f,0));t->set_scale(glm::vec3(editor_state->brush_size*.5f,1,editor_state->brush_size*.5f));}
            editor_ui->update();
        });add_child(editor_update);
        std::cout << "Island seed=" << config.seed << " layers=" << config.layers_count
                  << " vertices=" << terrain_mesh.vertices.size() << " triangles=" << terrain_mesh.indices.size()/3
                  << " generation_ms=" << std::chrono::duration<double,std::milli>(generated-start).count()
                  << " mesh_ms=" << std::chrono::duration<double,std::milli>(built-generated).count() << std::endl;
        std::cout<<"Ocean patches="<<water_patches.size()<<" terraces="<<config.layers_count<<"+"<<config.underwater_layers_count<<std::endl;
    }
}
