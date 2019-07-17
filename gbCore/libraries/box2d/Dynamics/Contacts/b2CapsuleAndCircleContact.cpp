//
//  b2CapsuleAndCircleContact.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Dynamics/Contacts/b2CapsuleAndCircleContact.h>
#include <box2d/Dynamics/b2Body.h>
#include <box2d/Dynamics/b2Fixture.h>
#include <box2d/Dynamics/b2WorldCallbacks.h>
#include <box2d/Common/b2BlockAllocator.h>
#include <box2d/Collision/b2TimeOfImpact.h>
#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <box2d/Collision/Shapes/b2PolygonShape.h>

#include <new>

b2Contact* b2CapsuleAndCircleContact::Create(b2Fixture* fixtureA, int32, b2Fixture* fixtureB, int32, b2BlockAllocator* allocator)
{
    void* mem = allocator->Allocate(sizeof(b2CapsuleAndCircleContact));
    return new (mem) b2CapsuleAndCircleContact(fixtureA, fixtureB);
}

void b2CapsuleAndCircleContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2CapsuleAndCircleContact*)contact)->~b2CapsuleAndCircleContact();
    allocator->Free(contact, sizeof(b2CapsuleAndCircleContact));
}

b2CapsuleAndCircleContact::b2CapsuleAndCircleContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
: b2Contact(fixtureA, 0, fixtureB, 0)
{
    b2Assert(m_fixtureA->GetType() == b2Shape::e_capsule);
    b2Assert(m_fixtureB->GetType() == b2Shape::e_circle);
}

void b2CapsuleAndCircleContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2CapsuleShape* capsule = (b2CapsuleShape*)m_fixtureA->GetShape();
    
    b2PolygonShape polygon;
    polygon.SetAsEdge(capsule->m_vertices[0], capsule->m_vertices[1]);
    polygon.m_radius = capsule->m_radius;
    
    b2CollidePolygonAndCircle(manifold,
                              &polygon, xfA,
                              (b2CircleShape*)m_fixtureB->GetShape(), xfB);
}
