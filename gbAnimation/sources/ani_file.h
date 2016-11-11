#pragma once

#include "ani_header.h"

namespace gb
{
    namespace anim
    {
        class ani_file
        {
        private:
            
            ui8* m_data;
            ui32 m_data_position;
            ui64 m_data_length;
            std::shared_ptr<ani_header> m_header;
            
        private:
            
            ui8* get_data(const std::string& filename, const char* open_mode, ui64& out_length);
            bool process_open();
            
        protected:
            
            void read_header_begin(const std::shared_ptr<ani_header>& header);
            
        public:
            
            ani_file();
            ~ani_file();
            
            ui8 read_1_byte();
            ui16 read_2_bytes();
            ui32 read_4_bytes();
            ui64 read_8_bytes();
            f32 read_float();
            f64 read_double();
            
            bool is_eof() const;
            
            size_t read_string(std::string* out);
            void read_bytes(void* out, ui32 length);
            
            void close();
            
            bool open(const std::string& filename, const char* open_mode);
            bool open(const ui8* data, size_t length);
            
            bool is_opened() const;
            
            std::shared_ptr<ani_header> get_header() const;
            
            ui32 get_position() const;
            void rewind(ui32 position);
        };
    };
};
