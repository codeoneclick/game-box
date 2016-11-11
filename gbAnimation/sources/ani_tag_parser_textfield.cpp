#include "ani_tag_parser_textfield.h"

namespace gb
{
    namespace anim
    {
        void ani_tag_parser_textfield::read(const ani_parser_t& parser,
                                            const std::shared_ptr<ani_stream>& stream,
                                            const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                            const std::shared_ptr<ani_timeline>& timeline)
        {
            ui32 count = stream->read_u32();
            
            for (ui32 i = 0; i < count; ++i)
            {
                __attr_unused ui32 object_id_reference = stream->read_u32();
                glm::vec2 pivot;
                ani_tag_parser_base::deserialize(stream, &pivot);
                
                __attr_unused f32 width = stream->read_float();
                __attr_unused f32 height = stream->read_float();
                std::string string_data;
                stream->read_string(&string_data);
                __attr_unused ui8 is_embed_fonts = stream->read_ubyte() ? true : false;
                __attr_unused ui8 is_multiline = stream->read_ubyte() ? true : false;
                __attr_unused ui8 is_word_wrap = stream->read_ubyte() ? true : false;
                ui8 has_restrict = stream->read_ubyte() ? true : false;
                if (has_restrict)
                {
                    std::string restrict_data;
                    stream->read_string(&restrict_data);
                }
                __attr_unused ui8 is_editable = stream->read_ubyte() ? true : false;
                __attr_unused ui8 is_selectable = stream->read_ubyte() ? true : false;
                __attr_unused ui8 display_as_password = stream->read_ubyte() ? true : false;
                __attr_unused ui8 max_chars = stream->read_u32();

                {
                    __attr_unused ui32 align = stream->read_u32();
                    __attr_unused ui32 block_indent = stream->read_u32();
                    __attr_unused ui8 is_bold = stream->read_ubyte() ? true : false;
                    __attr_unused ui8 is_bullet = stream->read_ubyte() ? true : false;
                    
                    __attr_unused ui32 color = stream->read_u32();
                    
                    std::string font;
                    stream->read_string(&font);
                    __attr_unused ui32 indent = stream->read_u32();
                    __attr_unused ui8 is_italic = stream->read_ubyte() ? true : false;
                    __attr_unused ui8 use_kerning = stream->read_ubyte() ? true : false;
                    __attr_unused ui32 leading = stream->read_u32();
                    __attr_unused ui32 left_margin = stream->read_u32();
                    __attr_unused f32 letter_spacing = stream->read_float();
                    __attr_unused ui32 right_margin = stream->read_u32();
                    __attr_unused ui32 size = stream->read_u32();
                    
                    ui32 tabs_count = stream->read_u32();
                    for (ui32 j = 0; j < tabs_count; ++j)
                    {
                        __attr_unused ui32 tab_value = stream->read_u32();
                    }
                    
                    std::string target;
                    stream->read_string(&target);
                    __attr_unused ui8 is_underline = stream->read_ubyte() ? true : false;
                    std::string url;
                    stream->read_string(&url);
                }
            }
        }
    }
}
