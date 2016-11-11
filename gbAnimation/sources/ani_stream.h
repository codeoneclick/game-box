#pragma once

#include "tags_defines.h"
#include "ani_file.h"

namespace gb
{
    namespace anim
    {
        class ani_stream
        {
        private:
            
            ani_file* m_input;
            ui8 m_current_byte;
            ui8 m_unused_bits;
            
            struct tag_record
            {
                ui32 m_expected_stream_position;
                ui32 m_tag_size;
                tags::names m_tag_name;
            };
            
            typedef std::stack<tag_record> tags_stack_t;
            tags_stack_t m_tags_stack;
            
        public:
            
            ani_stream(ani_file* input);
            ~ani_stream();
            
            void read_n_bytes(void* dest, ui32);
            bool read_bool();
            ui32 read_uint(ui16 bitcount);
            i32 read_sint(ui16 bitcount);
            f32 read_fixed();
            f32 read_fixed_8();
            
            f32 read_float();
            
            ui8 read_ubyte();
            i8 read_sbyte();
            
            ui8 read_u8() { return read_ubyte(); }
            i8 read_s8() { return read_sbyte(); }
            ui16 read_u16();
            ui32 read_u32();
            i32 read_s32();
            
            void read_string(std::string* out);
            
            ani_file* get_input() const;
            
            tags::names open_tag();
            void close_tag();
            ui32 get_tag_lenght_on_stack_top() const;
            ui32 get_tag_expected_position() const;
            ui32 get_position() const;
            
            void align();
            
            bool is_end_of_stream() const;
        };
    };
};
