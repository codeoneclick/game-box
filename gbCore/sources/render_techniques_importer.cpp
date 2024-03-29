//
//  render_techniques_importer.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_techniques_importer.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "graphics_context.h"
#include "render_technique_main.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "material.h"
#include "mesh_2d.h"
#include "mesh_constructor.h"
#include "built_in_shaders.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>
#include <QuartzCore/QuartzCore.h>

#elif defined(__IOS__) || defined(__TVOS__)

#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

#endif

namespace gb
{
    i32 render_techniques_importer::m_render_techniques_count = 0;

    i32 render_techniques_importer::get_render_techniques_count()
    {
        return m_render_techniques_count;
    }

    render_techniques_importer::render_techniques_importer(const std::shared_ptr<graphics_context>& graphics_context, bool is_offscreen) :
    m_graphics_context(graphics_context),
    m_offscreen(is_offscreen),
    m_main_render_technique(nullptr)
    {
        assert(m_graphics_context);
    }
    
    render_techniques_importer::~render_techniques_importer()
    {
        
    }
    
    void render_techniques_importer::create_main_render_technique(const std::string& technique_name, const material_shared_ptr& material)
    {
        assert(m_offscreen == false);
        assert(m_graphics_context != nullptr);
        assert(material != nullptr);
        m_main_render_technique = std::make_shared<render_technique_main>(m_graphics_context->get_width(),
                                                                          m_graphics_context->get_height(),
                                                                          technique_name,
                                                                          material,
                                                                          m_graphics_context->get_frame_buffer(),
                                                                          m_graphics_context->get_render_buffer());

        std::cout<<"[Output resolution] : "<<m_graphics_context->get_width()<<"x"<<m_graphics_context->get_height()<<std::endl;
        
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API
        
        std::cout<<"["<<glGetString(GL_RENDERER)<<"] ["<<glGetString(GL_VERSION)<<"] ["<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<"]"<<std::endl;
        
        i32 max_uniform_vectors;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_uniform_vectors);
        std::cout<<"[Max uniform vectors] : "<<max_uniform_vectors<<std::endl;
#endif
        
    }
    
    void render_techniques_importer::add_ws_render_technique(const std::string &technique_name, i32 technique_index,
                                                             const std::shared_ptr<render_technique_ws> &technique)
    {
        std::stringstream guid_stream;
        guid_stream<<technique_index<<"_"<<technique_name;
        std::string guid = guid_stream.str();
        
        assert(m_ws_render_techniques.find(guid) == m_ws_render_techniques.end());
        m_ws_render_techniques.insert(std::make_pair(guid, technique));
		m_ordered_ws_render_techniques.push_back(technique);
		m_ordered_ws_render_techniques.sort([](const std::shared_ptr<render_technique_ws> &value_01, const std::shared_ptr<render_technique_ws> &value_02) {
			return value_01->get_order() < value_02->get_order();
		});
        m_render_techniques_count++;
    }
    
    void render_techniques_importer::remove_ws_render_technique(const std::string &technique_name, i32 technique_index)
    {
        std::stringstream guid_stream;
        guid_stream<<technique_index<<technique_name;
        std::string guid = guid_stream.str();
        
        const auto& technique = m_ws_render_techniques.find(guid);
        assert(technique != m_ws_render_techniques.end());

		if (technique != m_ws_render_techniques.end())
		{
			m_ws_render_techniques.erase(technique);
			const auto& ordered_technique = std::find(m_ordered_ws_render_techniques.begin(), m_ordered_ws_render_techniques.end(), technique->second);
			if (ordered_technique != m_ordered_ws_render_techniques.end())
			{
				m_ordered_ws_render_techniques.erase(ordered_technique);
			}
			else
			{
				assert(false);
			}
		}
        m_render_techniques_count--;
    }
    
    void render_techniques_importer::add_ss_render_technique(const std::string &technique_name, const std::shared_ptr<render_technique_ss> &technique)
    {
        assert(m_ss_render_techniques.find(technique_name) == m_ss_render_techniques.end());
        m_ss_render_techniques.insert(std::make_pair(technique_name, technique));
        m_ordered_ss_render_techniques.push_back(technique);
        m_render_techniques_count++;
    }
    
    void render_techniques_importer::remove_ss_render_technique(const std::string &technique_name)
    {
        const auto& iterator = m_ss_render_techniques.find(technique_name);
        assert(iterator != m_ss_render_techniques.end());
        m_ss_render_techniques.erase(iterator);
        m_render_techniques_count--;
    }
    
    void render_techniques_importer::save_texture(const std::shared_ptr<texture>& texture, const std::string &filename, ui32 width, ui32 height)
	{
        assert(texture != nullptr);
        assert(texture->is_loaded() && texture->is_commited());
        
        material_shared_ptr material = std::make_shared<gb::material>();
        shader_shared_ptr shader = gb::shader::construct("shader_screen_quad_tex2d", shader_screen_quad_tex2d_vert, shader_screen_quad_tex2d_frag);
        assert(shader != nullptr);
        mesh_2d_shared_ptr quad = mesh_constructor::create_screen_quad();
        material->set_shader(shader);
        material->set_texture(texture, e_shader_sampler_01);
        
        material->set_culling(false);
        material->set_culling_mode(gl::constant::back);
        
        material->set_blending(false);
        material->set_blending_function_source(gl::constant::src_alpha);
        material->set_blending_function_destination(gl::constant::one);
        
        material->set_depth_test(false);
        material->set_depth_mask(true);
        
#if USED_GRAPHICS_API == VULKAN_API

		material->bind(quad->get_vbo()->get_vertex_input_state());

#elif USED_GRAPHICS_API == METAL_API
        
        material->bind(quad->get_vbo()->get_mtl_vertex_descriptor());
        
#else
        
		material->bind();

#endif

        assert(material->get_shader()->get_attributes().at(e_shader_attribute_position) >= 0);
        assert(material->get_shader()->get_attributes().at(e_shader_attribute_texcoord) >= 0);
        quad->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
        
        quad->draw();
        
        quad->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
        material->unbind();
        
        ui32 rawdata_size = static_cast<ui32>(width) * static_cast<ui32>(height) * 4;
        ui8 *rawdata = new ui8[rawdata_size];
        gl::command::read_pixels(0, 0, width, height, gl::constant::rgba_t, gl::constant::ui8_t, rawdata);
        
#if defined(__OSX__)
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rawdata, rawdata_size, NULL);
        ui32 bitsPerComponent = 8;
        ui32 bitsPerPixel = 32;
        ui32 bytesPerRow = 4 * width;
        CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
        CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
        CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
        CGImageRef image = CGImageCreate(width,
                                         height,
                                         bitsPerComponent,
                                         bitsPerPixel,
                                         bytesPerRow,
                                         colorSpaceRef,
                                         bitmapInfo,
                                         provider, NULL, NO, renderingIntent);
        
        CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:[NSString stringWithUTF8String:filename.c_str()]];
        CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
        CGImageDestinationAddImage(destination, image, nil);
        
        if (!CGImageDestinationFinalize(destination))
        {
            assert(false);
        }
        CFRelease(destination);
        
#elif defined(__IOS__)
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rawdata, rawdata_size, NULL);
        
        ui32 bitsPerComponent = 8;
        ui32 bitsPerPixel = 32;
        ui32 bytesPerRow = 4 * width;
        CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
        CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast;
        CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
        CGImageRef imageRef = CGImageCreate(width,
                                            height,
                                            bitsPerComponent,
                                            bitsPerPixel,
                                            bytesPerRow,
                                            colorSpaceRef,
                                            bitmapInfo,
                                            provider, NULL, NO, renderingIntent);
        UIImage *image = [UIImage imageWithCGImage:imageRef];
        
        UIImageView *imageView = [[UIImageView alloc] initWithImage:image];
        UIGraphicsBeginImageContext(CGSizeMake(width, height));
        CGContextRef context = UIGraphicsGetCurrentContext();
        CGAffineTransform flip = CGAffineTransformMake(1, 0, 0, -1, 0, width);
        CGContextConcatCTM(context, flip);
        [imageView.layer renderInContext:context];
        image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *imageFilePath = [[paths objectAtIndex:0] stringByAppendingPathComponent:
                                   [NSString stringWithCString:filename.c_str()
                                                      encoding:[NSString defaultCStringEncoding]]];
        [UIImagePNGRepresentation(image) writeToFile:imageFilePath atomically:YES];
        
#endif
        
        delete[] rawdata;
    }
}

#endif
