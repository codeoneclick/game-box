//
//  ui_declarations.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_declarations_h
#define ui_declarations_h

namespace gb
{
    namespace ui
    {
        class element;
        typedef std::shared_ptr<element> element_shared_ptr;
        
        class control;
        typedef std::shared_ptr<control> control_shared_ptr;
        
        class dialog;
        typedef std::shared_ptr<dialog> dialog_shared_ptr;
        
        class ui_fabricator;
        typedef std::shared_ptr<ui_fabricator> ui_fabricator_shared_ptr;
        
        class ui_graph;
        typedef std::shared_ptr<ui_graph> ui_graph_shared_ptr;
    };
};

#endif
