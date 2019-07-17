//
//  b2LoopAndCapsuleContact.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Dynamics/Contacts/b2LoopAndCapsuleContact.h>
#include <box2d/Common/b2BlockAllocator.h>
#include <box2d/Dynamics/b2Fixture.h>
#include <box2d/Collision/Shapes/b2ChainShape.h>
#include <box2d/Collision/Shapes/b2EdgeShape.h>
#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <box2d/Collision/Shapes/b2PolygonShape.h>

#include <new>
using namespace std;

b2Contact* b2LoopAndCapsuleContact::Create(b2Fixture* fixtureA, int32 indexA, b2Fixture* fixtureB, int32 indexB, b2BlockAllocator* allocator)
{
    void* mem = allocator->Allocate(sizeof(b2LoopAndCapsuleContact));
    return new (mem) b2LoopAndCapsuleContact(fixtureA, indexA, fixtureB, indexB);
}

void b2LoopAndCapsuleContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2LoopAndCapsuleContact*)contact)->~b2LoopAndCapsuleContact();
    allocator->Free(contact, sizeof(b2LoopAndCapsuleContact));
}

b2LoopAndCapsuleContact::b2LoopAndCapsuleContact(b2Fixture* fixtureA, int32 indexA, b2Fixture* fixtureB, int32 indexB)
: b2Contact(fixtureA, indexA, fixtureB, indexB)
{
    b2Assert(m_fixtureA->GetType() == b2Shape::e_chain);
    b2Assert(m_fixtureB->GetType() == b2Shape::e_capsule);
}

void b2LoopAndCapsuleContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2ChainShape* loop = (b2ChainShape*)m_fixtureA->GetShape();
    b2EdgeShape edge;
    loop->GetChildEdge(&edge, m_indexA);
    
    b2CapsuleShape* capsule = (b2CapsuleShape*)m_fixtureB->GetShape();
    
    b2PolygonShape polygon;
    polygon.SetAsEdge(capsule->m_vertices[0], capsule->m_vertices[1]);
    polygon.m_radius = capsule->m_radius;
    
    b2CollideEdgeAndPolygon(manifold,
                            &edge, xfA,
                            &polygon, xfB);
}
