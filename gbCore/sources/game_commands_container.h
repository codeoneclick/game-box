//
//  game_commands_container.h
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_commands_container_h
#define game_commands_container_h

#include "main_headers.h"
#include "declarations.h"
#include "game_command.h"

namespace gb
{
    class game_commands_container
    {
    private:
        
    protected:
        
        std::unordered_map<std::string, game_command_i_shared_ptr> m_commands;
        
    public:
        
        game_commands_container() = default;
        ~game_commands_container();
        
        void add_command(const std::string& guid, const game_command_i_shared_ptr& command);
        void remove_command(const std::string& guid);
        
        template<class t_function, typename... t_args>
        void execute(const std::string& guid, t_args... args)
        {
            auto command = m_commands.find(guid);
            if(command != m_commands.end())
            {
                std::shared_ptr<game_command<t_function>> function = std::static_pointer_cast<game_command<t_function>>(command->second);
                function->execute(args...);
            }
        };
    };
};

#endif
