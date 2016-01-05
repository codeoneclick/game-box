//
//  mesh.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_h
#define mesh_h

#include "resource.h"
#include "vbo.h"
#include "ibo.h"
#include "vao.h"
#include "bone.h"
#include "sequence.h"
#include "shader.h"

namespace gb
{
    class mesh_transfering_data : public resource_transfering_data
    {
    private:
        
    protected:
        
        vbo::vertex_data* m_vertices;
        ui16* m_indices;
        
        ui32 m_num_vertices;
        ui32 m_num_indices;
        
        glm::vec3 m_min_bound;
        glm::vec3 m_max_bound;
        
    public:
        
        mesh_transfering_data(vbo::vertex_data* vertices, ui16* indices,
                              ui32 num_vertices, ui32 num_indices,
                              const glm::vec3& min_bound, const glm::vec3& max_bound);
        ~mesh_transfering_data(void);
        
        const vbo::vertex_data* get_vertices(void) const;
        const ui16* get_indices(void) const;
        
        ui32 get_num_vertices(void) const;
        ui32 get_num_indices(void) const;
        
        const glm::vec3 get_min_bound(void) const;
        const glm::vec3 get_max_bound(void) const;
    };
    
    class skeleton_transfering_data : public resource_transfering_data
    {
    private:
        
    protected:
        
        ui32 m_num_bones;
        std::vector<std::shared_ptr<bone_data>> m_bones_data;
        
    public:
        
        skeleton_transfering_data(ui32 numBones);
        ~skeleton_transfering_data(void);
        
        void add_bone(i32 id, i32 parent_id);
        ui32 get_num_bones(void) const;
        const std::vector<std::shared_ptr<bone_data>> get_bones_data(void) const;
    };
    
    class mesh : public resource
    {
    private:
        
    protected:
        
        std::shared_ptr<mesh_transfering_data> m_mesh_data;
        std::shared_ptr<skeleton_transfering_data> m_skeleton_data;
        std::shared_ptr<sequence_transfering_data> m_bindpose_data;
        
        std::shared_ptr<vbo> m_vbo;
        std::shared_ptr<ibo> m_ibo;
        
        std::unordered_map<std::string, std::shared_ptr<vao>> m_vao_states;
        
        glm::vec3 m_max_bound;
        glm::vec3 m_min_bound;
        
        GLenum m_mode;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        mesh(const std::string& guid, GLenum mode = GL_TRIANGLES);
        
        static std::shared_ptr<mesh> construct(const std::string& guid,
                                               const std::shared_ptr<vbo>& vbo,
                                               const std::shared_ptr<ibo>& ibo,
                                               const glm::vec3& min_bound,
                                               const glm::vec3& max_bound,
                                               GLenum mode = GL_TRIANGLES);
        
        static std::shared_ptr<mesh> construct(const std::string& guid,
                                               const std::shared_ptr<vbo>& vbo,
                                               const std::shared_ptr<ibo>& ibo,
                                               GLenum mode = GL_TRIANGLES);
        ~mesh();
        
        std::shared_ptr<vbo> get_vbo() const;
        std::shared_ptr<ibo> get_ibo() const;
        
        const vbo::vertex_data* get_raw_vertices() const;
        const ui16* get_raw_indices() const;
        
        ui32 get_num_raw_vertices() const;
        ui32 get_num_raw_indices() const;
        
        const glm::vec3 get_min_bound() const;
        const glm::vec3 get_max_bound() const;
        const std::tuple<glm::vec3, glm::vec3> get_bounds(void) const;
        
        const glm::vec3 get_min_bound(const glm::mat4& matrix) const;
        const glm::vec3 get_max_bound(const glm::mat4& matrix) const;
        const std::tuple<glm::vec3, glm::vec3> get_bounds(const glm::mat4& matrix) const;
        
        const std::shared_ptr<skeleton_transfering_data> get_skeleton_data() const;
        const std::shared_ptr<sequence_transfering_data> get_bindpose_data() const;
        
        virtual void bind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
        virtual void draw() const;
        virtual void draw(ui32 indices) const;
        virtual void unbind(const std::string& attributes_guid, const std::array<i32, e_shader_attribute_max>& attributes);
    };
    
}

#endif
