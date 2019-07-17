//
//  b2CapsuleContact.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Dynamics/Contacts/b2CapsuleContact.h>
#include <box2d/Dynamics/b2Body.h>
#include <box2d/Dynamics/b2Fixture.h>
#include <box2d/Dynamics/b2WorldCallbacks.h>
#include <box2d/Common/b2BlockAllocator.h>
#include <box2d/Collision/b2TimeOfImpact.h>
#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <box2d/Collision/Shapes/b2PolygonShape.h>
#include <new>

b2Contact* b2CapsuleContact::Create(b2Fixture* fixtureA, int32, b2Fixture* fixtureB, int32, b2BlockAllocator* allocator)
{
    void* mem = allocator->Allocate(sizeof(b2CapsuleContact));
    return new (mem) b2CapsuleContact(fixtureA, fixtureB);
}

void b2CapsuleContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2CapsuleContact*)contact)->~b2CapsuleContact();
    allocator->Free(contact, sizeof(b2CapsuleContact));
}

b2CapsuleContact::b2CapsuleContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
: b2Contact(fixtureA, 0, fixtureB, 0)
{
    b2Assert(m_fixtureA->GetType() == b2Shape::e_capsule);
    b2Assert(m_fixtureB->GetType() == b2Shape::e_capsule);
}

void b2CapsuleContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2CapsuleShape* capsule1 = (b2CapsuleShape*)m_fixtureA->GetShape();
    b2CapsuleShape* capsule2 = (b2CapsuleShape*)m_fixtureB->GetShape();
    
    b2PolygonShape polygon1, polygon2;
    polygon1.SetAsEdge(capsule1->m_vertices[0], capsule1->m_vertices[1]);
    polygon2.SetAsEdge(capsule2->m_vertices[0], capsule2->m_vertices[1]);
    polygon1.m_radius = capsule1->m_radius;
    polygon2.m_radius = capsule2->m_radius;
    
    b2CollidePolygons(manifold,
                      &polygon1, xfA,
                      &polygon2, xfB);
}
