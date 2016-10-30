//
//  batching_pipeline.hpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef batching_pipeline_h
#define batching_pipeline_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class batching_pipeline
    {
    public:
        
        static const ui32 k_max_num_batches_with_same_material;
        
    private:
        
    protected:
        
        std::map<std::string, batch_shared_ptr> m_batches;
        
    public:
        
        batching_pipeline();
        ~batching_pipeline();
        
        void batch(const material_shared_ptr& material, const mesh_shared_ptr& mesh, const glm::mat4& matrix, ui32 matrix_version);
        void draw();
    };
};



#endif
