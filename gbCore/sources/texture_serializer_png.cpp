//
//  texture_serializer_png.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "texture_serializer_png.h"
#include "texture.h"

namespace gb
{
    void png_read_fn(png_structp pngstruct, png_bytep data,
                     png_size_t size)
    {
        if(pngstruct->io_ptr == NULL)
        {
            return;
        }
        
        std::istream* filestream = (std::istream*)pngstruct->io_ptr;
        filestream->read((char *)data, static_cast<i32>(size));
    }
    
    texture_serializer_png::texture_serializer_png(const std::string& filename,
                                                   const resource_shared_ptr& resource) :
    gb::resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    texture_serializer_png::~texture_serializer_png()
    {
        
    }
    
    std::tuple<png_byte*, ui32, ui32, ui32, ui32> texture_serializer_png::read_png(const std::shared_ptr<std::istream>& filestream)
    {
        png_byte header[8];
        filestream->read((char *)header, 8);
        
        i32 png_sig = png_sig_cmp(header, 0, 8);
        if(png_sig)
        {
            std::cout<<"error: is not png."<<std::endl;
            resource_serializer::close_stream(filestream);
        }
        
        png_structp pngstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!pngstruct)
        {
            std::cout<<"error: png_create_read_struct returned 0."<<std::endl;
            resource_serializer::close_stream(filestream);
        }
        
        png_infop pnginfo = png_create_info_struct(pngstruct);
        if (!pnginfo)
        {
            std::cout<<"error: png_create_info_struct returned 0."<<std::endl;
            png_destroy_read_struct(&pngstruct, (png_infopp)NULL, (png_infopp)NULL);
            resource_serializer::close_stream(filestream);
        }
        
        png_infop pngendinfo = png_create_info_struct(pngstruct);
        if (!pngendinfo)
        {
            std::cout<<"error: png_create_info_struct returned 0."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, (png_infopp) NULL);
            resource_serializer::close_stream(filestream);
        }
        
        if (setjmp(png_jmpbuf(pngstruct)))
        {
            std::cout<<"error from libpng."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            resource_serializer::close_stream(filestream);
        }
        
        png_set_read_fn(pngstruct, filestream.get(), png_read_fn);
        
        filestream->seekg(0, std::ios::beg);
        
        png_read_info(pngstruct, pnginfo);
        
        i32 bitdepth, colortype;
        ui32 width, height, format;
        png_byte* data;
        
        png_get_IHDR(pngstruct, pnginfo, &width, &height, &bitdepth, &colortype,
                     NULL, NULL, NULL);
        format = colortype;
        
        png_read_update_info(pngstruct, pnginfo);
        png_size_t rowbytes = png_get_rowbytes(pngstruct, pnginfo);
        
        ui64 size = rowbytes * height * sizeof(png_byte);
        
        data = new png_byte[size];
        if (data == nullptr)
        {
            std::cout<<"error: could not allocate memory for PNG image data."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            resource_serializer::close_stream(filestream);
        }
        
        png_bytep *rowpointers = new png_bytep[height * sizeof(png_bytep)];
        if (rowpointers == nullptr)
        {
            std::cout<<"error: could not allocate memory for PNG row pointers."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            delete[] data;
            resource_serializer::close_stream(filestream);
        }
        
        for (ui32 i = 0; i < height; ++i)
        {
            rowpointers[height - 1 - i] = data + i * rowbytes;
        }
        png_read_image(pngstruct, rowpointers);
        png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
        delete[] rowpointers;
        
        switch (format)
        {
            case PNG_COLOR_TYPE_RGB:
            {
                format = gl::constant::rgb_t;
            }
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
            {
                format = gl::constant::rgba_t;
            }
                break;
            case PNG_COLOR_TYPE_GRAY:
            {
#if USED_GRAPHICS_API != NO_GRAPHICS_API
                
#if USED_GRAPHICS_API == OPENGL_30_API
                
                format = GL_RED;
                
#elif USED_GRAPHICS_API == OPENGL_20_API
                
                format = GL_LUMINANCE;
                
#endif
                
#endif
            }
                break;
            default:
                assert(false);
                break;
        }
        
#if USED_GRAPHICS_API == NO_GRAPHICS_API
        
        format = 0;
        
#endif
        
        return std::make_tuple(data, width, height, format, size);
    }

    void texture_serializer_png::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_filename, &m_status);
        const auto result = read_png(filestream);
        resource_serializer::close_stream(filestream);

        texture_transfering_data_shared_ptr texture_transfering_data = std::static_pointer_cast<gb::texture_transfering_data>(transfering_data);
        texture_transfering_data->m_width = std::get<1>(result);
        texture_transfering_data->m_height = std::get<2>(result);
        texture_transfering_data->m_data = std::get<0>(result);
        texture_transfering_data->m_size = std::get<4>(result);
        texture_transfering_data->m_format = std::get<3>(result);
        texture_transfering_data->m_bpp = 8;
        texture_transfering_data->m_mips = 0;
        texture_transfering_data->m_compressed = false;
        
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(texture_transfering_data);
    }
}
