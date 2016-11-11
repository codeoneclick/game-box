
#include "ani_stream.h"

namespace gb
{
    namespace anim
    {
        ani_stream::ani_stream(ani_file* input):
        m_input(input),
        m_current_byte(0),
        m_unused_bits(0)
        {
            assert(input);
        }
        
        ani_stream::~ani_stream()
        {
        }
        
        void ani_stream::read_n_bytes(void* dest, ui32 num)
        {
            align();
            m_input->read_bytes(dest, num);
        }
        
        bool ani_stream::read_bool()
        {
            return read_uint(1) ? true : false;
        }
        
        ui32 ani_stream::read_uint(ui16 bitcount)
        {
            assert(bitcount <= 32);
            
            ui32 result = 0;
            
            while (bitcount)
            {
                if (m_unused_bits)
                {
                    if (bitcount >= m_unused_bits)
                    {
                        result |= (m_current_byte << (bitcount - m_unused_bits));
                        bitcount -= m_unused_bits;
                        
                        m_current_byte = 0;
                        m_unused_bits = 0;
                    }
                    else
                    {
                        result |= (m_current_byte >> (m_unused_bits - bitcount));
                        m_current_byte &= ((1 << (m_unused_bits - bitcount)) - 1);
                        m_unused_bits -= bitcount;
                        
                        bitcount = 0;
                    }
                }
                else
                {
                    m_current_byte = m_input->read_1_byte();
                    m_unused_bits = 8;
                }
            }
            
            assert(bitcount == 0);
            
            return result;
        }
        
        i32 ani_stream::read_sint(ui16 bitcount)
        {
            assert(bitcount <= 32);
            
            i32 result = static_cast<i32>(read_uint(bitcount));
            
            if (result & (1 << (bitcount - 1)))
            {
                result |= -1 << bitcount;
            }
            
            return result;
        }
        
        f32 ani_stream::read_fixed()
        {
            i32 result = m_input->read_4_bytes();
            return static_cast<f32>(result / 65536.f);
        }
        
        f32 ani_stream::read_fixed_8()
        {
            i16 result = m_input->read_2_bytes();
            return static_cast<f32>(result / 255.f);
        }
        
        unsigned char ani_stream::read_ubyte()
        {
            align();
            return m_input->read_1_byte();
        }
        
        i8 ani_stream::read_sbyte()
        {
            align();
            return m_input->read_1_byte();
        }
        
        ui16 ani_stream::read_u16()
        {
            ui16 result = 0;
            read_n_bytes(&result, sizeof(ui16));
            return result;
        }
        
        ui32 ani_stream::read_u32()
        {
            align();
            return m_input->read_4_bytes();
        }
        
        i32 ani_stream::read_s32()
        {
            align();
            return static_cast<i32>(m_input->read_4_bytes());
        }
        
        f32 ani_stream::read_float()
        {
            return m_input->read_float();
        }
        
        void ani_stream::read_string(std::string* out)
        {
            m_input->read_string(out);
        }
        
        ani_file* ani_stream::get_input() const
        {
            return m_input;
        }
        
        tags::names ani_stream::open_tag()
        {
            align();
            
            ui16 tag_name = read_u16();
            ui32 tag_lenght = read_u32();
            
            assert(m_unused_bits == 0);
            
            tag_record record = { m_input->get_position() + tag_lenght, tag_lenght, (tags::names)tag_name };
            m_tags_stack.push(record);
            
            return (tags::names)tag_name;
        }
        
        void ani_stream::close_tag()
        {
            assert(!m_tags_stack.empty());
            
            tag_record record = m_tags_stack.top();
            
            m_tags_stack.pop();
            
            const ui32 input_position = m_input->get_position();
            
            if (record.m_expected_stream_position != input_position)
            {
                assert(false);
            }
            
            m_input->rewind(record.m_expected_stream_position);
            
            m_unused_bits = 0;
        }
        
        ui32 ani_stream::get_tag_lenght_on_stack_top() const
        {
            assert(!m_tags_stack.empty());
            return m_tags_stack.top().m_tag_size;
        }
        
        ui32 ani_stream::get_tag_expected_position() const
        {
            assert(!m_tags_stack.empty());
            return m_tags_stack.top().m_expected_stream_position;
        }
        
        ui32 ani_stream::get_position() const
        {
            return m_input->get_position();
        }
        
        void ani_stream::align()
        {
            m_current_byte = 0;
            m_unused_bits = 0;
        }
        
        bool ani_stream::is_end_of_stream() const
        {
            return m_input->is_eof();
        }
    }
}
