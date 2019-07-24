//
//  mtl_vertex_descriptor.cpp
//  gbCore
//
//  Created by serhii.s on 3/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_vertex_descriptor.h"
#include "mtl_device.h"
#include "vbo.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_vertex_descriptor_impl : public i_mtl_vertex_descriptor_impl
    {
    private:
        
    protected:
        
        MTLVertexDescriptor* m_vertex_descriptor = nil;
        
    public:
        
        mtl_vertex_descriptor_impl();
        ~mtl_vertex_descriptor_impl();
        
        void* get_mtl_vertex_descriptor_ptr() const override;
    };
    
    mtl_vertex_descriptor_impl::mtl_vertex_descriptor_impl()
    {
        m_vertex_descriptor = [[MTLVertexDescriptor alloc] init];
    }
    
    mtl_vertex_descriptor_impl::~mtl_vertex_descriptor_impl()
    {
        
    }
    
    void* mtl_vertex_descriptor_impl::get_mtl_vertex_descriptor_ptr() const
    {
        return (__bridge void*)m_vertex_descriptor;
    }
    
    class mtl_vertex_descriptor_impl_PTNTC : public mtl_vertex_descriptor_impl
    {
    private:
        
    protected:
        
    public:
        
        mtl_vertex_descriptor_impl_PTNTC();
        ~mtl_vertex_descriptor_impl_PTNTC() = default;
    };
    
    mtl_vertex_descriptor_impl_PTNTC::mtl_vertex_descriptor_impl_PTNTC()
    {
        m_vertex_descriptor.attributes[0].format = MTLVertexFormatFloat3;
        m_vertex_descriptor.attributes[0].bufferIndex = 0;
        m_vertex_descriptor.attributes[0].offset = offsetof(vbo::vertex_attribute_PTNTC, m_position);
        
        m_vertex_descriptor.attributes[1].format = MTLVertexFormatHalf2;
        m_vertex_descriptor.attributes[1].bufferIndex = 0;
        m_vertex_descriptor.attributes[1].offset = offsetof(vbo::vertex_attribute_PTNTC, m_texcoord);
        
        m_vertex_descriptor.attributes[2].format = MTLVertexFormatChar4Normalized;
        m_vertex_descriptor.attributes[2].bufferIndex = 0;
        m_vertex_descriptor.attributes[2].offset = offsetof(vbo::vertex_attribute_PTNTC, m_normal);
        
        m_vertex_descriptor.attributes[3].format = MTLVertexFormatChar4Normalized;
        m_vertex_descriptor.attributes[3].bufferIndex = 0;
        m_vertex_descriptor.attributes[3].offset = offsetof(vbo::vertex_attribute_PTNTC, m_tangent);
        
        m_vertex_descriptor.attributes[4].format = MTLVertexFormatUChar4Normalized;
        m_vertex_descriptor.attributes[4].bufferIndex = 0;
        m_vertex_descriptor.attributes[4].offset = offsetof(vbo::vertex_attribute_PTNTC, m_color);;
        
        m_vertex_descriptor.layouts[0].stride = sizeof(vbo::vertex_attribute_PTNTC);
        m_vertex_descriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    }
    
    class mtl_vertex_descriptor_impl_PTC : public mtl_vertex_descriptor_impl
    {
    private:
        
    protected:
        
    public:
        
        mtl_vertex_descriptor_impl_PTC();
        ~mtl_vertex_descriptor_impl_PTC() = default;
    };
    
    mtl_vertex_descriptor_impl_PTC::mtl_vertex_descriptor_impl_PTC()
    {
        m_vertex_descriptor.attributes[0].format = MTLVertexFormatFloat3;
        m_vertex_descriptor.attributes[0].bufferIndex = 0;
        m_vertex_descriptor.attributes[0].offset = offsetof(vbo::vertex_attribute_PTC, m_position);
        
        m_vertex_descriptor.attributes[1].format = MTLVertexFormatFloat2;
        m_vertex_descriptor.attributes[1].bufferIndex = 0;
        m_vertex_descriptor.attributes[1].offset = offsetof(vbo::vertex_attribute_PTC, m_texcoord);
        
        m_vertex_descriptor.attributes[4].format = MTLVertexFormatUChar4;
        m_vertex_descriptor.attributes[4].bufferIndex = 0;
        m_vertex_descriptor.attributes[4].offset = offsetof(vbo::vertex_attribute_PTC, m_color);;
        
        m_vertex_descriptor.layouts[0].stride = sizeof(vbo::vertex_attribute_PTC);
        m_vertex_descriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    }
    
    std::shared_ptr<mtl_vertex_descriptor> mtl_vertex_descriptor::m_PTNTC_vertex_descriptor = nullptr;
    std::shared_ptr<mtl_vertex_descriptor> mtl_vertex_descriptor::m_PTC_vertex_descriptor = nullptr;
    
    mtl_vertex_descriptor::mtl_vertex_descriptor()
    {
        
    }
    
    mtl_vertex_descriptor::~mtl_vertex_descriptor()
    {
        
    }
    
    std::shared_ptr<mtl_vertex_descriptor> mtl_vertex_descriptor::create_PTNTC_vertex_descriptor()
    {
        if (!m_PTNTC_vertex_descriptor)
        {
            m_PTNTC_vertex_descriptor = std::make_shared<mtl_vertex_descriptor>();
            m_PTNTC_vertex_descriptor->m_vertex_descriptor_impl = std::make_shared<mtl_vertex_descriptor_impl_PTNTC>();
        }
        return m_PTNTC_vertex_descriptor;
    }
    
    std::shared_ptr<mtl_vertex_descriptor> mtl_vertex_descriptor::create_PTC_vertex_descriptor()
    {
        if (!m_PTC_vertex_descriptor)
        {
            m_PTC_vertex_descriptor = std::make_shared<mtl_vertex_descriptor>();
            m_PTC_vertex_descriptor->m_vertex_descriptor_impl = std::make_shared<mtl_vertex_descriptor_impl_PTC>();
        }
        return m_PTC_vertex_descriptor;
    }

    void* mtl_vertex_descriptor::get_mtl_vertex_descriptor_ptr() const
    {
        return impl_as<mtl_vertex_descriptor_impl>()->get_mtl_vertex_descriptor_ptr();
    }
}

#endif
