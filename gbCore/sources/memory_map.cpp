//
//  memory_map.cpp
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "memory_map.h"
namespace gb
{
	i32 memory_map::g_filedescriptors = 0;

	memory_map::memory_map() :

#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)

		m_filedescriptor(-1),

#elif defined(__WIN32__)

		m_filedescriptor(nullptr),

#endif
		m_pointer(nullptr)
	{

	}

	memory_map::~memory_map()
	{
		memory_map::deallocate();
	}

#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)

    void* memory_map::allocate(const std::string& filename)
    {
        if(m_filedescriptor != -1)
        {
            assert(m_pointer != nullptr);
            return m_pointer;
        }
        ui32 filelength;
        struct stat status;
        
        m_filedescriptor = ::open(filename.c_str(), O_RDWR);
        if (m_filedescriptor < 0)
        {
            std::cout<<"can't open filedescriptor for filename: "<<filename<<"filedescriptors count: "<<g_filedescriptors<<std::endl;
            assert(false);
            return m_pointer;
        }
        g_filedescriptors++;
        
        if (fstat(m_filedescriptor, &status) < 0)
        {
            std::cout<<"can't retrive filedescriptor status for filename: "<<filename<<"filedescriptors count: "<<g_filedescriptors<<std::endl;
            memory_map::deallocate();
            assert(false);
            return m_pointer;
        }
        
        filelength = (ui32)status.st_size;
        m_pointer = (void* )mmap(0, filelength, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, m_filedescriptor, 0);
        if (m_pointer == nullptr)
        {
            std::cout<<"can't mmap filedescriptor for filename: "<<filename<<"filedescriptors count: "<<g_filedescriptors<<std::endl;
            memory_map::deallocate();
            assert(false);
            return m_pointer;
        }
        f32 size = static_cast<f32>(filelength) / (1024.0 * 1024);
        std::cout<<"filedescriptor was allocated: "<<filename<<"; filedescriptors count: "<<g_filedescriptors<<" size: "<<size<<"mb"<<std::endl;
        m_filename = filename;
        return m_pointer;
    }
    
    void memory_map::deallocate()
    {
        if(m_filedescriptor >= 0)
        {
            ::close(m_filedescriptor);
            g_filedescriptors--;
            std::cout<<"filedescriptor was deallocated: "<<m_filename.c_str()<<"; filedescriptors count: "<<g_filedescriptors<<std::endl;
        }
        m_filedescriptor = -1;
        m_pointer = nullptr;
    }
    
#elif defined(__WIN32__)

	void* memory_map::allocate(const std::string& filename)
	{
		if (m_filedescriptor != nullptr)
		{
			assert(m_pointer != nullptr);
			return m_pointer;
		}

		m_filedescriptor = CreateFile(std::wstring(filename.begin(), filename.end()).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (m_filedescriptor == nullptr)
		{
			std::cout << "can't open filedescriptor for filename: " << filename << "filedescriptors count: " << g_filedescriptors << std::endl;
			assert(false);
			return m_pointer;
		}
		g_filedescriptors++;

		ui32 filelength = (ui32)GetFileSize(m_filedescriptor, NULL);
		m_pointer = MapViewOfFile(m_filedescriptor, FILE_MAP_ALL_ACCESS, 0, 0, filelength);
		if (m_pointer == nullptr)
		{
			std::cout << "can't mmap filedescriptor for filename: " << filename << "filedescriptors count: " << g_filedescriptors << std::endl;
			memory_map::deallocate();
			assert(false);
			return m_pointer;
		}
		f32 size = static_cast<f32>(filelength) / (1024.0 * 1024);
		std::cout << "filedescriptor was allocated: " << filename << "; filedescriptors count: " << g_filedescriptors << " size: " << size << "mb" << std::endl;
		m_filename = filename;
		return m_pointer;
	}

	void memory_map::deallocate()
	{
		if (m_filedescriptor != nullptr)
		{
			CloseHandle(m_filedescriptor);
			g_filedescriptors--;
			std::cout << "filedescriptor was deallocated: " << m_filename.c_str() << "; filedescriptors count: " << g_filedescriptors << std::endl;
		}
		m_filedescriptor = nullptr;
		m_pointer = nullptr;
	}

#endif

	void memory_map::reallocate()
	{
		memory_map::deallocate();
		memory_map::allocate(m_filename);
	}
}
