//
//  ces_base_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "std_extensions.h"

namespace gb
{
    class ces_base_component : public std::enable_shared_from_this<ces_base_component>
    {
    private:
        
        friend class ces_entity;
        
    protected:
        
        static std::set<guid_t> g_guids_container;
        ces_entity_weak_ptr m_owner;
        
        ces_base_component();
        void set_owner(const ces_entity_shared_ptr& entity);
        
    public:
        
        typedef std::shared_ptr<ces_base_component> (component_constructor)();
        
        template <typename T, typename... ARGS> static std::shared_ptr<ces_base_component> construct(ARGS... args)
        {
            auto component = std::make_shared<T>(std::forward<ARGS>(args)...);
            return component;
        };
        
        CTTI_CLASS_GUID(ces_base_component, ces_base_component::g_guids_container)
        virtual ~ces_base_component() = default;
    };
};
