//
//  mtl_depth_stencil_state.cpp
//  gbCore
//
//  Created by serhii.s on 3/5/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_depth_stencil_state.h"
#include "mtl_device.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_depth_stencil_state_impl : public i_mtl_depth_stencil_state_impl
    {
    private:
        
        id<MTLDepthStencilState> m_depth_stencil_state = nil;
        MTLDepthStencilDescriptor* m_depth_stencil_state_descriptor = nil;
        
        std::string m_guid;
        
        MTLStencilOperation convert_stencil_op_from_gl_to_mtl(ui32 value);
        MTLCompareFunction convert_compare_func_from_gl_to_mtl(ui32 value);
        
    protected:
        
    public:
        
        mtl_depth_stencil_state_impl(const std::shared_ptr<material_cached_parameters>& material_parameters);
        ~mtl_depth_stencil_state_impl();
        
        void* get_mtl_raw_depth_stencil_state_ptr() const override;
        
        std::string get_guid() const override;
    };
    
    mtl_depth_stencil_state_impl::mtl_depth_stencil_state_impl(const std::shared_ptr<material_cached_parameters>& material_parameters)
    {
        id<MTLDevice> mtl_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        
        m_depth_stencil_state_descriptor = [[MTLDepthStencilDescriptor alloc] init];
        m_depth_stencil_state_descriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
        m_depth_stencil_state_descriptor.depthWriteEnabled = material_parameters->get_is_depth_test();
        
        if (material_parameters->get_is_stencil_test())
        {
            MTLStencilDescriptor *back_stencil_descriptor = [[MTLStencilDescriptor alloc] init];
            MTLStencilDescriptor *front_stencil_descriptor = [[MTLStencilDescriptor alloc] init];
            
            back_stencil_descriptor.stencilCompareFunction = convert_compare_func_from_gl_to_mtl(material_parameters->get_stencil_function());
            front_stencil_descriptor.stencilCompareFunction = convert_compare_func_from_gl_to_mtl(material_parameters->get_stencil_function());
            
            back_stencil_descriptor.readMask = material_parameters->get_stencil_mask_read();
            back_stencil_descriptor.writeMask = material_parameters->get_stencil_mask_write();
            
            front_stencil_descriptor.readMask = material_parameters->get_stencil_mask_read();
            front_stencil_descriptor.writeMask = material_parameters->get_stencil_mask_write();
            
            back_stencil_descriptor.stencilFailureOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_back_stencil_op_1());
            back_stencil_descriptor.depthFailureOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_back_stencil_op_2());
            back_stencil_descriptor.depthStencilPassOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_back_stencil_op_3());
            
            front_stencil_descriptor.stencilFailureOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_front_stencil_op_1());
            front_stencil_descriptor.depthFailureOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_front_stencil_op_2());
            front_stencil_descriptor.depthStencilPassOperation = convert_stencil_op_from_gl_to_mtl(material_parameters->get_front_stencil_op_3());
            
            m_depth_stencil_state_descriptor.frontFaceStencil = front_stencil_descriptor;
            m_depth_stencil_state_descriptor.backFaceStencil = back_stencil_descriptor;
        }
        
        m_depth_stencil_state = [mtl_device newDepthStencilStateWithDescriptor:m_depth_stencil_state_descriptor];
        m_guid = material_parameters->get_guid();
    }
    
    mtl_depth_stencil_state_impl::~mtl_depth_stencil_state_impl()
    {
        
    }
    
    MTLStencilOperation mtl_depth_stencil_state_impl::convert_stencil_op_from_gl_to_mtl(ui32 value)
    {
        if (value == gl::constant::stencil_op_keep)
        {
            return MTLStencilOperationKeep;
        }
        if (value == gl::constant::stencil_op_zero)
        {
            return MTLStencilOperationZero;
        }
        if (value == gl::constant::stencil_op_replace)
        {
            return MTLStencilOperationReplace;
        }
        if (value == gl::constant::stencil_op_inc_clamp)
        {
            return MTLStencilOperationIncrementClamp;
        }
        if (value == gl::constant::stencil_op_dec_clamp)
        {
            return MTLStencilOperationDecrementClamp;
        }
        if (value == gl::constant::stencil_op_inv)
        {
            return MTLStencilOperationInvert;
        }
        if (value == gl::constant::stencil_op_inc_wrap)
        {
            return MTLStencilOperationIncrementWrap;
        }
        if (value == gl::constant::stencil_op_dec_wrap)
        {
            return MTLStencilOperationDecrementWrap;
        }
        return MTLStencilOperationKeep;
    }
    
    MTLCompareFunction mtl_depth_stencil_state_impl::convert_compare_func_from_gl_to_mtl(ui32 value)
    {
        if (value == gl::constant::always)
        {
             return MTLCompareFunctionAlways;
        }
        if (value == gl::constant::equal)
        {
            return MTLCompareFunctionEqual;
        }
        if (value == gl::constant::notequal)
        {
            return MTLCompareFunctionNotEqual;
        }
        if (value == gl::constant::less)
        {
            return MTLCompareFunctionLess;
        }
        if (value == gl::constant::less_equal)
        {
            return MTLCompareFunctionLessEqual;
        }
        
        return MTLCompareFunctionAlways;
    }
    
    void* mtl_depth_stencil_state_impl::get_mtl_raw_depth_stencil_state_ptr() const
    {
        return (__bridge void*)m_depth_stencil_state;
    }
    
    std::string mtl_depth_stencil_state_impl::get_guid() const
    {
        return m_guid;
    }
    
    std::unordered_map<std::string, std::shared_ptr<mtl_depth_stencil_state>> mtl_depth_stencil_state::m_depth_stencil_pipelines_pool;
    
    mtl_depth_stencil_state::mtl_depth_stencil_state(const std::shared_ptr<material_cached_parameters>& material_parameters)
    {
        m_mtl_depth_stencil_state_impl = std::make_shared<mtl_depth_stencil_state_impl>(material_parameters);
    }
    
    mtl_depth_stencil_state::~mtl_depth_stencil_state()
    {
        
    }
    
    std::shared_ptr<mtl_depth_stencil_state> mtl_depth_stencil_state::construct(const std::shared_ptr<material_cached_parameters>& material_parameters)
    {
        std::shared_ptr<mtl_depth_stencil_state> depth_stencil_state = nullptr;
        std::string material_parameters_guid = material_parameters->get_guid();
        const auto depth_stencil_state_it = m_depth_stencil_pipelines_pool.find(material_parameters_guid);
        if (depth_stencil_state_it != m_depth_stencil_pipelines_pool.end())
        {
            depth_stencil_state = depth_stencil_state_it->second;
        }
        else
        {
            depth_stencil_state = std::make_shared<mtl_depth_stencil_state>(material_parameters);
            m_depth_stencil_pipelines_pool.insert(std::make_pair(material_parameters_guid, depth_stencil_state));
        }
        return depth_stencil_state;
    }
    
    void* mtl_depth_stencil_state::get_mtl_raw_depth_stencil_state_ptr() const
    {
        return impl_as<mtl_depth_stencil_state_impl>()->get_mtl_raw_depth_stencil_state_ptr();
    }
    
    std::string mtl_depth_stencil_state::get_guid() const
    {
        return impl_as<mtl_depth_stencil_state_impl>()->get_guid();
    }
}

#endif
