//
//  b2EdgeAndCapsuleContact.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Dynamics/Contacts/b2EdgeAndCapsuleContact.h>
#include <box2d/Common/b2BlockAllocator.h>
#include <box2d/Dynamics/b2Fixture.h>
#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <box2d/Collision/Shapes/b2CircleShape.h>
#include <box2d/Collision/Shapes/b2PolygonShape.h>

#include <new>
using namespace std;

b2Contact* b2EdgeAndCapsuleContact::Create(b2Fixture* fixtureA, int32, b2Fixture* fixtureB, int32, b2BlockAllocator* allocator)
{
    void* mem = allocator->Allocate(sizeof(b2EdgeAndCapsuleContact));
    return new (mem) b2EdgeAndCapsuleContact(fixtureA, fixtureB);
}

void b2EdgeAndCapsuleContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2EdgeAndCapsuleContact*)contact)->~b2EdgeAndCapsuleContact();
    allocator->Free(contact, sizeof(b2EdgeAndCapsuleContact));
}

b2EdgeAndCapsuleContact::b2EdgeAndCapsuleContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
: b2Contact(fixtureA, 0, fixtureB, 0)
{
    b2Assert(m_fixtureA->GetType() == b2Shape::e_edge);
    b2Assert(m_fixtureB->GetType() == b2Shape::e_capsule);
}

void b2EdgeAndCapsuleContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2CapsuleShape* capsule = (b2CapsuleShape*)m_fixtureB->GetShape();
    
    b2PolygonShape polygon;
    polygon.SetAsEdge(capsule->m_vertices[0], capsule->m_vertices[1]);
    polygon.m_radius = capsule->m_radius;
    
    b2CollideEdgeAndPolygon(manifold,
                            (b2EdgeShape*)m_fixtureA->GetShape(), xfA,
                            &polygon, xfB);
}
