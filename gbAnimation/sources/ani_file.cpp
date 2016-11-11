
#include "ani_file.h"

namespace gb
{
    namespace anim
    {
        void ani_file::read_header_begin(const std::shared_ptr<ani_header>& header)
        {
            read_bytes(&header->m_compression, 4);
            header->m_version = read_2_bytes();
            header->m_file_lenght = read_4_bytes();
        }
        
        ani_file::ani_file() :
        m_data(nullptr)
        {
            m_header = std::make_shared<ani_header>();
        }
        
        ani_file::~ani_file()
        {
            close();
        }
        
        ui8 ani_file::read_1_byte()
        {
            ui8 result = 0;
            read_bytes(&result, sizeof(ui8));
            return result;
        }
        
        ui16 ani_file::read_2_bytes()
        {
            ui16 result = 0;
            read_bytes(&result, sizeof(ui16));
            return result;
        }
        
        ui32 ani_file::read_4_bytes()
        {
            ui32 result = 0;
            read_bytes(&result, sizeof(ui32));
            return result;
        }
        
        ui64 ani_file::read_8_bytes()
        {
            ui64 result = 0;
            read_bytes(&result, sizeof(ui64));
            return result;
        }
        
        f32 ani_file::read_float()
        {
            f32 result = 0;
            read_bytes(&result, sizeof(f32));
            return result;
        }
        
        f64 ani_file::read_double()
        {
            f64 result = 0;
            read_bytes(&result, sizeof(f64));
            return result;
        }
        
        bool ani_file::is_eof() const
        {
            return m_data_position >= m_data_length;
        }
        
        size_t ani_file::read_string(std::string* out)
        {
            assert(m_data_position + sizeof(i16) <= m_data_length);
            ui16 length = read_2_bytes();
            
            assert(m_data_position + length <= m_data_length);
            
            char* data = new char[length];
            read_bytes(data, length);
            
            out->assign(data, length);
            
            delete[] data;
            
            return out->length() + sizeof(ui16);
        }
        
        void ani_file::read_bytes(void* out, ui32 length)
        {
            assert(out && m_data);
            assert(m_data_position + length <= m_data_length);
            
            memcpy(out, m_data + m_data_position, length);
            m_data_position += length;
        }
        
        void ani_file::close()
        {
            free(m_data);
            m_data = nullptr;
            m_data_length = 0;
            m_data_position = 0;
        }
        
        bool ani_file::open(const ui8* data, size_t length)
        {
            close();
            
            m_data = const_cast<ui8*>(data);
            m_data_length = length;
            
            if (m_data)
            {
                return process_open();
            }
            return false;
        }
        
        bool ani_file::open(const std::string& filepath, const char* open_mode)
        {
            close();
            
            m_data = get_data(filepath, open_mode, m_data_length);
            
            if (m_data)
            {
                return process_open();
            }
            
            return false;
        }
        
        bool ani_file::is_opened() const
        {
            return m_data != nullptr;
        }
        
        std::shared_ptr<ani_header> ani_file::get_header() const
        {
            return m_header;
        }
        
        ui32 ani_file::get_position() const
        {
            return m_data_position;
        }
        
        void ani_file::rewind(ui32 position)
        {
            m_data_position = position;
        }
        
        ui8* ani_file::get_data(const std::string& filename, const char* open_mode, ui64& out_length)
        {
            assert(!(filename.empty()));
            
            ui8* result = nullptr;
            out_length = 0;
            ssize_t size = 0;
            
            do
            {
                std::string fullpath = filename;
                FILE *file = fopen(fullpath.c_str(), open_mode);
                fseek(file, 0, SEEK_END);
                size = ftell(file);
                fseek(file, 0, SEEK_SET);
                
                result = static_cast<ui8*>(malloc(size));
                
                size = fread(result, sizeof(ui8), size, file);
                fclose(file);
            } while (0);
            
            if (nullptr == result || 0 == size)
            {
                std::string message = "get data from file(";
                message.append(filename).append(") failed!");
                std::cout<<message<<std::endl;
            }
            else
            {
                out_length = size;
            }
            return result;
        }
        
        bool ani_file::process_open()
        {
            read_header_begin(m_header);
            
            static const ui32 uncompressed_data_size = 10;
            
            assert(m_data_position == uncompressed_data_size);
            
            if (m_header->m_compression == ani_header::e_compressed_none)
            {
                
            }
            else if (m_header->m_compression == ani_header::e_compressed_zip)
            {

                uLongf uncompressed_size = m_header->m_file_lenght;
                Bytef* uncompressed_buffer = static_cast<Bytef*>(malloc(uncompressed_size));
                
                i32 status = uncompress(uncompressed_buffer, &uncompressed_size, (Bytef*)(m_data + m_data_position), m_data_length - m_data_position);
                
                if (status != Z_OK)
                {
                    free(uncompressed_buffer);
                    return false;
                }
                
                assert("error" && uncompressed_size == m_header->m_file_lenght);
                
                free(m_data);
                
                m_data = static_cast<ui8*>(malloc(uncompressed_size));
                
                memcpy(m_data, uncompressed_buffer, uncompressed_size);
                m_data_length = uncompressed_size;
                m_data_position = 0;
                
                free(uncompressed_buffer);
            }
            else
            {
                return false;
            }
            
            return true;
        }
    }
}
