//
//  render_techniques_importer.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_techniques_importer.h"
#include "graphics_context.h"
#include "render_technique_main.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "material.h"
#include "mesh.h"
#include "mesh_constructor.h"
#include "built_in_shaders.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>
#include <QuartzCore/QuartzCore.h>

#elif defined(__IOS__)

#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

#endif

namespace gb
{
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
    
    void render_techniques_importer::create_main_render_technique(const material_shared_ptr& material)
    {
        assert(m_offscreen == false);
        assert(m_graphics_context != nullptr);
        assert(material != nullptr);
        m_main_render_technique = std::make_shared<render_technique_main>(m_graphics_context->get_width(),
                                                                          m_graphics_context->get_height(),
                                                                          material,
                                                                          m_graphics_context->get_frame_buffer(),
                                                                          m_graphics_context->get_render_buffer());

        std::cout<<"[Output resolution] : "<<m_graphics_context->get_width()<<"x"<<m_graphics_context->get_height()<<std::endl;
        std::cout<<"["<<glGetString(GL_RENDERER)<<"] ["<<glGetString(GL_VERSION)<<"] ["<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<"]"<<std::endl;
        
        i32 max_uniform_vectors;
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_uniform_vectors);
        std::cout<<"[Max uniform vectors] : "<<max_uniform_vectors<<std::endl;
    }
    
    void render_techniques_importer::add_ws_render_technique(const std::string &technique_name, i32 technique_index,
                                                             const std::shared_ptr<render_technique_ws> &technique)
    {
        std::stringstream guid_stream;
        guid_stream<<technique_index<<"_"<<technique_name;
        std::string guid = guid_stream.str();
        
        assert(m_ws_render_techniques.find(guid) == m_ws_render_techniques.end());
        m_ws_render_techniques.insert(std::make_pair(guid, technique));
    }
    
    void render_techniques_importer::remove_ws_render_technique(const std::string &technique_name, i32 technique_index)
    {
        std::stringstream guid_stream;
        guid_stream<<technique_index<<technique_name;
        std::string guid = guid_stream.str();
        
        const auto& iterator = m_ws_render_techniques.find(guid);
        assert(iterator != m_ws_render_techniques.end());
        m_ws_render_techniques.erase(iterator);
    }
    
    void render_techniques_importer::add_ss_render_technique(const std::string &technique_name, const std::shared_ptr<render_technique_ss> &technique)
    {
        assert(m_ss_render_techniques.find(technique_name) == m_ss_render_techniques.end());
        m_ss_render_techniques.insert(std::make_pair(technique_name, technique));
    }
    
    void render_techniques_importer::remove_ss_render_technique(const std::string &technique_name)
    {
        const auto& iterator = m_ss_render_techniques.find(technique_name);
        assert(iterator != m_ss_render_techniques.end());
        m_ss_render_techniques.erase(iterator);
    }
    
    void render_techniques_importer::save_texture(const std::shared_ptr<texture>& texture, const std::string &filename, ui32 width, ui32 height)
    {
        assert(texture != nullptr);
        assert(texture->is_loaded() && texture->is_commited());
        
        material_shared_ptr material = std::make_shared<gb::material>();
        shader_shared_ptr shader = gb::shader::construct("shader_texture_2d", shader_texure2d_vert, shader_texure2d_frag);
        assert(shader != nullptr);
        mesh_shared_ptr quad = mesh_constructor::create_screen_quad();
        material->set_shader(shader);
        material->set_texture(texture, e_shader_sampler_01);
        
        material->set_culling(false);
        material->set_culling_mode(GL_BACK);
        
        material->set_blending(false);
        material->set_blending_function_source(GL_SRC_ALPHA);
        material->set_blending_function_destination(GL_ONE);
        
        material->set_depth_test(false);
        material->set_depth_mask(true);
        
        material->set_clipping(false);
        material->set_clipping_plane(glm::vec4(0.0, 0.0, 0.0, 0.0));
        
        material->set_reflecting(false);
        material->set_shadowing(false);
        material->set_debugging(false);
        
        material->bind();
        assert(material->get_shader()->get_attributes().at(e_shader_attribute_position) >= 0);
        assert(material->get_shader()->get_attributes().at(e_shader_attribute_texcoord) >= 0);
        quad->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
        
        quad->draw();
        
        quad->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
        material->unbind();
        
        ui32 rawdataSize = static_cast<ui32>(width) * static_cast<ui32>(height) * 4;
        ui8 *rawdata = new ui8[rawdataSize];
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rawdata);
        
#if defined(__OSX__)
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rawdata, rawdataSize, NULL);
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
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rawdata, rawdataSize, NULL);
        
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