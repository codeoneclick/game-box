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

    void texture_serializer_png::serialize()
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_filename, &m_status);
        
        texture_shared_ptr texture = std::static_pointer_cast<gb::texture>(m_resource);
        
        png_byte header[8];
        filestream->read((char *)header, 8);
        
        if (png_sig_cmp(header, 0, 8))
        {
            std::cout<<"error: "<<m_filename<<"is not a png."<<std::endl;
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
        }
        
        png_structp pngstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!pngstruct)
        {
            std::cout<<"error: png_create_read_struct returned 0."<<std::endl;
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
        }
        
        png_infop pnginfo = png_create_info_struct(pngstruct);
        if (!pnginfo)
        {
            std::cout<<"error: png_create_info_struct returned 0."<<std::endl;
            png_destroy_read_struct(&pngstruct, (png_infopp)NULL, (png_infopp)NULL);
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
        }
        
        png_infop pngendinfo = png_create_info_struct(pngstruct);
        if (!pngendinfo)
        {
            std::cout<<"error: png_create_info_struct returned 0."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, (png_infopp) NULL);
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
        }
        
        if (setjmp(png_jmpbuf(pngstruct)))
        {
            std::cout<<"error from libpng."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
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
        rowbytes += 3 - ((rowbytes - 1) % 4);
        
        data = new png_byte[rowbytes * height * sizeof(png_byte) + 15];
        if (data == nullptr)
        {
            std::cout<<"error: could not allocate memory for PNG image data."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
        }
        
        png_bytep *rowpointers = new png_bytep[height * sizeof(png_bytep)];
        if (rowpointers == nullptr)
        {
            std::cout<<"error: could not allocate memory for PNG row pointers."<<std::endl;
            png_destroy_read_struct(&pngstruct, &pnginfo, &pngendinfo);
            delete[] data;
            m_status = e_serializer_status_failure;
            resource_serializer::close_stream(filestream);
            return;
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
                format = GL_RGB;
            }
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
            {
                format = GL_RGBA;
            }
                break;
            case PNG_COLOR_TYPE_GRAY:
            {
#if defined(__OPENGL_30__)
                format = GL_RED;
#else
                format = GL_LUMINANCE;
#endif
            }
                break;
            default:
                assert(false);
                break;
        }
        
        resource_serializer::close_stream(filestream);
        
        texture_transfering_data_shared_ptr texture_transfering_data = std::make_shared<gb::texture_transfering_data>();
        texture_transfering_data->m_width = width;
        texture_transfering_data->m_height = height;
        texture_transfering_data->m_data = data;
        texture_transfering_data->m_format = format;
        texture_transfering_data->m_bpp = 8;
        texture_transfering_data->m_mips = 0;
        texture_transfering_data->m_compressed = false;
        
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(texture_transfering_data);
    }
}