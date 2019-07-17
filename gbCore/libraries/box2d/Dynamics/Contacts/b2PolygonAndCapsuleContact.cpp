//
//  b2PolygonAndCapsuleContact.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Dynamics/Contacts/b2PolygonAndCapsuleContact.h>
#include <box2d/Dynamics/b2Body.h>
#include <box2d/Dynamics/b2Fixture.h>
#include <box2d/Dynamics/b2WorldCallbacks.h>
#include <box2d/Common/b2BlockAllocator.h>
#include <box2d/Collision/b2TimeOfImpact.h>
#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <box2d/Collision/Shapes/b2PolygonShape.h>

#include <new>

b2Contact* b2PolygonAndCapsuleContact::Create(b2Fixture* fixtureA, int32, b2Fixture* fixtureB, int32, b2BlockAllocator* allocator)
{
    void* mem = allocator->Allocate(sizeof(b2PolygonAndCapsuleContact));
    return new (mem) b2PolygonAndCapsuleContact(fixtureA, fixtureB);
}

void b2PolygonAndCapsuleContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2PolygonAndCapsuleContact*)contact)->~b2PolygonAndCapsuleContact();
    allocator->Free(contact, sizeof(b2PolygonAndCapsuleContact));
}

b2PolygonAndCapsuleContact::b2PolygonAndCapsuleContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
: b2Contact(fixtureA, 0, fixtureB, 0)
{
    b2Assert(m_fixtureA->GetType() == b2Shape::e_polygon);
    b2Assert(m_fixtureB->GetType() == b2Shape::e_capsule);
}

void b2PolygonAndCapsuleContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2CapsuleShape* capsule = (b2CapsuleShape*)m_fixtureB->GetShape();
    
    b2PolygonShape polygon;
    polygon.SetAsEdge(capsule->m_vertices[0], capsule->m_vertices[1]);
    polygon.m_radius = capsule->m_radius;
    
    b2CollidePolygons(manifold,
                      (b2PolygonShape*)m_fixtureA->GetShape(), xfA,
                      &polygon, xfB);
}
