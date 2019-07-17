//
//  b2CapsuleShape.hpp
//  gbCore
//
//  Created by serhii.s on 7/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#ifndef B2_CAPSULE_SHAPE_H
#define B2_CAPSULE_SHAPE_H

#include <box2d/Collision/Shapes/b2Shape.h>

/// A circle shape.
class b2CapsuleShape : public b2Shape
{
public:
    b2CapsuleShape();
    
    /// Set to a capsule across the X axis, with given width between focii.
    void SetByExtentsX(float32 radius, float32 width);
    /// Set to a capsule across the X axis, rotated and transposed.
    void SetByExtentsX(float32 radius, float32 width,
                       const b2Vec2& center, float32 angle);
    
    /// Set to a capsule across the Y axis, with given height between focii.
    void SetByExtentsY(float32 radius, float32 height);
    /// Set to a capsule across the Y axis, rotated and transposed.
    void SetByExtentsY(float32 radius, float32 height,
                       const b2Vec2& center, float32 angle);
    
    /// Implement b2Shape.
    b2Shape* Clone(b2BlockAllocator* allocator) const;
    
    /// @see b2Shape::GetChildCount
    int32 GetChildCount() const;
    
    /// Implement b2Shape.
    bool TestPoint(const b2Transform& transform, const b2Vec2& p) const;
    
    /// Implement b2Shape.
    bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                 const b2Transform& transform, int32 childIndex) const;
    
    /// @see b2Shape::ComputeAABB
    void ComputeAABB(b2AABB* aabb, const b2Transform& transform, int32 childIndex) const;
    
    /// @see b2Shape::ComputeMass
    void ComputeMass(b2MassData* massData, float32 density) const;
    
    /// Get the supporting vertex index in the given direction.
    int32 GetSupport(const b2Vec2& d) const;
    
    /// Get the supporting vertex in the given direction.
    const b2Vec2& GetSupportVertex(const b2Vec2& d) const;
    
    /// Get the vertex count.
    int32 GetVertexCount() const { return 2; }
    
    /// Get a vertex by index. Used by b2Distance.
    const b2Vec2& GetVertex(int32 index) const;
    
    /// Position of circle centers
    b2Vec2 m_vertices[2];
};

inline b2CapsuleShape::b2CapsuleShape()
{
    m_type = e_capsule;
    m_radius = 0.0f;
    m_vertices[0].SetZero();
    m_vertices[1].SetZero();
}


inline int32 b2CapsuleShape::GetSupport(const b2Vec2 &d) const
{
    if ((m_vertices[0] - d).LengthSquared() <= (m_vertices[1] - d).LengthSquared()) {
        return 0;
    }
    return 1;
}

inline const b2Vec2& b2CapsuleShape::GetSupportVertex(const b2Vec2 &d) const
{
    return GetVertex(GetSupport(d));
}

inline const b2Vec2& b2CapsuleShape::GetVertex(int32 index) const
{
    b2Assert(index == 0 || index == 1);
    return m_vertices[index];
}



#endif
