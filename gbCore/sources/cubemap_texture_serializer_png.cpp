//
//  cubemap_texture_serializer_png.cpp
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "cubemap_texture_serializer_png.h"
#include "texture_serializer_png.h"
#include "cubemap_texture.h"

namespace gb
{
    cubemap_texture_serializer_png::cubemap_texture_serializer_png(const std::string& filename,
                                                                   const resource_shared_ptr& resource) :
    gb::resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    void cubemap_texture_serializer_png::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        const auto texture_transfering_data = std::static_pointer_cast<gb::cubemap_texture_transfering_data>(transfering_data);
        texture_transfering_data->m_bpp = 8;
        texture_transfering_data->m_mips = 0;
        texture_transfering_data->m_compressed = false;
        
        std::array<std::string, 6> slice_suffix;
        slice_suffix[0] = "_px.png";
        slice_suffix[1] = "_nx.png";
        slice_suffix[2] = "_py.png";
        slice_suffix[3] = "_ny.png";
        slice_suffix[4] = "_pz.png";
        slice_suffix[5] = "_nz.png";

        std::array<std::string,6> filenames;
        bool top_down=false;
        if(m_filename.size()>=4 && m_filename.substr(m_filename.size()-4)==".xml"){
            auto stream=resource_serializer::open_stream(m_filename,&m_status);
            pugi::xml_document document;
            const auto parsed=document.load(*stream);
            resource_serializer::close_stream(stream);
            const auto root=document.child("cubemap");
            if(!parsed || !root){m_status=e_serializer_status_failure;return;}
            const char* keys[]={"positive_x","negative_x","positive_y","negative_y","positive_z","negative_z"};
            for(size_t face=0;face<6;++face){
                filenames[face]=root.attribute(keys[face]).as_string();
                if(filenames[face].empty()){m_status=e_serializer_status_failure;return;}
            }
            // read_png returns bottom-up rows for legacy 2D/GL assets.
            // Explicit cube manifests default to native top-down cube faces.
            top_down=!root.attribute("flip_vertical").as_bool(false);
        }else for(size_t face=0;face<6;++face)filenames[face]=m_filename+slice_suffix[face];
        
        for (ui32 slice = 0; slice < 6; ++slice)
        {
            const std::string& filename=filenames[slice];
            std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(filename, &m_status);
            const auto result = texture_serializer_png::read_png(filestream);
            
            auto pixels=std::get<0>(result);
            const ui32 size=std::get<1>(result),height=std::get<2>(result),format=std::get<3>(result);
            const ui32 channels=format==gl::constant::rgb_t?3:4;
            if(size==0 || size!=height || (slice>0 && size!=texture_transfering_data->m_size)
                || (format!=gl::constant::rgb_t && format!=gl::constant::rgba_t)
                || std::get<4>(result)!=static_cast<size_t>(size)*height*channels){
                delete[] pixels;resource_serializer::close_stream(filestream);m_status=e_serializer_status_failure;return;
            }
            if(format==gl::constant::rgb_t || top_down){
                auto rgba=new ui8[static_cast<size_t>(size)*size*4];
                for(ui32 y=0;y<size;++y)for(ui32 x=0;x<size;++x){
                    const size_t source=(static_cast<size_t>(top_down?size-1-y:y)*size+x)*channels;
                    const size_t target=(static_cast<size_t>(y)*size+x)*4;
                    for(ui32 c=0;c<3;++c)rgba[target+c]=pixels[source+c];
                    rgba[target+3]=channels==4?pixels[source+3]:255;
                }
                delete[] pixels;pixels=rgba;
            }
            texture_transfering_data->m_data[slice]=pixels;
            texture_transfering_data->m_size=size;
            texture_transfering_data->m_format=gl::constant::rgba_t;
            
            resource_serializer::close_stream(filestream);
        }
       
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(texture_transfering_data);
    }
}
