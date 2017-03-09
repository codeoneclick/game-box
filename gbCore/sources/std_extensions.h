//
//  std_extensions.h
//  gbCore
//
//  Created by sergey.sergeev on 9/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef std_extensions_h
#define std_extensions_h

#include "main_headers.h"

#if defined(__IOS__) || defined(__OSX__)

#include <mach/mach.h>
#include <mach/mach_time.h>

#endif

namespace std
{
    inline ui64 get_tick_count()
    {
#if defined(__IOS__) || defined(__OSX__)
        static mach_timebase_info_data_t timebase_data;
        uint64_t mach_time = mach_absolute_time();
        if (timebase_data.denom == 0 )
        {
            (void)mach_timebase_info(&timebase_data);
        }
        uint64_t milliseconds = ((mach_time / 1000000) * timebase_data.numer) / timebase_data.denom;
        return milliseconds;
#endif
        return 0;
    }
    
    inline f32 get_random_f(f32 min, f32 max)
    {
#if defined(__IOS__) || defined(__OSX__)
        f32 random = (((f32)arc4random() / 0x100000000) * (max - min) + min);
        return random;
#endif
        return 0;
    };
    
    inline i32 get_random_i(i32 min, i32 max)
    {
        static std::random_device device;
        static std::mt19937 algorithm(device());
        std::uniform_int_distribution<> distribution(min, max);
        return distribution(algorithm);
    };
    
    inline std::string get_guid()
    {
        static const i8 alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        
        char c_string[16];
        
        for (i32 i = 0; i < 15; ++i)
        {
            c_string[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        c_string[15] = 0;
        
        return std::string(c_string);
    };
    
    inline i32 next_pot_2(i32 value)
    {
        i32 rval = 1;
        while(rval < value) rval <<= 1;
        return rval;
    };
    
    template <typename TYPE>
    class property_rw
    {
    private:
        
        std::function<TYPE()> m_getter;
        std::function<void(const TYPE&)> m_setter;
        
    protected:
        
    public:
        
        property_rw() :
        m_getter(nullptr),
        m_setter(nullptr)
        {
            
        }
        
        void getter(const std::function<TYPE()>& function)
        {
            m_getter = function;
        }
        
        void setter(const std::function<void(const TYPE&)>& function)
        {
            m_setter = function;
        }
        
        operator TYPE() const
        {
            if(!m_getter)
            {
                assert(false);
            }
            return m_getter();
        }
        
        void operator=(const TYPE& value)
        {
            if(!m_setter)
            {
                assert(false);
            }
            m_setter(value);
        }
    };
    
    template <typename TYPE>
    class property_ro
    {
    private:
        
        std::function<TYPE()> m_getter;
        
    protected:
        
    public:
        
        property_ro() :
        m_getter(nullptr)
        {
            
        }
        
        void getter(const std::function<TYPE()>& function)
        {
            m_getter = function;
        }
        
        operator TYPE() const
        {
            if(!m_getter)
            {
                assert(false);
            }
            return m_getter();
        }
    };
    
    template<typename CONTAINER_T, typename PREDICATE_T>
    inline void erase_if(CONTAINER_T& container, const PREDICATE_T& predicate)
    {
        for(auto iterator = container.begin(); iterator != container.end();)
        {
            if(predicate(*iterator) )
            {
                iterator = container.erase(iterator);
            }
            else
            {
                ++iterator;
            }
        }
    };
    
    inline std::string class_name(const std::string& pretty_function)
    {
        size_t end = pretty_function.rfind("::");
        if(end == std::string::npos)
        {
            return "";
        }
        size_t begin = pretty_function.substr(0, end).rfind(" ") + 1;
        end = end - begin;
        return pretty_function.substr(begin, end);
    }
    
#define __CLASS_NAME__ class_name(__PRETTY_FUNCTION__)
}

#endif
