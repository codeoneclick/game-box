//
//  batch.h
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef batch_h
#define batch_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class batch
    {
    public:
        
        enum e_batch_data_state
        {
            e_batch_data_state_waiting = 0,
            e_batch_data_state_generating,
            e_batch_data_state_generated
        };
        
        enum e_batch_render_state
        {
            e_batch_render_state_waiting = 0,
            e_batch_render_state_done,
            e_batch_render_state_not_exist
        };
        
    private:
        
    protected:
    
        mesh_shared_ptr m_mesh;
        std::unordered_map<std::string, std::pair<mesh_shared_ptr, glm::mat4>> m_data;
        
        i32 m_num_vertices;
        i32 m_num_indices;
        
        e_batch_data_state m_data_state;
        std::unordered_map<std::string, e_batch_render_state> m_render_states;
        
    public:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        
        batch();
        ~batch();
        
        mesh_shared_ptr get_mesh() const;
        
        e_batch_data_state get_data_state() const;
        
        e_batch_render_state get_render_state(const std::string& guid) const;
        void set_render_state(const std::string& guid, e_batch_render_state state);
        
        void add_data(const std::string& guid, const mesh_shared_ptr& mesh, const glm::mat4& mat_m);
        
        void cleanup();
        void generate();
        
    };
};


#endif
