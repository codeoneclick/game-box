#include "ani_tag_parser_sounds.h"


namespace gb
{
    namespace anim
    {
        void ani_tag_parser_sounds::read(const ani_parser_t& parser,
                                         const std::shared_ptr<ani_stream>& stream,
                                         const std::shared_ptr<ani_asset_metadata>& asset_metadata,
                                         const std::shared_ptr<ani_timeline>& timeline)
        {
            ui16 count = stream->read_u16();
            
            for (ui16 i = 0; i < count; ++i)
            {
                __attr_unused ui16 id = stream->read_u16();
                std::string linkage;
                stream->read_string(&linkage);
                std::string source;
                stream->read_string(&source);
                
                ui8 unused = stream->read_ubyte();
                unused = stream->read_ubyte();
                unused = stream->read_ubyte();
                
                __attr_unused bool stereo = stream->read_bool();
                __attr_unused ui32 samples_count = stream->read_u32();
            }
        }
    }
}
