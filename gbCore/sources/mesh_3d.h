//
//  mesh_3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "resource.h"
#include "mesh_2d.h"

namespace gb
{
    extern const ui32 k_max_bones;
    
    struct mesh_3d_bone_data
    {
        i32	m_id;
        f32 m_weigth;
    };
    
    struct mesh_3d_vertex_data
    {
        glm::vec3 m_position;
        glm::vec2 m_texcoord;
        glm::vec3 m_normal;
        glm::vec3 m_tangent;
        std::vector<mesh_3d_bone_data> m_bones;
    };
    
    class mesh_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        mesh_3d_vertex_data* m_vertices;
        ui16* m_indices;
        
        ui32 m_num_vertices;
        ui32 m_num_indices;
        
        glm::vec3 m_min_bound;
        glm::vec3 m_max_bound;
        
    public:
        
        mesh_3d_transfering_data(mesh_3d_vertex_data* vertices, ui16* indices,
                                 ui32 num_vertices, ui32 num_indices,
                                 const glm::vec3& min_bound, const glm::vec3& max_bound);
        
        ~mesh_3d_transfering_data();
        
        const mesh_3d_vertex_data* get_vertices() const;
        const ui16* get_indices() const;
        
        ui32 get_num_vertices() const;
        ui32 get_num_indices() const;
        
        const glm::vec3 get_min_bound() const;
        const glm::vec3 get_max_bound() const;
    };
    
    class skeleton_3d_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        ui32 m_num_bones;
        std::vector<bone_3d_data_shared_ptr> m_bones_raw_data;
        
    public:
        
        skeleton_3d_transfering_data(ui32 num_bones);
        ~skeleton_3d_transfering_data();
        
        void add_bone(i32 id, i32 parent_id);
        ui32 get_num_bones() const;
        const std::vector<bone_3d_data_shared_ptr>& get_bones_raw_data() const;
    };
    
    
    class mesh_3d : public mesh_2d
    {
    private:
        
    protected:
        
        mesh_3d_transfering_data_shared_ptr m_mesh_data;
        skeleton_3d_transfering_data_shared_ptr m_skeleton_data;
        sequence_3d_transfering_data_shared_ptr m_bindpose_data;
        
        glm::vec3 m_max_bound;
        glm::vec3 m_min_bound;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        mesh_3d(const std::string& guid);
        
        static mesh_3d_shared_ptr construct(const std::string& guid,
                                            const vbo_shared_ptr& vbo,
                                            const ibo_shared_ptr& ibo,
                                            const glm::vec3& max_bound,
                                            const glm::vec3& min_bound);
        
        static mesh_3d_shared_ptr construct(const std::string& guid,
                                            const vbo_shared_ptr& vbo,
                                            const ibo_shared_ptr& ibo);
        ~mesh_3d();
        
        vbo_shared_ptr get_vbo() const;
        ibo_shared_ptr get_ibo() const;
        
        const glm::vec3 get_min_bound() const;
        const glm::vec3 get_max_bound() const;
        
        const mesh_3d_vertex_data* get_raw_vertices() const;
        const ui16* get_raw_indices() const;
        
        ui32 get_num_raw_vertices() const;
        ui32 get_num_raw_indices() const;
        
        const skeleton_3d_transfering_data_shared_ptr get_skeleton_data() const;
        const sequence_3d_transfering_data_shared_ptr get_bindpose_data() const;
        
        void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        void draw() const;
        void draw(ui32 indices) const;
        void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
    };
};

