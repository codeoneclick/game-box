//
//  std_extensions.h
//  gbCore
//
//  Created by sergey.sergeev on 9/1/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

#include <mach/mach.h>
#include <mach/mach_time.h>

#endif

namespace std
{
    inline ui64 get_tick_count()
    {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
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
        static std::random_device device;
        static std::mt19937 algorithm(device());
        std::uniform_real_distribution<> distribution(min, max);
        return static_cast<f32>(distribution(algorithm));
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
    };
    
    inline bool is_f32_equal(f32 value_01, f32 value_02)
    {
        return fabsf(value_01 - value_02) <= std::numeric_limits<f32>::epsilon();
    };
    
    #define SIGNMASK(i) (-(int)(((unsigned int)(i))>>31))
    inline bool is_f32_equal_v2(f32 value_01, f32 value_02, i32 max_diff = 1)
    {
        i32 value_01i = *reinterpret_cast<i32*>(&value_01);
        i32 value_02i = *reinterpret_cast<i32*>(&value_02);
        i32 sign = SIGNMASK(value_01i ^ value_02i);
        i32 diff = (value_01i ^ (sign & 0x7fffffff)) - value_02i;
        i32 v1 = max_diff + diff;
        i32 v2 = max_diff - diff;
        return (v1 | v2) >= 0;
    };

	inline std::vector<std::string> split_string(const std::string& str, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(str);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	};

/*!Convert RGB to HSV color space
  
  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0,
  1], respectively.
  
  Red component, used as output, range: [0, 1]
  Green component, used as output, range: [0, 1]
  Blue component, used as output, range: [0, 1]
  H Hue component, used as input, range: [0, 360]
  S Hue component, used as input, range: [0, 1]
  V Hue component, used as input, range: [0, 1]
*/

inline void rgb_to_hsv(f32 r, f32 g, f32 b, f32& h, f32& s, f32& v)
{
    f32 c_max = max(max(r, g), b);
    f32 c_min = min(min(r, g), b);
    f32 delta = c_max - c_min;
    
    if (delta > 0.f)
    {
        if (c_max == r)
        {
            h = 60.f * (fmod(((g - b) / delta), 6));
        }
        else if (c_max == g)
        {
            h = 60.f * (((b - r) / delta) + 2);
        }
        else if (c_max == b)
        {
            h = 60 * (((r - g) / delta) + 4);
        }
        
        if (c_max > 0.f)
        {
            s = delta / c_max;
        }
        else
        {
            s = 0.f;
        }
        
        v = c_max;
    }
    else
    {
        h = 0.f;
        s = 0.f;
        v = c_max;
    }
    
    if(h < 0.f)
    {
        h = 360.f + h;
    }
};

/*! Convert HSV to RGB color space
  
  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.
  
    Red component, used as output, range: [0, 1]
    Green component, used as output, range: [0, 1]
    Blue component, used as output, range: [0, 1]
    H Hue component, used as input, range: [0, 360]
    S Hue component, used as input, range: [0, 1]
    V Hue component, used as input, range: [0, 1]
*/

inline void hsv_to_rgb(f32& r, f32& g, f32& b, f32 h, f32 s, f32 v)
{
    f32 c = v * s; // Chroma
    f32 h_prime = fmod(h / 60.f, 6);
    f32 x = c * (1.f - fabs(fmod(h_prime, 2) - 1));
    f32 m = v - c;
    
    if (0.f <= h_prime && h_prime < 1.f)
    {
        r = c;
        g = x;
        b = 0.f;
    }
    else if (1.f <= h_prime && h_prime < 2.f)
    {
        r = x;
        g = c;
        b = 0.f;
    }
    else if (2.f <= h_prime && h_prime < 3.f)
    {
        r = 0.f;
        g = c;
        b = x;
    }
    else if (3.f <= h_prime && h_prime < 4.f)
    {
        r = 0.f;
        g = x;
        b = c;
    }
    else if (4.f <= h_prime && h_prime < 5.f)
    {
        r = x;
        g = 0.f;
        b = c;
    }
    else if (5.f <= h_prime && h_prime < 6.f)
    {
        r = c;
        g = 0.f;
        b = x;
    }
    else
    {
        r = 0.f;
        g = 0.f;
        b = 0.f;
    }
    
    r += m;
    g += m;
    b += m;
};


#define __CLASS_NAME__ class_name(__PRETTY_FUNCTION__)
}
