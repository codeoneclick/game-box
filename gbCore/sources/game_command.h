//
//  game_command.h
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_command_h
#define game_command_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class game_command_i
    {
    private:
        
    protected:
        
        game_command_i() = default;
        
    public:
        
        virtual ~game_command_i() = default;
    };
    
    template<class t_function>
    class game_command : public game_command_i
    {
        
    private:
        
        t_function m_function;
        
    protected:
        
    public:
        
        game_command(t_function function)
        {
            m_function = function;
        };
        
        ~game_command() = default;
        
        template<typename... t_args>
        void execute(t_args... args)
        {
            assert(m_function != nullptr);
            m_function(args...);
        };
    };
};

#endif
