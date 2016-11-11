#include "ani_tag_parser_atlas.h"
#include "ani_texture_atlas.h"
#include "ani_texture_atlas_element.h"
#include "ani_timeline.h"

namespace gb
{
    namespace anim
    {
        
        void ani_tag_parser_atlas::read(const ani_parser_t& parser,
                                        const std::shared_ptr<ani_stream>& stream,
                                        const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                        const std::shared_ptr<ani_timeline>& timeline)
        {
            std::shared_ptr<ani_texture_atlas> atlas = std::make_shared<ani_texture_atlas>();
            
            atlas->set_scale(stream->read_float());
            
            ui8 atlases_count = stream->read_ubyte();
            
            for (ui8 i = 0; i < atlases_count; ++i)
            {
                ani_texture_atlas::ani_atlas_metadata metadata;
                
                metadata.m_id = stream->read_u32();
                ui8 sources = stream->read_ubyte();
                
                for (ui8 j = 0; j < sources; ++j)
                {
                    ani_texture_atlas::ani_atlas_metadata::source source;
                    
                    stream->read_string(&source.m_source);
                    source.m_csf = stream->read_float();
                    
                    metadata.m_sources.push_back(source);
                }
                atlas->push_atlas_metadata(metadata);
            }
            
            ui32 elements_count = stream->read_u32();
            
            for (ui32 i = 0; i < elements_count; ++i)
            {
                std::shared_ptr<ani_texture_atlas_element> element = std::make_shared<ani_texture_atlas_element>();
                
                ani_tag_parser_base::deserialize(stream, &element->m_pivot);
                glm::vec2 origin;
                ani_tag_parser_base::deserialize(stream, &origin);
                f32 scale = stream->read_float();
                element->set_scale(scale);
                
                f32 width = stream->read_float();
                f32 height = stream->read_float();
                
                element->m_atlas_index = stream->read_u32();
                
                if (element->m_atlas_index > 0)
                {
                    element->m_atlas_index--;
                }
                
                element->m_element_atlas_index = stream->read_u32();
                element->m_bounds = glm::vec4(origin.x, origin.y, width, height);
                
                atlas->push_element(element->m_element_atlas_index, element);
                
                if (stream->get_input()->get_header()->get_major_version() >= 4)
                {
                    element->set_scale(element->get_scale() * atlas->get_scale());
                    
                    i8 has_scale_9_grid = stream->read_ubyte();
                    
                    if (has_scale_9_grid)
                    {
                        glm::vec4 scale_9_grid;
                        ani_tag_parser_base::deserialize(stream, &scale_9_grid);
                        element->m_scale9_grid = scale_9_grid;
                    }
                }
            }
            timeline->push_texture_atlas(atlas);
        }
    }
}

