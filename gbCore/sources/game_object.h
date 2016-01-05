//
//  game_object.h
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_object_h
#define game_object_h

#include "ces_entity.h"

namespace gb
{
    class game_object : public ces_entity
    {
    private:
        
         std::weak_ptr<scene_graph> m_scene_graph;
        
    protected:
        
        void set_scene_graph(const scene_graph_shared_ptr& scene_graph);
        scene_graph_shared_ptr get_scene_graph() const;
        
    public:
        
        game_object();
        virtual ~game_object();
        
        virtual void on_added_to_scene(const scene_graph_shared_ptr& scene_graph);
        virtual void on_removed_from_scene();
        
        virtual void set_position(const glm::vec3& position);
        virtual glm::vec3 get_position() const;
        
        virtual void set_rotation(const glm::vec3& rotation);
        virtual glm::vec3 get_rotation() const;
        
        virtual void set_scale(const glm::vec3& scale);
        virtual glm::vec3 get_scale() const;
        
        virtual glm::vec3 get_forward() const;
        virtual glm::vec3 get_up() const;
        virtual glm::vec3 get_right() const;
    };
};

#endif
