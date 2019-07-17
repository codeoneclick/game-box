//
//  b2CapsuleShape.cpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <box2d/Collision/Shapes/b2CapsuleShape.h>
#include <new>

void b2CapsuleShape::SetByExtentsX(float32 radius, float32 length)
{
    m_radius = radius;
    float32 half = length * 0.5f;
    m_vertices[0] = b2Vec2(-half, 0.0f);
    m_vertices[1] = b2Vec2(half, 0.0f);
}

void b2CapsuleShape::SetByExtentsX(float32 radius, float32 length,
                                   const b2Vec2& center, float32 angle)
{
    SetByExtentsX(radius, length);
    b2Transform xf;
    xf.Set(center, angle);
    
    m_vertices[0] = b2Mul(xf, m_vertices[0]);
    m_vertices[1] = b2Mul(xf, m_vertices[1]);
}


void b2CapsuleShape::SetByExtentsY(float32 radius, float32 height)
{
    m_radius = radius;
    float32 half = height * 0.5f;
    m_vertices[0] = b2Vec2(0.0f, -half);
    m_vertices[1] = b2Vec2(0.0f, half);
}

void b2CapsuleShape::SetByExtentsY(float32 radius, float32 height, const b2Vec2& center, float32 angle)
{
    SetByExtentsY(radius, height);
    b2Transform xf;
    xf.Set(center, angle);
    
    m_vertices[0] = b2Mul(xf, m_vertices[0]);
    m_vertices[1] = b2Mul(xf, m_vertices[1]);
}

b2Shape* b2CapsuleShape::Clone(b2BlockAllocator* allocator) const
{
    void* mem = allocator->Allocate(sizeof(b2CapsuleShape));
    b2CapsuleShape* clone = new (mem) b2CapsuleShape;
    *clone = *this;
    return clone;
}

int32 b2CapsuleShape::GetChildCount() const
{
    return 1;
}

bool b2CapsuleShape::TestPoint(const b2Transform& transform, const b2Vec2& p) const
{
    b2Vec2 local = b2MulT(transform, p);
    b2Vec2 d = b2NearestPointOnLine(local, m_vertices[0], m_vertices[1]) - local;
    return (d.LengthSquared() <= m_radius * m_radius);
}

bool b2CapsuleShape::RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                             const b2Transform& transform, int32 childIndex) const
{
    B2_NOT_USED(childIndex);
    
    // Test for starting inside first significantly reduces complexity
    if (TestPoint(transform, input.p1)) {
        return false;
    }
    
    // Project capsule to world
    b2Vec2 capsule[2];
    capsule[0] = b2Mul(transform, m_vertices[0]);
    capsule[1] = b2Mul(transform, m_vertices[1]);
    b2Vec2 d = capsule[1] - capsule[0];
    
    // Consider "cylinder"
    
    // Figure out signed distance from p1 to infinite capsule line
    float32 ld = b2Cross(d, capsule[0] - input.p1);
    
    // Only bother if we don't start inside the infinite "cylinder"
    if (!(ld * ld <= m_radius * m_radius * d.LengthSquared())) {
        
        // Find a perpendicular vector to the intersect, with length equal to radius
        b2Vec2 perp = b2Cross(d, m_radius / d.Length());
        
        // Push the capsule segment by that vector
        // Must swap if coming from the other side
        b2Vec2 boundary[2];
        if (ld < 0) {
            boundary[0] = capsule[1] - perp;
            boundary[1] = capsule[0] - perp;
        }
        else {
            boundary[0] = capsule[0] + perp;
            boundary[1] = capsule[1] + perp;
        }
        
        // Check intersection against the adjusted segments.
        if (b2RaycastSegment(output, input, boundary[0], boundary[1]))
            return true;
    }
    
    // Consider circular caps
    
    // Precompute some circle values
    b2Vec2 r = input.p2 - input.p1;
    float32 rr = r.LengthSquared();
    
    // Check for short segment
    if (rr < b2_epsilon)
    {
        return false;
    }
    
    // Check the circle caps, starting with closer
    int startingIndex = 0;
    if ((capsule[1] - input.p1).LengthSquared() < (capsule[0] - input.p1).LengthSquared()) {
        startingIndex = 1;
    }
    b2Vec2 center = capsule[startingIndex];
    for (int i = 0; i < 2; ++i)
    {
        b2Vec2 s = input.p1 - center;
        float32 b = b2Dot(s, s) - m_radius * m_radius;
        
        // Should not start inside
        b2Assert(!(b < 0.0f));
        
        // Solve quadratic equation.
        float32 c =  b2Dot(s, r);
        float32 sigma = c * c - rr * b;
        
        // Check for negative discriminant.
        if (!(sigma < 0.0f))
        {
            // Find the point of intersection of the line with the circle.
            float32 a = -(c + b2Sqrt(sigma));
            
            // Is the intersection point on the segment?
            if (0.0f <= a && a <= input.maxFraction * rr)
            {
                a /= rr;
                output->fraction = a;
                output->normal = s + a * r;
                output->normal.Normalize();
                return true;
            }
        }
        center = capsule[!startingIndex];
    }
    
    return false;
}

void b2CapsuleShape::ComputeAABB(b2AABB* aabb, const b2Transform& transform, int32 childIndex) const
{
    B2_NOT_USED(childIndex);
    
    b2Vec2 p1 = transform.p + b2Mul(transform.q, m_vertices[0]);
    b2Vec2 p2 = transform.p + b2Mul(transform.q, m_vertices[1]);
    b2Vec2 min = b2Min(p1, p2);
    b2Vec2 max = b2Max(p1, p2);
    aabb->lowerBound.Set(min.x - m_radius, min.y - m_radius);
    aabb->upperBound.Set(max.x + m_radius, max.y + m_radius);
}

void b2CapsuleShape::ComputeMass(b2MassData* massData, float32 density) const
{
    // Area of rectangle + 2 half circles
    float32 rectHeight = (m_vertices[1] - m_vertices[0]).Length();
    float32 rectMass = density * rectHeight * m_radius;
    float32 circleMass = density * b2_pi * m_radius * m_radius;
    massData->mass = rectMass + circleMass;
    massData->center = 0.5f * (m_vertices[0] + m_vertices[1]);
    
    // inertia about the local origin
    float32 rectInertia = rectMass * (rectHeight * rectHeight + m_radius * m_radius) / 12.0f;
    float32 circleInertia = circleMass * (0.5f * m_radius * m_radius + rectHeight * rectHeight * .25f);
    massData->I = rectInertia + circleInertia + massData->mass * massData->center.LengthSquared();
}
