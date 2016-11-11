//
//  animation_serializer_ani.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "animation_serializer_ani.h"
#include "common.h"
#include "animation.h"
#include "ani_file.h"
#include "ani_stream.h"
#include "ani_asset_metadata.h"
#include "ani_timeline.h"
#include "ani_tag_parser_atlas.h"
#include "ani_tag_parser_animation_masks.h"
#include "ani_tag_parser_animation_objects.h"
#include "ani_tag_parser_animation_frames.h"
#include "ani_tag_parser_animation_frames2.h"
#include "ani_tag_parser_animation_frames3.h"
#include "ani_tag_parser_atlas3.h"
#include "ani_tag_parser_atlas4.h"
#include "ani_tag_parser_textfield.h"
#include "ani_tag_parser_timeline.h"
#include "ani_tag_parser_timeline2.h"
#include "ani_tag_parser_timeline3.h"
#include "ani_tag_parser_sounds.h"
#include "ani_tag_parser_external_objects.h"
#include "ani_tag_parser_external_objects2.h"
#include "ani_tag_parser_stage.h"
#include "ani_tag_parser_named_parts.h"
#include "ani_tag_parser_sequences.h"

namespace gb
{
    namespace anim
    {
        animation_serializer_ani::animation_serializer_ani(const std::string& filename,
                                                           const resource_shared_ptr& resource) :
        gb::resource_serializer(filename, resource),
        m_filename(filename)
        {
            m_parser = std::bind(&animation_serializer_ani::parse, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        }
        
        animation_serializer_ani::~animation_serializer_ani()
        {
            
        }
        
        void animation_serializer_ani::read_header_end(const std::shared_ptr<ani_header>& header, const std::shared_ptr<ani_stream>& stream)
        {
            header->m_frames_count = stream->read_u16();
            ani_tag_parser_base::deserialize(stream, &header->m_frame_size);
            ani_tag_parser_base::deserialize(stream, &header->m_pivot);
        }
        
        void animation_serializer_ani::read_header_end_v4(const std::shared_ptr<ani_header>& header, const std::shared_ptr<ani_stream>& stream)
        {
            size_t scale_values_count = stream->read_u32();
            while (scale_values_count)
            {
                f32 value = stream->read_float();
                header->m_scale_values.push_back(value);
                scale_values_count--;
            }
            
            size_t csf_values_count = stream->read_u32();
            while (csf_values_count)
            {
                f32 value = stream->read_float();
                header->m_csf_values.push_back(value);
                csf_values_count--;
            }
        }
        
        void animation_serializer_ani::register_tag_loaders_v3()
        {
            m_tags_parsers[tags::TagDefineAtlas] = std::make_shared<ani_tag_parser_atlas>();
            m_tags_parsers[tags::TagDefineAnimationMasks] = std::make_shared<ani_tag_parser_animation_masks>();
            m_tags_parsers[tags::TagDefineAnimationObjects] = std::make_shared<ani_tag_parser_animation_objects>();
            m_tags_parsers[tags::TagDefineAnimationFrames] = std::make_shared<ani_tag_parser_animation_frames>();
        }
        
        void animation_serializer_ani::register_tag_loaders_v4()
        {
            m_tags_parsers[tags::TagDefineAnimationFrames2] = std::make_shared<ani_tag_parser_animation_frames2>();
            m_tags_parsers[tags::TagDefineAnimationObjects2] = std::make_shared<ani_tag_parser_animation_objects>();
            m_tags_parsers[tags::TagDefineAnimationMasks2] = std::make_shared<ani_tag_parser_animation_masks>();
            m_tags_parsers[tags::TagDefineAtlas2] = std::make_shared<ani_tag_parser_atlas>();
            m_tags_parsers[tags::TagDefineAtlas3] = std::make_shared<ani_tag_parser_atlas3>();
            m_tags_parsers[tags::TagDefineAtlas4] = std::make_shared<ani_tag_parser_atlas4>();
            m_tags_parsers[tags::TagDefineTextFields] = std::make_shared<ani_tag_parser_textfield>();
            m_tags_parsers[tags::TagDefineTimeline] = std::make_shared<ani_tag_parser_timeline>();
            m_tags_parsers[tags::TagDefineTimeline2] = std::make_shared<ani_tag_parser_timeline2>();
            m_tags_parsers[tags::TagDefineTimeline3] = std::make_shared<ani_tag_parser_timeline3>();
            m_tags_parsers[tags::TagDefineSounds] = std::make_shared<ani_tag_parser_sounds>();
            m_tags_parsers[tags::TagDefineExternalObjects] = std::make_shared<ani_tag_parser_external_objects>();
            m_tags_parsers[tags::TagDefineAnimationFrames3] = std::make_shared<ani_tag_parser_animation_frames3>();
            m_tags_parsers[tags::TagDefineExternalObjects2] = std::make_shared<ani_tag_parser_external_objects2>();
        }
        
        void animation_serializer_ani::register_tag_loaders_common()
        {
            m_tags_parsers[tags::TagDefineStage] = std::make_shared<ani_tag_parser_stage>();
            m_tags_parsers[tags::TagDefineNamedParts] = std::make_shared<ani_tag_parser_named_parts>();
            m_tags_parsers[tags::TagDefineSequences] = std::make_shared<ani_tag_parser_sequences>();
        }
        
        void animation_serializer_ani::serialize(const resource_transfering_data_shared_ptr& transfering_data)
        {
            m_status = e_serializer_status_in_progress;
            
            std::shared_ptr<ani_asset_metadata> asset_metadata = nullptr;
            std::string filepath = bundlepath().append(m_filename).c_str();
            
            std::shared_ptr<ani_file> file = std::make_shared<gb::anim::ani_file>();
            if(file->open(filepath, "rb"))
            {
                std::shared_ptr<ani_stream> stream = std::make_shared<ani_stream>(file.get());
                std::shared_ptr<ani_header> header = stream->get_input()->get_header();
                
                asset_metadata = ani_asset_metadata::create(m_filename);
                
                if (header->get_major_version() >= 4)
                {
                    read_header_end_v4(header, stream);
                    register_tag_loaders_v4();
                }
                else
                {
                    read_header_end(header, stream);
                    register_tag_loaders_v3();
                    
                    std::shared_ptr<ani_timeline> timeline = std::make_shared<ani_timeline>(nullptr, 0,
                                                                                            header->m_frame_size, header->m_pivot, header->m_frames_count);
                    asset_metadata->push_timeline(0, timeline);
                    asset_metadata->set_root_timeline(0);
                }
                
                animation_serializer_ani::register_tag_loaders_common();
                
                asset_metadata->set_header(header);
                
                animation_serializer_ani::parse(stream, asset_metadata, nullptr);
            }
            
            std::shared_ptr<gb::anim::animation_transfering_data> animation_transfering_data = std::static_pointer_cast<gb::anim::animation_transfering_data>(transfering_data);
            animation_transfering_data->m_metadata = asset_metadata;
            
            m_status = e_serializer_status_success;
            resource_serializer::on_transfering_data_serialized(animation_transfering_data);
        }
        
        void animation_serializer_ani::parse(const std::shared_ptr<ani_stream> &stream, const std::shared_ptr<ani_asset_metadata> &asset_metadata, const std::shared_ptr<ani_timeline> &timeline)
        {
            while (!stream->is_end_of_stream())
            {
                tags::names current_tag = stream->open_tag();
                auto current_tag_parser = m_tags_parsers.find(current_tag);
                if (current_tag_parser != m_tags_parsers.end())
                {
                    current_tag_parser->second->read(m_parser, stream, asset_metadata, timeline);
                }
                
                stream->close_tag();
                
                if (current_tag == tags::TagEnd)
                {
                    break;
                }
            }
        }
    }
}
