#include "editor_hand_cursor.h"
#include "terrain_editor_input.h"
#include "hand_cursor_math.h"
#include "hand_surface_query.h"
#include "resident_shader_uniforms.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "camera_3d.h"
#include "window_impl.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "bone_3d.h"
#include "ibo.h"
#include "ces_geometry_3d_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_material_component.h"
#include "ces_transformation_3d_component.h"
#include "material.h"
#include "shader.h"
#include <limits>
#if defined(__OSX__)
#include <objc/message.h>
#include <objc/runtime.h>
#endif
namespace game {
struct EditorHandCursor::implementation {
    gb::shape_3d_shared_ptr hand;
    gb::camera_3d_shared_ptr camera;
    gb::window_impl_shared_ptr window;
    std::function<float(float,float)> water_height;
    std::shared_ptr<TerrainEditorInput> input;
    std::shared_ptr<resident_bone_uniforms> bones;
    std::shared_ptr<resident_color_uniforms> color;
    std::vector<std::pair<glm::vec3,int>> corners;
    struct Surface {
        std::weak_ptr<gb::mesh_3d> owner;
        unsigned vb_version=0,ib_version=0;
        std::vector<glm::vec3> bind,delta,pose;
        std::vector<glm::vec4> wind;
        std::vector<glm::u8vec4> ids,weights;
        std::vector<unsigned short> indices;
        glm::vec3 low,high;
        HandSurfaceMesh tree;
        bool ready=false;
    };
    std::map<const gb::mesh_3d*,Surface> surfaces;
    unsigned frame=0;
    HandCursorMotion motion;
    bool native_hidden=false;
    bool last_visible=false;
    int palm_bone=-1;
    ui32 triangles=0;
    float report_time=0;
    void native_cursor(bool hide){
        if(native_hidden==hide)return;
#if defined(__WINOS__)
        // Balance exactly our own contribution to the thread's cursor counter.
        ShowCursor(hide?FALSE:TRUE);
#elif defined(__OSX__)
        reinterpret_cast<void(*)(id,SEL)>(objc_msgSend)(reinterpret_cast<id>(objc_getClass("NSCursor")),sel_registerName(hide?"hide":"unhide"));
#endif
        native_hidden=hide;
    }
    ~implementation(){native_cursor(false);}
};
EditorHandCursor::EditorHandCursor(const gb::scene_fabricator_shared_ptr& fabricator,const gb::camera_3d_shared_ptr& camera,const gb::window_impl_shared_ptr& window,const std::shared_ptr<TerrainEditorInput>& input,std::function<float(float,float)> water_height):impl_(std::make_shared<implementation>()){
    auto& p=*impl_;p.camera=camera;p.window=window;
    p.input=input;
    add_required_component_guid(mask_,gb::ces_geometry_3d_component::class_guid());
    add_required_component_guid(mask_,gb::ces_transformation_3d_component::class_guid());
    add_required_component_guid(mask_,gb::ces_material_component::class_guid());
    add_required_components_mask(mask_);
    // All feed callbacks (including resident skinning/grounding) finish before
    // this end phase. The render system's end phase follows at order 7.
    set_order(6);
    p.water_height=std::move(water_height);
    p.hand=fabricator->create_shape_3d("editor_hand_cursor.xml");p.hand->tag="editor.hand_cursor";
    p.hand->visible=false;p.hand->visible_in_next_frame=false;
    auto geometry=p.hand->get_component<gb::ces_geometry_3d_component>();
    geometry->bounding_mode=gb::ces_geometry_component::e_none;
    auto mesh=std::static_pointer_cast<gb::mesh_3d>(geometry->get_mesh());
    for(const auto& bone:mesh->get_skeleton_data()->get_bones_raw_data())if(bone->get_bone_name()=="palm")p.palm_bone=bone->get_bone_id();
    if(p.palm_bone<0)throw std::runtime_error("Hand cursor palm anchor missing");
    p.triangles=mesh->get_ibo()->get_used_size()/3;
    const auto vbo=mesh->get_vbo();const auto vertices=vbo->lock<gb::vbo::vertex_attribute_PT4B>();
    // Authored cuboids are rigidly bound to individual finger joints.
    for(ui32 i=0;i<vbo->get_used_size();++i){
        const auto& v=vertices[i];
        if(v.m_bone_weights[0]!=255)throw std::runtime_error("Hand cursor requires rigid Blockbench joints");
        p.corners.emplace_back(v.m_position,v.m_bone_ids[0]);
    }
    auto uniforms=p.hand->add_component<gb::ces_shader_uniforms_component>();
    p.bones=uniforms->construct_uniforms_buffer<resident_bone_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2);
    p.color=uniforms->construct_uniforms_buffer<resident_color_uniforms>(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,"resident.color",0);
    p.color->set_outline_id(9000000.f);
    p.hand->play_animation("idle",true);
    std::cout<<"Hand cursor initialized: world_length_m=2 clearance_m=0.5 bones="<<p.hand->get_component<gb::ces_animation_3d_mixer_component>()->get_transformation_size()<<std::endl;
}
gb::ces_entity_shared_ptr EditorHandCursor::root()const{return impl_->hand;}
void EditorHandCursor::on_feed_end(f32 dt){update(*impl_->input,dt);}
bool EditorHandCursor::surface_height(float x,float z,float& height,std::string& hit_tag,size_t& tests,const glm::vec2* path_end){
    auto& p=*impl_;bool hit=false;
    enumerate_entities_with_components(mask_,[&](const gb::ces_entity_shared_ptr& entity){
        if(entity==p.hand||!entity->get_is_visible())return;
        const std::string tag=entity->tag;
        if(tag.find("editor.")==0||tag.find("island.grass")==0||tag.find("1_")==0)return;
        auto material=entity->get_component<gb::ces_material_component>()->get_material("ws.base");
        if(!material||!material->is_depth_test()||!material->get_shader())return;
        const auto shader=material->get_shader()->get_guid();
        if(shader.find("brush")!=std::string::npos||shader.find("aura")!=std::string::npos||shader.find("sky")!=std::string::npos)return;
        for(gb::ces_entity_shared_ptr parent=entity->parent;parent;parent=parent->parent)if(!parent->get_is_visible())return;
        auto base_mesh=entity->get_component<gb::ces_geometry_3d_component>()->get_mesh();
        if(!base_mesh||!base_mesh->is_3d())return;
        auto mesh=base_mesh->as_3d();auto vb=mesh->get_vbo();auto ib=mesh->get_ibo();
        if(!vb||!ib||vb->get_used_size()==0||ib->get_used_size()<3)return;
        const auto mixer=entity->get_component<gb::ces_animation_3d_mixer_component>();
        const auto skin_uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
        auto skin_buffer=skin_uniforms?skin_uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2):nullptr;
        const bool skin=(mixer&&mixer->get_transformation_size()>0)||(skin_buffer&&mesh->get_skeleton_data());
        auto& s=p.surfaces[mesh.get()];
        if(s.owner.lock()!=mesh||s.vb_version!=vb->get_version()||s.ib_version!=ib->get_version()||s.bind.empty()){
            s=implementation::Surface();s.owner=mesh;s.vb_version=vb->get_version();s.ib_version=ib->get_version();
            const size_t count=vb->get_used_size();s.bind.resize(count);s.delta.resize(count,glm::vec3(0));s.wind.resize(count,glm::vec4(0));
            s.low=glm::vec3(std::numeric_limits<float>::max());s.high=-s.low;
            const auto vertices=vb->lock<gb::vbo::vertex_attribute_P>();
            const auto rigid=vb->lock<gb::vbo::vertex_attribute_PTNTC>();
            const auto skinned=vb->lock<gb::vbo::vertex_attribute_PT4B>();
            if(skin){s.ids.resize(count);s.weights.resize(count);s.pose.resize(count);}
            const bool terrain=shader.find("shader_island_terrain")!=std::string::npos;
            for(size_t i=0;i<count;i++){
                s.bind[i]=vertices[i].m_position;
                if(terrain&&glm::unpackSnorm4x8(rigid[i].m_tangent).w<-.5f){const auto d=glm::unpackHalf2x16(rigid[i].m_texcoord);s.delta[i]=glm::vec3(d.x,0,d.y);}
                if(terrain){const auto bend=glm::unpackSnorm4x8(rigid[i].m_tangent);if(bend.w>.5f){const auto root=glm::unpackHalf2x16(rigid[i].m_texcoord);s.wind[i]=glm::vec4(root,bend.x*bend.x,bend.y);}}
                if(skin){s.ids[i]=skinned[i].m_bone_ids;s.weights[i]=skinned[i].m_bone_weights;}
                s.low=glm::min(s.low,glm::min(s.bind[i],s.bind[i]+s.delta[i]));s.high=glm::max(s.high,glm::max(s.bind[i],s.bind[i]+s.delta[i]));
                const glm::vec3 margin(.301f*s.wind[i].z,0,.301f*s.wind[i].z);s.low=glm::min(s.low,s.bind[i]-margin);s.high=glm::max(s.high,s.bind[i]+margin);
            }
            const auto indices=ib->lock();s.indices.assign(indices,indices+ib->get_used_size());
        }
        const auto model=entity->get_component<gb::ces_transformation_3d_component>()->get_absolute_transformation();
        const glm::mat4* bones=nullptr;int bone_count=0;
        const auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
        if(skin){
            bone_count=mixer&&mixer->get_transformation_size()>0?mixer->get_transformation_size():static_cast<int>(mesh->get_skeleton_data()->get_bones_raw_data().size());if(bone_count>64)return;
            auto buffer=skin_buffer;
            bones=buffer?static_cast<const glm::mat4*>(buffer->get_values()):mixer->get_transformations();
        }
        const auto center=(s.low+s.high)*.5f,half=(s.high-s.low)*.5f;
        glm::vec3 low(std::numeric_limits<float>::max()),high=-low;
        const auto extend=[&](const glm::mat4& transform){
            const auto c=glm::vec3(transform*glm::vec4(center,1));
            const auto e=glm::abs(glm::vec3(transform[0]))*half.x+glm::abs(glm::vec3(transform[1]))*half.y+glm::abs(glm::vec3(transform[2]))*half.z;
            low=glm::min(low,c-e);high=glm::max(high,c+e);
        };
        if(skin)for(int b=0;b<bone_count;b++)extend(model*bones[b]);else extend(model);
        const float end_x=path_end?path_end->x:x,end_z=path_end?path_end->y:z;
        if(std::max(x,end_x)<low.x-.0001f||std::min(x,end_x)>high.x+.0001f||std::max(z,end_z)<low.z-.0001f||std::min(z,end_z)>high.z+.0001f||high.y<height)return;
        if(!s.ready){s.tree.reset(s.bind,s.indices,s.delta,s.wind);s.ready=true;}
        if(skin){
            for(size_t i=0;i<s.bind.size();i++){
                glm::vec4 vertex(0);float total=0;
                for(int j=0;j<4;j++){const float weight=s.weights[i][j];const int id=s.ids[i][j];if(weight>0&&id<bone_count){vertex+=bones[id]*glm::vec4(s.bind[i],1)*weight;total+=weight;}}
                s.pose[i]=total>0?glm::vec3(vertex)/total:s.bind[i];
            }s.tree.refit(s.pose);
        }
        float morph=0,time=0;
        if(uniforms)if(auto ocean=uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"ocean",2)){
            const auto clock=ocean->get_uniforms_ref().find("time_level");if(clock!=ocean->get_uniforms_ref().end())time=clock->second->get_vec4().x;
            const auto found=ocean->get_uniforms_ref().find("terrain_lod");if(found!=ocean->get_uniforms_ref().end()){const auto value=found->second->get_vec4();if(value.y>.5f)morph=value.x;}
        }
        float y=0;const bool found=path_end?s.tree.path_height(glm::vec2(x,z),*path_end,model,morph,y,&tests,time):s.tree.height(x,z,model,morph,y,&tests,time);if(found&&y>=height){height=y;hit_tag=tag.empty()?shader:tag;hit=true;}
    });
    return hit;
}
void EditorHandCursor::update(const TerrainEditorInput& input,float dt){
    auto& p=*impl_;glm::vec3 surface;
    const auto test_mode=std::getenv("GB_HAND_CURSOR_TEST");
    const bool synthetic=test_mode&&std::string(test_mode)=="1";
    bool visible=input.cursor_surface(surface);
    if(++p.frame%60==0)for(auto i=p.surfaces.begin();i!=p.surfaces.end();)if(i->second.owner.expired())i=p.surfaces.erase(i);else ++i;
#if defined(__WINOS__)
    POINT mouse{};GetCursorPos(&mouse);
    if(!synthetic)visible=visible&&GetForegroundWindow()==p.window->get_hwnd()&&WindowFromPoint(mouse)==p.window->get_hwnd();
#elif defined(__OSX__)
    struct cursor_point{double x,y;};
    const auto send_id=reinterpret_cast<id(*)(id,SEL)>(objc_msgSend);
    const auto send_bool=reinterpret_cast<bool(*)(id,SEL)>(objc_msgSend);
    const auto native_view=reinterpret_cast<id>(const_cast<void*>(p.window->get_hwnd()));
    const auto native_window=send_id(native_view,sel_registerName("window"));
    const auto application=send_id(reinterpret_cast<id>(objc_getClass("NSApplication")),sel_registerName("sharedApplication"));
    const auto mouse=reinterpret_cast<cursor_point(*)(id,SEL)>(objc_msgSend)(native_window,sel_registerName("mouseLocationOutsideOfEventStream"));
    const auto local=reinterpret_cast<cursor_point(*)(id,SEL,cursor_point,id)>(objc_msgSend)(native_view,sel_registerName("convertPoint:fromView:"),mouse,nullptr);
    const auto viewport=p.camera->get_viewport();
    visible=visible&&send_bool(application,sel_registerName("isActive"))&&send_bool(native_window,sel_registerName("isKeyWindow"))&&local.x>=0&&local.y>=0&&local.x<viewport.z&&local.y<viewport.w;
#endif
    auto mixer=p.hand->get_component<gb::ces_animation_3d_mixer_component>();
    const int count=mixer->get_transformation_size();visible=visible&&count>0&&count<=64;
    if(visible){
        // Dorsal view of an open hand, fingers away/up, wrist toward the camera.
        const auto view=p.camera->get_mat_v();
        const auto inverse_view=glm::inverse(view);
        const auto facing=hand_cursor_facing(glm::vec3(inverse_view[2]),glm::vec3(inverse_view[0]));
        glm::mat4 palette[64];
        for(int i=0;i<count;++i)palette[i]=facing*mixer->get_transformations()[i];
        const auto palm_origin=glm::vec3(palette[p.palm_bone]*glm::vec4(hand_cursor_palm_bind_point(),1));
        const auto neutral_palm=glm::vec3(facing*glm::vec4(hand_cursor_palm_bind_point(),0));
        // Preserve the small horizontal stroke; trace vertically below the
        // actual animated palm. Vertical breathing is compensated to keep 0.5m.
        surface.x+=palm_origin.x-neutral_palm.x;surface.z+=palm_origin.z-neutral_palm.z;
        float height=p.water_height?p.water_height(surface.x,surface.z):surface.y;
        std::string hit_tag="ocean";size_t tests=0;
        surface_height(surface.x,surface.z,height,hit_tag,tests);
        surface.y=height;
        if(!p.last_visible)p.motion.reset(surface+glm::vec3(0,.5f,0));
        const auto ground=[&](float x,float z){float h=p.water_height?p.water_height(x,z):0.f;std::string tag;surface_height(x,z,h,tag,tests);return h;};
        const auto corridor=[&](glm::vec2 from,glm::vec2 to){float h=std::max(p.water_height?p.water_height(from.x,from.y):0.f,p.water_height?p.water_height(to.x,to.y):0.f);std::string tag;surface_height(from.x,from.y,h,tag,tests,&to);return h;};
        const glm::vec3 anchor=p.motion.update(glm::vec2(surface.x,surface.z),dt,ground,corridor);
        surface=glm::vec3(anchor.x,ground(anchor.x,anchor.z),anchor.z);
        {
            for(int i=0;i<count;++i)palette[i][3]-=glm::vec4(palm_origin,0);
            p.hand->position=anchor;p.hand->scale=glm::vec3(1);
            p.bones->update(palette,count);
            p.hand->get_component<gb::ces_material_component>()->get_material("ws.base")->set_custom_shader_uniform(static_cast<glm::mat4*>(p.bones->get_values()),64,"u_bones");
            p.color->update(view,glm::vec4(1),p.camera->get_position());
            p.report_time+=std::max(0.f,dt);
            if(std::getenv("GB_HAND_CURSOR_TEST")&&p.report_time>.15f){
                glm::vec2 screen_low(1.e9f),screen_high(-1.e9f);
                const auto vp=p.camera->get_viewport();
                for(const auto& v:p.corners){auto q=p.camera->get_mat_p()*view*glm::vec4(anchor+glm::vec3(palette[v.second]*glm::vec4(v.first,1)),1);q/=q.w;const glm::vec2 pixel((q.x+1)*vp.z*.5f,(1-q.y)*vp.w*.5f);screen_low=glm::min(screen_low,pixel);screen_high=glm::max(screen_high,pixel);}
                const auto palm=anchor+glm::vec3(palette[p.palm_bone]*glm::vec4(hand_cursor_palm_bind_point(),1));
                std::cout<<"Hand cursor visible surface="<<surface.x<<","<<surface.y<<","<<surface.z<<" clearance="<<palm.y-surface.y<<" scale=1 idle_frame="<<mixer->get_current_animation_frame(0)<<" triangles="<<p.triangles<<" pixels="<<screen_low.x<<","<<screen_low.y<<".."<<screen_high.x<<","<<screen_high.y<<" hit="<<hit_tag<<" tests="<<tests<<std::endl;p.report_time=0;}
        }
    }
    // Integration probes exercise live ECS meshes, transforms and shader poses.
    // They run only in the synthetic smoke process, never during normal play.
    if(synthetic&&p.frame==120){
        std::set<std::string> checked;
        enumerate_entities_with_components(mask_,[&](const gb::ces_entity_shared_ptr& entity){
            const std::string tag=entity->tag;std::string kind;
            if(tag.find("island.terrain.lod.main.")==0||tag.find("0_")==0)kind="terrain";
            else if(tag.find("island.building.")==0)kind="building";
            else if(tag.find("island.trees.")==0||tag.find("2_")==0)kind="tree";
            else if(tag.find("island.resident")==0)kind="resident";
            if(kind.empty()||checked.count(kind)||!entity->get_is_visible())return;
            const auto mesh=entity->get_component<gb::ces_geometry_3d_component>()->get_mesh();
            if(!mesh||!mesh->is_3d())return;const auto found=p.surfaces.find(mesh->as_3d().get());if(found==p.surfaces.end())return;
            const auto& cached=found->second;const auto model=entity->get_component<gb::ces_transformation_3d_component>()->get_absolute_transformation();
            const auto uniforms=entity->get_component<gb::ces_shader_uniforms_component>();
            const auto bone_buffer=uniforms?uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"resident.bones",2):nullptr;
            const auto bones=bone_buffer?static_cast<const glm::mat4*>(bone_buffer->get_values()):nullptr;
            float time=0,morph=0;
            if(uniforms)if(auto ocean=uniforms->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_vertex,"ocean",2)){
                const auto clock=ocean->get_uniforms_ref().find("time_level");if(clock!=ocean->get_uniforms_ref().end())time=clock->second->get_vec4().x;
                const auto lod=ocean->get_uniforms_ref().find("terrain_lod");if(lod!=ocean->get_uniforms_ref().end()&&lod->second->get_vec4().y>.5f)morph=lod->second->get_vec4().x;
            }
            auto vertex=[&](unsigned short i){
                auto v=cached.bind[i]+cached.delta[i]*morph;
                if(bones&&!cached.ids.empty()){glm::vec4 posed(0);float total=0;for(int b=0;b<4;b++){const float w=cached.weights[i][b];if(w>0){posed+=bones[cached.ids[i][b]]*glm::vec4(v,1)*w;total+=w;}}if(total>0)v=glm::vec3(posed)/total;}
                if(cached.wind[i].z>0){const auto w=hand_tree_wind(glm::vec2(cached.wind[i]),time,cached.wind[i].w)*cached.wind[i].z;v.x+=w.x;v.z+=w.y;}
                return glm::vec3(model*glm::vec4(v,1));
            };
            glm::vec3 sample(0);float top=-1.e9f;
            for(size_t i=0;i+2<cached.indices.size();i+=3){const auto a=vertex(cached.indices[i]),b=vertex(cached.indices[i+1]),c=vertex(cached.indices[i+2]);const auto center=(a+b+c)/3.f;if(std::abs(glm::cross(b-a,c-a).y)>1.e-5f&&center.y>top){top=center.y;sample=center;}}
            if(top<-1.e8f)return;float height=p.water_height?p.water_height(sample.x,sample.z):-1.e9f;std::string hit;size_t tests=0;
            const bool valid=surface_height(sample.x,sample.z,height,hit,tests)&&height>=top-.001f;
            std::cout<<"Hand surface probe kind="<<kind<<" valid="<<valid<<" expected="<<top<<" actual="<<height<<" target="<<tag<<" hit="<<hit<<std::endl;checked.insert(kind);
        });
    }
    p.hand->visible=visible;p.hand->visible_in_next_frame=visible;
    if(std::getenv("GB_HAND_CURSOR_TEST")&&visible!=p.last_visible)std::cout<<"Hand cursor visibility="<<visible<<std::endl;
    p.last_visible=visible;
    p.native_cursor(visible&&!synthetic);
}
}
