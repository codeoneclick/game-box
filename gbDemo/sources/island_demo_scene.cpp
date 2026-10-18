#include "terrain_lod_renderer.h"
#include "terrain_lod.h"
#include <cstring>
#include "island_demo_scene.h"
#include "graphics_settings.h"
#include "render_debug_controller.h"
#include "ces_render_system.h"
#include "render_pipeline.h"
#include "render_technique_ws.h"
#include "texture.h"
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
#include <atomic>
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
        // Shared edge tessellation keeps the detailed surface and horizon watertight.
        constexpr int ocean_grid_cells=256;
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
        prepared_editor_mesh prepare_editor_mesh(const terrain_mesh_data& data,const std::string& guid,float water_level=0.f,const TerrainLodVariant* lod=nullptr){
            prepared_editor_mesh result;result.vertices.resize(data.vertices.size());
            for(size_t i=0;i<data.vertices.size();++i){
                const auto& source=data.vertices[i];auto& vertex=result.vertices[i];vertex={};
                vertex.m_position=glm::vec3(source.position[0],source.position[1],source.position[2]);
                vertex.m_normal=glm::packSnorm4x8(glm::vec4(source.normal[0],source.normal[1],source.normal[2],0.f));
                vertex.m_tangent=glm::packSnorm4x8(glm::vec4(1.f,0.f,0.f,0.f));
                if(source.grass_bend[1]>0.f)vertex.m_tangent=glm::packSnorm4x8(glm::vec4(source.grass_bend[0],source.grass_bend[1],1.f,0.f));
                else if(source.tree_bend[1]>0.f)vertex.m_tangent=glm::packSnorm4x8(glm::vec4(source.tree_bend[0],source.tree_bend[1],0.f,1.f));
                vertex.m_texcoord=glm::packHalf2x16(glm::vec2(source.texcoord[0],source.texcoord[1]));
                if(source.stone_texture)vertex.m_tangent=glm::packSnorm4x8(glm::vec4(1.f,0.f,0.f,.25f));
                if(source.editor_palette[0]>=0.f){vertex.m_tangent=glm::packSnorm4x8(glm::vec4(0,0,0,-1));vertex.m_texcoord=glm::packHalf2x16(glm::vec2(source.editor_palette[0],source.editor_palette[1]));}
                vertex.m_color=glm::u8vec4(source.color[0],source.color[1],source.color[2],source.color[3]);
                if(source.outline_id){
                    if(source.outline_id>0x3fffffu||source.outline_id==0x3fc000u)throw std::overflow_error("Procedural outline identity exceeds packed vertex capacity");
                    const auto id=source.outline_id;
                    vertex.m_normal=glm::packSnorm4x8(glm::vec4(source.normal[0],source.normal[1],source.normal[2],float((id>>7)&127u)/127.f));
                    auto tangent=glm::unpackSnorm4x8(vertex.m_tangent);tangent.z=-float(id&127u)/127.f;
                    vertex.m_tangent=glm::packSnorm4x8(tangent);vertex.m_color.a=static_cast<uint8_t>(id>>14);
                }
                if(lod&&i<lod->morph_delta_xz.size()){
                    const auto delta=lod->morph_delta_xz[i];
                    if(!std::isfinite(delta[0])||!std::isfinite(delta[1])||std::abs(delta[0])>60000||std::abs(delta[1])>60000)throw std::invalid_argument("Invalid terrain morph delta");
                    auto tangent=glm::unpackSnorm4x8(vertex.m_tangent);tangent.x=lod->morph_normal_xz[i][0];tangent.y=lod->morph_normal_xz[i][1];vertex.m_tangent=glm::packSnorm4x8(tangent);
                    vertex.m_texcoord=glm::packHalf2x16(glm::vec2(delta[0],delta[1]));
                    result.minimum=glm::min(result.minimum,vertex.m_position+glm::vec3(delta[0],0,delta[1]));result.maximum=glm::max(result.maximum,vertex.m_position+glm::vec3(delta[0],0,delta[1]));
                }
                result.minimum=glm::min(result.minimum,vertex.m_position);result.maximum=glm::max(result.maximum,vertex.m_position);
            }
            if(guid.find("island.water")==0){result.minimum.y=water_level-1.f;result.maximum.y=water_level+1.f;}
            if(guid.find("island.grass.")==0){result.minimum-=glm::vec3(1.05f,.2f,1.05f);result.maximum+=glm::vec3(1.05f,.2f,1.05f);}
            if(guid.find("island.trees.")==0){result.minimum-=glm::vec3(.4f,0,.4f);result.maximum+=glm::vec3(.4f,0,.4f);}
            return result;
        }
        struct editor_pending_mesh {EditorRenderBatch batch;prepared_editor_mesh prepared;};
        struct editor_detail_source {std::shared_ptr<const terrain_mesh_data> mesh;uint64_t revision=0;};
        struct editor_rebuild_result {
            std::vector<editor_pending_mesh> batches;
            std::vector<resident_navigation> navigation;
            TerrainEditorDirty dirty;
            std::shared_ptr<const BuildingSurfaceSupport> support;
            uint64_t support_revision=0;
            uint64_t generation=0,revision=0,terrain_revision=0;
            double mesh_ms=0,navigation_ms=0;
            TerrainLodVersion version;
            std::map<std::string,editor_detail_source> detail_sources;
            std::shared_ptr<const std::vector<terrain_mesh_data>> terrain_parts;
            std::chrono::steady_clock::time_point requested;
            bool cancelled=false;
            uint64_t detail_epoch=0;
        };
        struct editor_rebuild_jobs {
            std::future<editor_rebuild_result> worker;
            std::future<editor_rebuild_result> detail_worker;
            TerrainEditorDirty pending;
            std::vector<TerrainEditorDirty> regions;
            std::map<std::string,editor_pending_mesh> uploads;
            std::vector<resident_navigation> navigation;
            uint64_t terrain_revision=0,navigation_revision=0,generation=0;
            bool needs_navigation=false;
            std::shared_ptr<const BuildingSurfaceSupport> support;
            uint64_t support_revision=0;
            std::map<std::string,editor_detail_source> detail_sources;
            std::shared_ptr<const std::vector<terrain_mesh_data>> detail_parts;
            uint64_t detail_parts_revision=0;
            std::shared_ptr<std::atomic<uint64_t>> detail_epoch=std::make_shared<std::atomic<uint64_t>>(0);
            TerrainLodVersion visual_version;
            std::chrono::steady_clock::time_point pending_requested,visual_requested;
            float idle_seconds=0;
            bool visual_is_refinement=false;
            std::map<std::string,editor_detail_source> detail_inflight;
            uint64_t detail_job_generation=0,detail_job_epoch=0,failed_detail_epoch=std::numeric_limits<uint64_t>::max();
            ~editor_rebuild_jobs(){++*detail_epoch;}
        };
        void merge_editor_dirty(TerrainEditorDirty& destination,const TerrainEditorDirty& source){
            if(source.empty())return;
            if(destination.empty())destination=source;
            else {
                destination.x0=std::min(destination.x0,source.x0);destination.z0=std::min(destination.z0,source.z0);
                destination.x1=std::max(destination.x1,source.x1);destination.z1=std::max(destination.z1,source.z1);
                destination.terrain|=source.terrain;destination.grass|=source.grass;
                destination.trees|=source.trees;destination.climate|=source.climate;destination.buildings|=source.buildings;destination.stairs|=source.stairs;destination.lod|=source.lod;destination.lod_all|=source.lod_all;
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
                               const terrain_mesh_data& surface, float water_y, const glm::vec3& resident_focus,const glm::vec3& tree_focus,const terrain_config& config) :
                m_camera(camera), m_surface(std::make_shared<terrain_mesh_data>(surface)), m_water_y(water_y)
            {
                const float scale=config.island_size/256.f;
                m_radius*=scale;m_max_radius=std::max(700.f,config.world_size*1.5f);
                if(const char* inspect=std::getenv("GB_OCEAN_INSPECT")){
                    if(std::string(inspect)=="1"){m_target=glm::vec3(-85.f,0.f,100.f);m_radius=85.f;m_pitch=35.f;}
                    if(std::string(inspect)=="coast"){
                        for(size_t i=0;i+2<surface.indices.size();i+=3){
                            const auto& a=surface.vertices[surface.indices[i]];
                            if(a.normal[1]>.99f&&std::abs(a.position[1]+config.layer_height)<.01f){
                                m_target=glm::vec3(0);
                                for(int j=0;j<3;++j){const auto& p=surface.vertices[surface.indices[i+j]].position;m_target+=glm::vec3(p[0],p[1],p[2])/3.f;}
                                m_radius=45.f;m_pitch=50.f;break;
                            }
                        }
                    }
                    if(std::string(inspect)=="reef"){
                        const auto reef_mesh=build_ocean_reefs(config);
                        if(!reef_mesh.vertices.empty()){
                            m_target=glm::vec3(0);
                            const size_t count=std::min(size_t(144),reef_mesh.vertices.size());
                            for(size_t i=0;i<count;++i){const auto& p=reef_mesh.vertices[i].position;m_target+=glm::vec3(p[0],p[1],p[2]);}
                            m_target/=float(count);m_target.y=config.water_y+.5f;
                            m_radius=55.f;m_pitch=45.f;
                        }
                    }
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

        gb::mesh_3d_shared_ptr upload_mesh(const terrain_mesh_data& data, const std::string& guid,float water_level=0.f,bool streaming=false,const prepared_editor_mesh* supplied=nullptr,const TerrainLodVariant* lod=nullptr)
        {
            prepared_editor_mesh local;if(!supplied){local=prepare_editor_mesh(data,guid,water_level,lod);supplied=&local;}
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
        using editor_lod_cache=std::map<std::string,std::shared_ptr<const TerrainLodGeneration>>;
        void prepare_editor_lod(EditorRenderBatch& batch,const TerrainEditorState& state,const TerrainLodSettings& settings,editor_lod_cache& cache){
            if(batch.kind!=EditorRenderBatch::Terrain)return;
            if(batch.mesh.indices.empty()){cache.erase(batch.key);auto empty=std::make_shared<TerrainLodGeneration>();empty->version.load_generation=state.load_generation;empty->version.terrain_revision=state.terrain_revision;empty->version.settings_revision=settings.revision;batch.lod=empty;return;}
            const auto& config=state.level.terrain.config;const float cell=config.world_size/config.resolution;
            int kind=0,group=0,part=0;std::string key=batch.key;std::replace(key.begin(),key.end(),'_',' ');std::istringstream(key)>>kind>>group>>part;
            const int groups=(config.resolution+63)/64,gx=group%groups,gz=group/groups;const float half=config.world_size*.5f;
            TerrainContourConstraints constraints;constraints.bounds={{gx*64*cell-half,gz*64*cell-half,std::min(config.resolution,(gx+1)*64)*cell-half,std::min(config.resolution,(gz+1)*64)*cell-half}};
            const auto protect=[&](float x,float z,float r){const auto& b=constraints.bounds;if(x+r>=b[0]&&z+r>=b[1]&&x-r<=b[2]&&z-r<=b[3])constraints.protected_disks.push_back({{x,z,r,0}});};
            if(settings.contourSimplificationPercent>0){
                for(const auto& building:state.level.buildings){const auto& d=building_definition(building.type);protect(building.position[0],building.position[2],std::sqrt(d.half_width*d.half_width+d.half_depth*d.half_depth)+building_clearance+cell*2);}
                for(const auto& tree:state.level.trees)protect(tree.position[0],tree.position[2],4.f*tree.scale+cell);
                for(const auto& stairs:state.level.stairs)protect(stairs.position[0],stairs.position[2],std::sqrt(stairs_width*stairs_width+stairs_run*stairs_run)*.5f+cell);
            }
            uint64_t hash=1469598103934665603ull;const auto word=[&](uint32_t v){hash^=v;hash*=1099511628211ull;};const auto scalar=[&](float f){uint32_t v;std::memcpy(&v,&f,4);word(v);};
            word(settings.lodCount);scalar(settings.contourSimplificationPercent);scalar(cell);for(auto x:constraints.bounds)scalar(x);for(auto d:constraints.protected_disks)for(float f:d)scalar(f);
            for(auto& v:batch.mesh.vertices){for(float x:v.position)scalar(x);for(float x:v.normal)scalar(x);for(auto x:v.color)word(x);for(float x:v.editor_palette)scalar(x);word(v.outline_id);}for(auto x:batch.mesh.indices)word(x);
            TerrainLodVersion version;version.load_generation=state.load_generation;version.terrain_revision=state.terrain_revision;version.settings_revision=settings.revision;
            auto found=cache.find(batch.key);
            if(found!=cache.end()&&found->second->signature==hash){auto copy=std::make_shared<TerrainLodGeneration>(*found->second);copy->version=version;batch.lod=copy;}
            else {auto generation=std::make_shared<TerrainLodGeneration>(build_terrain_lod_generation(batch.mesh,settings,constraints,cell,version));generation->signature=hash;batch.lod=generation;}
            cache[batch.key]=batch.lod;
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
        auto lod_settings=std::make_shared<TerrainLodSettings>();
        lod_settings->wireframe_available=gb::material::supports_wireframe();
        const auto graphics_path=graphics_settings_path();
        auto saved_post=stylized_post_settings_from_environment();
        auto saved_lighting=lighting_settings_from_environment();
        std::string graphics_error;
        if(load_graphics_settings(graphics_path,saved_post,saved_lighting,*lod_settings,graphics_error))
            std::cout<<"Graphics settings loaded: "<<graphics_path<<std::endl;
        else std::cerr<<"Graphics settings defaults: "<<graphics_path<<": "<<graphics_error<<std::endl;
        stylized_lighting_settings()=lighting_settings_from_environment(&saved_lighting);
        auto technique_uniforms=add_component<gb::ces_render_technique_uniforms_component>();
        std::array<glm::ivec2,2> shadow_map_sizes={{glm::ivec2(0),glm::ivec2(0)}};
        for(const auto& technique:get_transition()->get_system<gb::ces_render_system>()->get_render_pipeline()->get_ws_techniques()){
            const auto texture=technique->get_color_attachment_texture();
            if(!texture)continue;
            for(int i=0;i<2;++i)if(texture->get_guid()==std::string(i==0?"ws.sun.near":"ws.sun.far")+".attachment_01")
                shadow_map_sizes[i]=glm::ivec2(texture->get_width(),texture->get_height());
        }
        auto sun_shadow_system=std::make_shared<ces_sun_shadow_system>(get_fabricator(),shadow_map_sizes);
        sun_shadow_system->initialize(shared_from_this());
        get_transition()->add_system(sun_shadow_system);
        auto post_settings=std::make_shared<StylizedPostProcessSettings>(stylized_post_settings_from_environment(&saved_post));
        auto render_debug=std::make_shared<RenderDebugController>(get_fabricator(),get_transition()->get_system<gb::ces_render_system>()->get_render_pipeline(),post_settings);
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
        for(const auto& technique:get_transition()->get_system<gb::ces_render_system>()->get_render_pipeline()->get_ws_techniques()){
            if(technique->get_name().find("ws.water.distance")!=std::string::npos){
                const auto texture=technique->get_color_attachment_texture();
                if(texture)ocean.interaction_resolution=float(texture->get_width());
            }
        }
        auto ocean_system=std::make_shared<ces_ocean_system>(ocean);
        ocean_system->set_terrain_dimensions(config.layer_height,config.shore_height);
        ocean_system->set_world_dimensions(config.world_size,config.water_size);
        sun_shadow_system->set_world_dimensions(config.world_size);
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
            if(water){
                // Foam uses its own compacted index list, sharing the water VBO.
                materials.erase(std::remove_if(materials.begin(),materials.end(),[](const std::shared_ptr<gb::configuration>& c){
                    return std::static_pointer_cast<gb::material_configuration>(c)->get_technique_name()=="ws.ocean.foam";
                }),materials.end());
            }
            get_fabricator()->add_materials(shape,materials);
            add_child(shape);
            if(water){
                gb::ces_entity_shared_ptr foam;
                if(guid!="island.water.horizon"){
                    foam=gb::ces_entity::construct<gb::shape_3d>();foam->tag=guid+".foam";
                    std::vector<std::shared_ptr<gb::configuration>> foam_materials;
                    for(const auto& item:water_configuration->get_materials_configurations())
                        if(std::static_pointer_cast<gb::material_configuration>(item)->get_technique_name()=="ws.ocean.foam")foam_materials.push_back(item);
                    get_fabricator()->add_materials(foam,foam_materials);add_child(foam);
                }
                ocean_system->add_surface(shape,foam,data,guid=="island.water.horizon");
            }else {
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
            if(std::string(name)=="water_distance")ocean_system->add_receiver(quad);
        }
        editor_legacy_entities.push_back(add_shape(editor_palette_mesh(terrain_mesh),"island.terrain"));
        // Grass receives ordinary lighting/SSAO but must not become a water
        // interaction mask: these dry blades cannot generate shoreline foam.
        const char* grass_enabled=std::getenv("GB_ISLAND_GRASS");
        if (!grass_enabled || std::string(grass_enabled)!="0")
        {
            editor_legacy_grass=terrain_mesh_builder::build_grass(terrain_mesh,config);
            size_t triangles=0;
            for(const auto& batch:editor_legacy_grass)triangles+=batch.indices.size()/3;
            // Only the editor chunks are rendered; avoid uploading a second, hidden meadow.
            std::cout<<"Island grass: terraces=4-7 (summit first), batches="<<editor_legacy_grass.size()<<" triangles="<<triangles<<std::endl;
        }
        glm::vec3 tree_focus(-24,10,24);
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
        const auto water_patches=ocean_mesh_builder::build(config.water_size,ocean_grid_cells);
        auto water_entities=std::make_shared<std::vector<gb::shape_3d_shared_ptr>>();
        for(size_t i=0;i<water_patches.size();++i)water_entities->push_back(add_shape(water_patches[i],"island.water."+std::to_string(i),true));
        auto horizon=add_shape(ocean_mesh_builder::build_horizon(config.water_size,200000.f,ocean_grid_cells),"island.water.horizon",true);
        // Closed rocks use water_mask (not the terrain-only projection), so the
        // existing wave field creates foam at their actual moving waterline.
        auto reefs=add_shape(build_ocean_reefs(config),"ocean.prop.reefs");
        const auto props=build_ocean_demo_props(ocean);
        for(size_t i=0;i<props.size();++i){
            auto p=props[i];const float radius=std::hypot(p.position.x,p.position.z);
            if(radius>.001f){
                const float angle=std::atan2(p.position.z,p.position.x),coast=terrain_generator::coast_radius(config,angle);
                const float old_coast=coast*256.f/config.island_size;
                const float relocated=std::max(0.f,radius+coast-old_coast);p.position.x*=relocated/radius;p.position.z*=relocated/radius;
            }
            auto shape=add_shape(p.mesh,"ocean.prop."+std::to_string(i));
            auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
            geometry->bounding_mode=gb::ces_geometry_component::e_radius;
            float bound_radius=0;for(const auto& vertex:p.mesh.vertices)bound_radius=std::max(bound_radius,glm::length(glm::vec3(vertex.position[0],vertex.position[1],vertex.position[2])));
            geometry->set_bounding_radius(bound_radius);
            auto transform=shape->get_component<gb::ces_transformation_3d_component>();
            transform->set_position(glm::vec3(p.position.x,p.position.y,p.position.z));transform->set_rotation(glm::vec3(0,p.yaw,0));
            if(p.floating){auto buoyancy=shape->add_component<ces_buoyancy_component>();buoyancy->settings=p.buoyancy;buoyancy->probes=p.probes;buoyancy->state.position=p.position;buoyancy->state.yaw=p.yaw;}
        }

        glm::vec3 resident_focus(-62.f,1.45f,74.f);
        try {
            auto resident=create_island_resident_demo(get_fabricator(),terrain_mesh,config);
            resident_focus=resident.focus;for(const auto& shape:resident.shapes)add_child(shape);
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
            action->set_update_callback([buffer,people,history,max_quads,editor_state,previous=std::vector<glm::vec3>(people.size(),glm::vec3(0)),initialized=false](const gb::ces_entity_shared_ptr&,float dt) mutable {
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
                        const float span=editor_state->level.terrain.config.world_size;
                        const glm::vec2 uv=(glm::vec2(p.x+dx*radius,p.z+dz*radius)+span*.5f)/span;
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
        auto update_post=[render_debug,post_settings,post_output,post_final,ao,camera,editor_state,manual_gamma,ocean_system](const gb::ces_entity_shared_ptr&,float){
            post_settings->sanitize();
            ocean_system->set_optics_intensity(glm::vec3(
                post_settings->enableWaterReflection?post_settings->waterReflectionIntensity:0.f,
                post_settings->enableWaterRefraction?post_settings->waterRefractionIntensity:0.f,
                post_settings->enableWaterCaustics?post_settings->waterCausticsIntensity:0.f));
            render_debug->update();
            stylized_lighting_settings().sanitize();
            ao->set(glm::vec4(3.f,.06f,post_settings->enableSSAO?post_settings->ssaoIntensity:0.f,1.f),"ao_params");
            post_final->update(*post_settings,camera->get_position(),camera->get_look_at(),camera->get_mat_v(),camera->get_mat_p(),editor_state->level.terrain.config.water_y,manual_gamma);
            post_output->update(*post_settings,camera->get_position(),camera->get_look_at(),camera->get_mat_v(),camera->get_mat_p(),editor_state->level.terrain.config.water_y,manual_gamma);
        };

        auto orbit=std::make_shared<orbit_camera_input>(camera, terrain_mesh, config.water_y,resident_focus,tree_focus,config);
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
        editor_ui->bind_terrain_lod_settings(lod_settings);
        editor_ui->bind_render_debug(render_debug->state());
        editor_ui->set_graphics_save_callback([post_settings,graphics_path,lod_settings] {
            std::string error;
            if(!save_graphics_settings(graphics_path,*post_settings,stylized_lighting_settings(),*lod_settings,error)) {
                std::cerr<<"Graphics settings save failed: "<<graphics_path<<": "<<error<<std::endl;
                return false;
            }
            std::cout<<"Graphics settings saved: "<<graphics_path<<std::endl;
            return true;
        });
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
        std::vector<terrain_mesh_data>().swap(editor_legacy_grass);
        editor_state->consume_dirty();
        auto batch_entities=std::make_shared<std::map<std::string,gb::shape_3d_shared_ptr>>();
        auto retired=std::make_shared<std::deque<std::pair<int,gb::mesh_3d_shared_ptr>>>();
        const bool lod_enabled=!std::getenv("GB_TERRAIN_LOD")||std::string(std::getenv("GB_TERRAIN_LOD"))!="0";
        auto lod_cache=std::make_shared<editor_lod_cache>();
        auto lod_renderer=std::make_shared<TerrainLodRenderer>(
            [](const TerrainLodVariant& variant,const std::string& name){return upload_mesh(variant.mesh,name,0.f,false,nullptr,&variant);},
            [this,ocean_system,configuration](const std::string& tag,int kind){
                auto shape=gb::ces_entity::construct<gb::shape_3d>();shape->tag=tag;
                if(kind==0){get_fabricator()->add_materials(shape,configuration->get_materials_configurations());ocean_system->add_receiver(shape);shape->visible=false;shape->visible_in_next_frame=false;}
                if(kind==2){auto c=std::static_pointer_cast<gb::shape_3d_configuration>(get_transition()->get_configuration_accessor()->get_shape_3d_configuration("water_land.xml"));get_fabricator()->add_materials(shape,c->get_materials_configurations());shape->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;}
                add_child(shape);return shape;
            },
            [this,retired](const gb::shape_3d_shared_ptr& shape){if(!shape)return;shape->visible=false;shape->visible_in_next_frame=false;auto geometry=shape->get_component<gb::ces_geometry_3d_component>();if(auto mesh=geometry->get_mesh())retired->push_back({12,mesh->as_3d()});geometry->set_mesh(nullptr);remove_child(shape);},
            [ocean_system](const gb::shape_3d_shared_ptr& source,const gb::shape_3d_shared_ptr& mask){ocean_system->add_interaction(source,mask);},
            [retired](const gb::mesh_3d_shared_ptr& mesh){if(mesh)retired->push_back({12,mesh});},
            [sun_shadow_system](const std::vector<TerrainShadowCaster>& casters){sun_shadow_system->set_terrain_casters(casters);});
        const auto publish=[this,ocean_system,configuration,batch_entities,retired,lod_renderer,lod_enabled](const std::vector<EditorRenderBatch>& batches,bool streaming=true,const std::vector<prepared_editor_mesh>* prepared=nullptr){
            size_t index=0;
            for(const auto& batch:batches){
                const auto supplied=prepared?&prepared->at(index):nullptr;++index;
                if(lod_enabled&&batch.kind==EditorRenderBatch::Terrain){lod_renderer->stage(batch.key,batch.lod);continue;}
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
        auto initial_batches=cache->rebuild(TerrainEditorDirty());
        if(lod_enabled){const auto settings=*lod_settings;auto initial_job=std::async(std::launch::async,[&initial_batches,editor_state,lod_cache,settings](){for(auto& batch:initial_batches)prepare_editor_lod(batch,*editor_state,settings,*lod_cache);});initial_job.get();}
        publish(initial_batches,false);
        if(lod_enabled){TerrainLodVersion v;v.load_generation=editor_state->load_generation;v.terrain_revision=editor_state->terrain_revision;v.settings_revision=lod_settings->revision;lod_renderer->publish_ready(v);}
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
                const float scale=config.island_size/256.f;
                const float desired_x=(house==0?48.f:-8.f)*scale,desired_z=(house==0?40.f:-5.f)*scale;
                float best=std::numeric_limits<float>::max(),chosen_x=0,chosen_z=0;
                for(int dz=-24;dz<=24;dz+=2)for(int dx=-24;dx<=24;dx+=2){
                    // Search the same relative neighborhood after resizing the
                    // island, while keeping each house on its intended terrace.
                    const float x=desired_x+dx*scale,z=desired_z+dz*scale;
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
        auto stairs_preview=gb::ces_entity::construct<gb::shape_3d>();stairs_preview->tag="editor.stairs.preview";
        get_fabricator()->add_materials(stairs_preview,configuration->get_materials_configurations());
        // One local mesh; cursor movement only updates its transform, never GPU buffers.
        EditorStairs preview_stairs;preview_stairs.id=1;preview_stairs.lower_terrace=2;
        stairs_preview->get_component<gb::ces_geometry_3d_component>()->set_mesh(upload_mesh(build_stairs_mesh(preview_stairs,terrain_config()),"editor.stairs.preview"));
        stairs_preview->get_component<gb::ces_geometry_3d_component>()->bounding_mode=gb::ces_geometry_component::e_none;
        ocean_system->add_receiver(stairs_preview);buildings->set_stairs_preview(stairs_preview);

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
        editor_update->add_component<gb::ces_action_component>()->set_update_callback([editor_input,editor_ui,editor_state,cache,jobs,publish,retired,brush,buildings,camera,lod_settings,lod_renderer,lod_cache,lod_enabled,render_debug,lod_seen=lod_settings->revision,lod_applied=lod_settings->revision,lod_elapsed=0.f,editor_resident_system,ocean_system,sun_shadow_system,water_entities,horizon,reefs,seabed,loaded_generation=uint64_t(0),stairs_revision=std::numeric_limits<uint64_t>::max(),loaded_config=config](const gb::ces_entity_shared_ptr&,float dt) mutable {
            const auto profile_begin=std::chrono::steady_clock::now();
            editor_input->update(dt);
            const auto profile_input_end=std::chrono::steady_clock::now();
            const bool loaded=loaded_generation!=editor_state->load_generation;
            if(loaded){
                const auto& current=editor_state->level.terrain.config;
                ocean_system->set_mean_level(current.water_y);
                ocean_system->set_terrain_dimensions(current.layer_height,current.shore_height);
                ocean_system->set_world_dimensions(current.world_size,current.water_size);
                sun_shadow_system->set_world_dimensions(current.world_size);
                const auto replace=[&](const gb::shape_3d_shared_ptr& shape,const terrain_mesh_data& data,const std::string& guid){
                    auto geometry=shape->get_component<gb::ces_geometry_3d_component>();
                    if(auto old=geometry->get_mesh())retired->push_back({12,old->as_3d()});
                    geometry->set_mesh(upload_mesh(data,guid,current.water_y));
                };
                if(current.water_size!=loaded_config.water_size||current.water_y!=loaded_config.water_y){
                    const auto patches=ocean_mesh_builder::build(current.water_size,ocean_grid_cells);
                    for(size_t i=0;i<patches.size();++i){
                        replace(water_entities->at(i),patches[i],"island.water."+std::to_string(i));
                        ocean_system->reset_surface(water_entities->at(i),patches[i]);
                    }
                    const auto horizon_data=ocean_mesh_builder::build_horizon(current.water_size,std::max(200000.f,current.water_size*2.f),ocean_grid_cells);
                    replace(horizon,horizon_data,"island.water.horizon");ocean_system->reset_surface(horizon,horizon_data);
                }
                if(current.layer_height!=loaded_config.layer_height||current.water_size!=loaded_config.water_size){
                    terrain_mesh_data bottom;const float s=current.water_size*.5f,y=-(current.underwater_layers_count+1)*current.layer_height;
                    for(const auto& p:{glm::vec3(-s,y,-s),glm::vec3(-s,y,s),glm::vec3(s,y,-s),glm::vec3(s,y,s)}){
                        terrain_vertex v;v.position={{p.x,p.y,p.z}};v.normal={{0,1,0}};v.color={{137,135,100,255}};v.editor_palette={{0,0}};bottom.vertices.push_back(v);
                    }
                    bottom.indices={0,1,2,2,1,3};replace(seabed,bottom,"island.seabed");
                }
                replace(reefs,build_ocean_reefs(current),"ocean.prop.reefs");
                loaded_config=current;loaded_generation=editor_state->load_generation;
            }
            auto dirty=editor_state->consume_dirty();
            // Bind immediately on authored stair changes so deletion cancels an
            // active traversal while regional mesh/navigation work is pending.
            if(editor_resident_system&&(loaded||stairs_revision==std::numeric_limits<uint64_t>::max()||
               (stairs_revision!=editor_state->revision&&(dirty.stairs||dirty.terrain)))){
                editor_resident_system->apply_stairs(editor_state->level.stairs,editor_state->level.terrain.config);
            }
            stairs_revision=editor_state->revision;
            if(std::getenv("GB_EDITOR_PROFILE")&&(!dirty.empty()||jobs->worker.valid()||!jobs->uploads.empty()))
                std::cout<<"Editor frame_ms="<<dt*1000.f<<std::endl;
            if(loaded){jobs->pending=TerrainEditorDirty();jobs->regions.clear();jobs->uploads.clear();jobs->navigation.clear();jobs->generation=editor_state->load_generation;jobs->support.reset();jobs->detail_sources.clear();jobs->detail_parts.reset();++*jobs->detail_epoch;}
            if(dirty.terrain||dirty.buildings){++jobs->terrain_revision;jobs->needs_navigation|=editor_resident_system!=nullptr;}
            if(lod_enabled){
                if(lod_seen!=lod_settings->revision){lod_seen=lod_settings->revision;lod_elapsed=0;}else lod_elapsed+=dt;
                if(lod_applied!=lod_settings->revision&&lod_elapsed>=.15f){if(dirty.empty()){dirty.x0=dirty.z0=0;dirty.x1=dirty.z1=editor_state->level.terrain.config.resolution-1;}dirty.lod=dirty.lod_all=true;lod_applied=lod_settings->revision;}
                if(dirty.terrain||dirty.buildings||dirty.stairs||dirty.trees)dirty.lod=true;
            }
            jobs->idle_seconds+=dt;
            if(!dirty.empty()){jobs->idle_seconds=0;++*jobs->detail_epoch;if(jobs->pending.empty())jobs->pending_requested=std::chrono::steady_clock::now();}
            merge_editor_dirty(jobs->pending,dirty);
            if(!dirty.empty()){
                auto region=dirty;
                for(auto it=jobs->regions.begin();it!=jobs->regions.end();){
                    if(region.x0<=it->x1+2&&region.x1+2>=it->x0&&region.z0<=it->z1+2&&region.z1+2>=it->z0){merge_editor_dirty(region,*it);it=jobs->regions.erase(it);}
                    else ++it;
                }
                jobs->regions.push_back(region);
            }
            const auto terrain_uploads_pending=[&](){for(const auto& entry:jobs->uploads)if(entry.second.batch.kind==EditorRenderBatch::Terrain)return true;return false;};
            if(!terrain_uploads_pending()&&jobs->worker.valid()&&jobs->worker.wait_for(std::chrono::seconds(0))==std::future_status::ready){
                try{
                    auto ready=jobs->worker.get();
                    if(ready.generation==editor_state->load_generation){
                        jobs->visual_version=ready.version;jobs->visual_requested=ready.requested;jobs->visual_is_refinement=false;
                        for(auto& source:ready.detail_sources)jobs->detail_sources[source.first]=std::move(source.second);
                        if(ready.terrain_parts){jobs->detail_parts=std::move(ready.terrain_parts);jobs->detail_parts_revision=ready.terrain_revision;}
                        for(auto& item:ready.batches){
                            jobs->uploads[item.batch.key]=std::move(item);
                        }
                        std::cout<<"Editor rebuilt region="<<ready.dirty.x0<<","<<ready.dirty.z0<<".."<<ready.dirty.x1<<","<<ready.dirty.z1<<" revision="<<ready.revision
                            <<" flags="<<ready.dirty.terrain<<ready.dirty.grass<<ready.dirty.trees<<ready.dirty.climate
                            <<" worker_mesh_ms="<<ready.mesh_ms<<" worker_navigation_ms="<<ready.navigation_ms<<" queued="<<jobs->uploads.size()<<std::endl;
                    }
                }catch(const std::exception& e){editor_state->error=e.what();}
            }
            const bool needs_parts=jobs->needs_navigation&&!editor_state->stroke_active()&&(!jobs->detail_parts||jobs->detail_parts_revision!=jobs->terrain_revision);
            if(!jobs->worker.valid()&&(!jobs->pending.empty()||needs_parts)){
                auto snapshot=std::make_shared<TerrainEditorState>();snapshot->level=editor_state->level;
                snapshot->load_generation=editor_state->load_generation;snapshot->revision=editor_state->revision;snapshot->terrain_revision=editor_state->terrain_revision;
                const auto region=jobs->pending;const uint64_t terrain_revision=jobs->terrain_revision;const auto settings=*lod_settings;
                const bool navigation=editor_resident_system&&jobs->needs_navigation&&!editor_state->stroke_active();
                const auto requested=jobs->pending.empty()?std::chrono::steady_clock::now():jobs->pending_requested;
                auto regions=std::move(jobs->regions);jobs->regions.clear();jobs->pending=TerrainEditorDirty();
                jobs->worker=std::async(std::launch::async,[cache,snapshot,region,regions,terrain_revision,navigation,lod_enabled,settings,requested](){
                    editor_rebuild_result result;result.dirty=region;result.revision=snapshot->revision;result.generation=snapshot->load_generation;result.terrain_revision=terrain_revision;
                    result.version={snapshot->load_generation,snapshot->terrain_revision,settings.revision};result.requested=requested;
                    const auto start=std::chrono::steady_clock::now();cache->set_state(snapshot);
                    std::map<std::string,EditorRenderBatch> batches;
                    for(const auto& area:regions)for(auto& batch:cache->rebuild(area,!lod_enabled))batches[batch.key]=std::move(batch);
                    for(auto& entry:batches){auto& batch=entry.second;
                        if(lod_enabled&&batch.kind==EditorRenderBatch::Terrain){
                            batch.lod=std::make_shared<const TerrainLodGeneration>(build_terrain_lod_preview(std::move(batch.mesh),snapshot->level.terrain.config.world_size/snapshot->level.terrain.config.resolution,result.version));
                            result.detail_sources[batch.key]={batch.lod->source,snapshot->terrain_revision};
                        }
                        editor_pending_mesh item;if(batch.kind!=EditorRenderBatch::Terrain||!lod_enabled)item.prepared=prepare_editor_mesh(batch.mesh,editor_batch_guid(batch));
                        std::vector<terrain_vertex>().swap(batch.mesh.vertices);item.batch=std::move(batch);result.batches.push_back(std::move(item));
                    }
                    if(navigation)result.terrain_parts=std::make_shared<const std::vector<terrain_mesh_data>>(cache->terrain_parts());
                    const auto built=std::chrono::steady_clock::now();result.mesh_ms=std::chrono::duration<double,std::milli>(built-start).count();
                    return result;
                });
            }
            if(!terrain_uploads_pending()&&!jobs->worker.valid()&&jobs->pending.empty()&&jobs->detail_worker.valid()&&jobs->detail_worker.wait_for(std::chrono::seconds(0))==std::future_status::ready){
                try{
                    auto ready=jobs->detail_worker.get();
                    if(ready.generation==editor_state->load_generation){
                        const bool current=!ready.cancelled&&ready.detail_epoch==jobs->detail_epoch->load()&&ready.version.terrain_revision==editor_state->terrain_revision&&ready.version.settings_revision==lod_settings->revision;
                        if(current){
                            jobs->visual_version=ready.version;jobs->visual_requested=ready.requested;jobs->visual_is_refinement=true;
                            for(auto& item:ready.batches)jobs->uploads[item.batch.key]=std::move(item);
                            if(ready.support){jobs->support=std::move(ready.support);jobs->support_revision=ready.support_revision;}
                            if(!ready.navigation.empty()){jobs->navigation=std::move(ready.navigation);jobs->navigation_revision=ready.terrain_revision;}
                            if(std::getenv("GB_EDITOR_PROFILE"))std::cout<<"Editor refined lod_ms="<<ready.mesh_ms<<" navigation_ms="<<ready.navigation_ms<<std::endl;
                        }else{
                            for(auto& source:ready.detail_sources){auto found=jobs->detail_sources.find(source.first);if(found==jobs->detail_sources.end()||found->second.revision<source.second.revision)jobs->detail_sources[source.first]=std::move(source.second);}
                            if(ready.terrain_parts)jobs->needs_navigation=true;
                        }
                    }
                    jobs->detail_inflight.clear();
                }catch(const std::exception& e){
                    editor_state->error=e.what();std::cerr<<"Editor refinement failed: "<<e.what()<<std::endl;
                    if(jobs->detail_job_generation==editor_state->load_generation){
                        for(auto& source:jobs->detail_inflight){auto found=jobs->detail_sources.find(source.first);if(found==jobs->detail_sources.end()||found->second.revision<source.second.revision)jobs->detail_sources[source.first]=std::move(source.second);}
                        jobs->needs_navigation=true;
                    }
                    jobs->detail_inflight.clear();jobs->failed_detail_epoch=jobs->detail_job_epoch;
                }
            }
            if(!jobs->detail_worker.valid()&&!jobs->worker.valid()&&jobs->pending.empty()&&!terrain_uploads_pending()&&!editor_state->stroke_active()&&jobs->idle_seconds>=.1f&&jobs->failed_detail_epoch!=jobs->detail_epoch->load()&&
               (!jobs->detail_sources.empty()||(jobs->needs_navigation&&jobs->detail_parts&&jobs->detail_parts_revision==jobs->terrain_revision))){
                auto snapshot=std::make_shared<TerrainEditorState>();snapshot->level=editor_state->level;snapshot->load_generation=editor_state->load_generation;snapshot->terrain_revision=editor_state->terrain_revision;
                auto sources=std::move(jobs->detail_sources);jobs->detail_sources.clear();const auto settings=*lod_settings;
                const bool navigation=jobs->needs_navigation&&jobs->detail_parts&&jobs->detail_parts_revision==jobs->terrain_revision;
                const auto parts=navigation?jobs->detail_parts:std::shared_ptr<const std::vector<terrain_mesh_data>>();if(navigation)jobs->needs_navigation=false;
                const uint64_t terrain_revision=jobs->terrain_revision,epoch=jobs->detail_epoch->load();auto cancellation=jobs->detail_epoch;
                jobs->detail_inflight=sources;jobs->detail_job_generation=snapshot->load_generation;jobs->detail_job_epoch=epoch;
                jobs->detail_worker=std::async(std::launch::async,[snapshot,sources,parts,navigation,terrain_revision,lod_cache,settings,cancellation,epoch](){
                    editor_rebuild_result result;result.generation=snapshot->load_generation;result.terrain_revision=terrain_revision;result.version={snapshot->load_generation,snapshot->terrain_revision,settings.revision};
                    result.detail_sources=sources;result.terrain_parts=parts;result.requested=std::chrono::steady_clock::now();const auto start=result.requested;
                    result.detail_epoch=epoch;
                    for(const auto& source:sources){
                        if(cancellation->load()!=epoch){result.cancelled=true;return result;}
                        editor_pending_mesh item;item.batch.kind=EditorRenderBatch::Terrain;item.batch.key=source.first;if(source.second.mesh)item.batch.mesh=*source.second.mesh;
                        prepare_editor_lod(item.batch,*snapshot,settings,*lod_cache);std::vector<terrain_vertex>().swap(item.batch.mesh.vertices);result.batches.push_back(std::move(item));
                    }
                    auto built=std::chrono::steady_clock::now();result.mesh_ms=std::chrono::duration<double,std::milli>(built-start).count();
                    if(navigation&&cancellation->load()==epoch){
                        result.support=std::make_shared<const BuildingSurfaceSupport>(*parts);result.support_revision=snapshot->terrain_revision;
                        result.navigation.resize(16);std::vector<std::future<void>> workers;
                        const unsigned count=std::min(2u,std::max(1u,std::thread::hardware_concurrency()>1?std::thread::hardware_concurrency()-1:1));
                        for(unsigned thread=0;thread<count;++thread)workers.push_back(std::async(std::launch::async,[&,thread,count](){
                            for(unsigned i=thread;i<result.navigation.size()&&cancellation->load()==epoch;i+=count){
                                auto& nav=result.navigation[i];nav=build_resident_navigation(*parts,editor_level_height(snapshot->level.terrain.config,int(i)+1),1.f);
                                for(const auto& b:snapshot->level.buildings)if(b.terrace==int(i)+1){const auto& d=building_definition(b.type);resident_obstacle obstacle;
                                    obstacle.center={b.position[0],b.position[2]};obstacle.cosine=std::cos(b.yaw);obstacle.sine=std::sin(b.yaw);obstacle.half_width=d.half_width+building_clearance;obstacle.half_depth=d.half_depth+building_clearance;nav.obstacles.push_back(obstacle);}
                                for(size_t cell=0;cell<nav.walkable.size();++cell)if(nav.walkable[cell]&&!nav.is_safe(nav.center(int(cell))))nav.walkable[cell]=0;
                            }
                        }));for(auto& worker:workers)worker.get();
                    }
                    result.cancelled=cancellation->load()!=epoch;result.navigation_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-built).count();return result;
                });
            }
            // Coalesce repeated updates to each batch; upload at most one changed
            // batch per frame using existing fence-safe dynamic buffers.
            const auto upload_budget_start=std::chrono::steady_clock::now();
            while(!jobs->uploads.empty()&&std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-upload_budget_start).count()<2.){
                const auto started=std::chrono::steady_clock::now();
                auto first=jobs->uploads.begin();std::vector<EditorRenderBatch> batch;std::vector<prepared_editor_mesh> prepared;
                batch.push_back(std::move(first->second.batch));prepared.push_back(std::move(first->second.prepared));jobs->uploads.erase(first);publish(batch,true,&prepared);
                if(std::getenv("GB_EDITOR_PROFILE"))std::cout<<"Editor upload_ms="<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()<<std::endl;
            }
            if(lod_enabled){
                TerrainLodVersion version;version.load_generation=editor_state->load_generation;version.terrain_revision=editor_state->terrain_revision;version.settings_revision=lod_settings->revision;
                if(!terrain_uploads_pending()&&lod_renderer->publish_ready(jobs->visual_version)&&std::getenv("GB_EDITOR_PROFILE"))
                    std::cout<<(jobs->visual_is_refinement?"Editor detail_latency_ms=":"Editor visual_latency_ms=")<<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-jobs->visual_requested).count()<<" revision="<<jobs->visual_version.terrain_revision<<std::endl;
                TerrainLodView view;const auto matrix=camera->get_mat_p()*camera->get_mat_v();for(int c=0;c<4;++c)for(int r=0;r<4;++r)view.view_projection[c*4+r]=matrix[c][r];const auto position=camera->get_position();view.camera={{position.x,position.y,position.z}};view.projection_scale=std::abs(camera->get_mat_p()[1][1])*camera->get_viewport().w*.5f;
                view.near_error_px=lod_settings->nearErrorPx;view.normal_error_px=lod_settings->normalErrorPx;view.far_error_px=lod_settings->farErrorPx;lod_renderer->update_main(view,lod_settings->wireframe,lod_settings->debugMode);render_debug->state()->terrain_diagnostics=lod_renderer->diagnostics();
                editor_ui->set_terrain_triangle_count(lod_renderer->selected_triangles());
                if(std::getenv("GB_TERRAIN_LOD_DEBUG"))std::cout<<"Terrain CLOD "<<lod_renderer->diagnostics()<<std::endl;
            }
            if(!jobs->navigation.empty()&&jobs->navigation_revision==jobs->terrain_revision&&jobs->uploads.empty()&&!jobs->worker.valid()&&jobs->pending.empty()&&!editor_state->stroke_active()&&editor_resident_system){
                const auto started=std::chrono::steady_clock::now();editor_resident_system->apply_navigation(std::move(jobs->navigation));jobs->navigation.clear();
                editor_resident_system->apply_stairs(editor_state->level.stairs,editor_state->level.terrain.config);
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
            const auto profile_ui_begin=std::chrono::steady_clock::now();
            editor_ui->update();
            if(std::getenv("GB_PROFILE_SYSTEMS")){
                static unsigned count=0;static double input_ms=0,editor_ms=0,ui_ms=0;
                const auto now=std::chrono::steady_clock::now();
                input_ms+=std::chrono::duration<double,std::milli>(profile_input_end-profile_begin).count();
                editor_ms+=std::chrono::duration<double,std::milli>(profile_ui_begin-profile_input_end).count();
                ui_ms+=std::chrono::duration<double,std::milli>(now-profile_ui_begin).count();
                if(++count==120){std::cout<<"PROFILE editor input_ms="<<input_ms/count<<" work_ms="<<editor_ms/count<<" ui_ms="<<ui_ms/count<<std::endl;count=0;input_ms=editor_ms=ui_ms=0;}
            }
        });add_child(editor_update);
        std::cout << "Island seed=" << config.seed << " layers=" << config.layers_count
                  << " vertices=" << terrain_mesh.vertices.size() << " triangles=" << terrain_mesh.indices.size()/3
                  << " generation_ms=" << std::chrono::duration<double,std::milli>(generated-start).count()
                  << " mesh_ms=" << std::chrono::duration<double,std::milli>(built-generated).count() << std::endl;
        std::cout<<"Ocean patches="<<water_patches.size()<<" terraces="<<config.layers_count<<"+"<<config.underwater_layers_count<<std::endl;
    }
}
