//
//  heightmap_quad_tree.hpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "vbo.h"

namespace gb
{
    class quad_tree : public std::enable_shared_from_this<quad_tree>
    {
    private:
        
        std::weak_ptr<quad_tree> m_parent;
        std::vector<std::shared_ptr<quad_tree>> m_childs;
        
        glm::vec3 m_max_bound;
        glm::vec3 m_min_bound;
        
        std::vector<ui16> m_indexes;
        std::vector<ui16> m_indexes_ids;
        ui32 m_num_indexes;
        
        vbo::vertex_attribute_PTNTCE* m_vertexes;
        ibo_shared_ptr m_ibo;
        
        bool m_is_generated;
        
    protected:
        
        void create_quad_tree_node(i32 size, i32 depth,
                                   const std::shared_ptr<quad_tree>& root);
        
        void create_index_buffer_for_quad_tree_node(const std::shared_ptr<quad_tree>& node);
        
        void generate_quad_tree_node(const frustum_3d_shared_ptr& frustum,
                                     const glm::mat4& matrix_m,
                                     const std::shared_ptr<quad_tree>& root,
                                     ui16* indexes,
                                     ui32& num_indexes);
        
    public:
        
        quad_tree();
        ~quad_tree();
        
        /*void generate(CSharedVertexBuffer vertexBuffer,
                      CSharedIndexBuffer indexBuffer,
                      const glm::vec3& maxBound,
                      const glm::vec3& minBound,
                      f32 depth,
                      ui32 size);
        
        ui32 update(CSharedFrustumRef frustum, const glm::mat4& matrixM);
        bool getIsGenerated(void) const;*/
    };
}
