//
//  ed_declarations.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ed_declarations_h
#define ed_declarations_h

namespace gb
{
    namespace ed
    {
        class ed_fabricator;
        typedef std::shared_ptr<ed_fabricator> ed_fabricator_shared_ptr;
        
        class grid;
        typedef std::shared_ptr<grid> grid_shared_ptr;
        
        class stroke;
        typedef std::shared_ptr<stroke> stroke_shared_ptr;
    };
};
#endif
