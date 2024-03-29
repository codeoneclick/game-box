//
//  b2CapsuleAndCircleContact.hpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#ifndef B2_CAPSULE_AND_CIRCLE_CONTACT_H
#define B2_CAPSULE_AND_CIRCLE_CONTACT_H

#include <box2d/Dynamics/Contacts/b2Contact.h>

class b2BlockAllocator;

class b2CapsuleAndCircleContact : public b2Contact
{
public:
    static b2Contact* Create(b2Fixture* fixtureA, int32 indexA,
                             b2Fixture* fixtureB, int32 indexB, b2BlockAllocator* allocator);
    static void Destroy(b2Contact* contact, b2BlockAllocator* allocator);
    
    b2CapsuleAndCircleContact(b2Fixture* fixtureA, b2Fixture* fixtureB);
    ~b2CapsuleAndCircleContact() {}
    
    void Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB);
};

#endif
