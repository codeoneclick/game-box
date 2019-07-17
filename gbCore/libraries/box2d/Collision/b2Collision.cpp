/*
 * Copyright (c) 2007-2009 Erin Catto http://www.box2d.org
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <box2d/Collision/b2Collision.h>
#include <box2d/Collision/b2Distance.h>

void b2WorldManifold::Initialize(const b2Manifold* manifold,
                                 const b2Transform& xfA, float32 radiusA,
                                 const b2Transform& xfB, float32 radiusB)
{
    if (manifold->pointCount == 0)
    {
        return;
    }
    
    switch (manifold->type)
    {
        case b2Manifold::e_circles:
        {
            normal.Set(1.0f, 0.0f);
            b2Vec2 pointA = b2Mul(xfA, manifold->localPoint);
            b2Vec2 pointB = b2Mul(xfB, manifold->points[0].localPoint);
            if (b2DistanceSquared(pointA, pointB) > b2_epsilon * b2_epsilon)
            {
                normal = pointB - pointA;
                normal.Normalize();
            }
            
            b2Vec2 cA = pointA + radiusA * normal;
            b2Vec2 cB = pointB - radiusB * normal;
            points[0] = 0.5f * (cA + cB);
            separations[0] = b2Dot(cB - cA, normal);
        }
            break;
            
        case b2Manifold::e_faceA:
        {
            normal = b2Mul(xfA.q, manifold->localNormal);
            b2Vec2 planePoint = b2Mul(xfA, manifold->localPoint);
            
            for (int32 i = 0; i < manifold->pointCount; ++i)
            {
                b2Vec2 clipPoint = b2Mul(xfB, manifold->points[i].localPoint);
                b2Vec2 cA = clipPoint + (radiusA - b2Dot(clipPoint - planePoint, normal)) * normal;
                b2Vec2 cB = clipPoint - radiusB * normal;
                points[i] = 0.5f * (cA + cB);
                separations[i] = b2Dot(cB - cA, normal);
            }
        }
            break;
            
        case b2Manifold::e_faceB:
        {
            normal = b2Mul(xfB.q, manifold->localNormal);
            b2Vec2 planePoint = b2Mul(xfB, manifold->localPoint);
            
            for (int32 i = 0; i < manifold->pointCount; ++i)
            {
                b2Vec2 clipPoint = b2Mul(xfA, manifold->points[i].localPoint);
                b2Vec2 cB = clipPoint + (radiusB - b2Dot(clipPoint - planePoint, normal)) * normal;
                b2Vec2 cA = clipPoint - radiusA * normal;
                points[i] = 0.5f * (cA + cB);
                separations[i] = b2Dot(cA - cB, normal);
            }
            
            // Ensure normal points from A to B.
            normal = -normal;
        }
            break;
    }
}

void b2GetPointStates(b2PointState state1[b2_maxManifoldPoints], b2PointState state2[b2_maxManifoldPoints],
                      const b2Manifold* manifold1, const b2Manifold* manifold2)
{
    for (int32 i = 0; i < b2_maxManifoldPoints; ++i)
    {
        state1[i] = b2_nullState;
        state2[i] = b2_nullState;
    }
    
    // Detect persists and removes.
    for (int32 i = 0; i < manifold1->pointCount; ++i)
    {
        b2ContactID id = manifold1->points[i].id;
        
        state1[i] = b2_removeState;
        
        for (int32 j = 0; j < manifold2->pointCount; ++j)
        {
            if (manifold2->points[j].id.key == id.key)
            {
                state1[i] = b2_persistState;
                break;
            }
        }
    }
    
    // Detect persists and adds.
    for (int32 i = 0; i < manifold2->pointCount; ++i)
    {
        b2ContactID id = manifold2->points[i].id;
        
        state2[i] = b2_addState;
        
        for (int32 j = 0; j < manifold1->pointCount; ++j)
        {
            if (manifold1->points[j].id.key == id.key)
            {
                state2[i] = b2_persistState;
                break;
            }
        }
    }
}

// From Real-time Collision Detection, p179.
bool b2AABB::RayCast(b2RayCastOutput* output, const b2RayCastInput& input) const
{
    float32 tmin = -b2_maxFloat;
    float32 tmax = b2_maxFloat;
    
    b2Vec2 p = input.p1;
    b2Vec2 d = input.p2 - input.p1;
    b2Vec2 absD = b2Abs(d);
    
    b2Vec2 normal;
    
    for (int32 i = 0; i < 2; ++i)
    {
        if (absD(i) < b2_epsilon)
        {
            // Parallel.
            if (p(i) < lowerBound(i) || upperBound(i) < p(i))
            {
                return false;
            }
        }
        else
        {
            float32 inv_d = 1.0f / d(i);
            float32 t1 = (lowerBound(i) - p(i)) * inv_d;
            float32 t2 = (upperBound(i) - p(i)) * inv_d;
            
            // Sign of the normal vector.
            float32 s = -1.0f;
            
            if (t1 > t2)
            {
                b2Swap(t1, t2);
                s = 1.0f;
            }
            
            // Push the min up
            if (t1 > tmin)
            {
                normal.SetZero();
                normal(i) = s;
                tmin = t1;
            }
            
            // Pull the max down
            tmax = b2Min(tmax, t2);
            
            if (tmin > tmax)
            {
                return false;
            }
        }
    }
    
    // Does the ray start inside the box?
    // Does the ray intersect beyond the max fraction?
    if (tmin < 0.0f || input.maxFraction < tmin)
    {
        return false;
    }
    
    // Intersection.
    output->fraction = tmin;
    output->normal = normal;
    return true;
}

// Sutherland-Hodgman clipping.
int32 b2ClipSegmentToLine(b2ClipVertex vOut[2], const b2ClipVertex vIn[2],
                          const b2Vec2& normal, float32 offset, int32 vertexIndexA)
{
    // Start with no output points
    int32 numOut = 0;
    
    // Calculate the distance of end points to the line
    float32 distance0 = b2Dot(normal, vIn[0].v) - offset;
    float32 distance1 = b2Dot(normal, vIn[1].v) - offset;
    
    // If the points are behind the plane
    if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
    if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];
    
    // If the points are on different sides of the plane
    if (distance0 * distance1 < 0.0f)
    {
        // Find intersection point of edge and plane
        float32 interp = distance0 / (distance0 - distance1);
        vOut[numOut].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);
        
        // VertexA is hitting edgeB.
        vOut[numOut].id.cf.indexA = static_cast<uint8>(vertexIndexA);
        vOut[numOut].id.cf.indexB = vIn[0].id.cf.indexB;
        vOut[numOut].id.cf.typeA = b2ContactFeature::e_vertex;
        vOut[numOut].id.cf.typeB = b2ContactFeature::e_face;
        ++numOut;
    }
    
    return numOut;
}

bool b2TestOverlap(	const b2Shape* shapeA, int32 indexA,
                   const b2Shape* shapeB, int32 indexB,
                   const b2Transform& xfA, const b2Transform& xfB)
{
    b2DistanceInput input;
    input.proxyA.Set(shapeA, indexA);
    input.proxyB.Set(shapeB, indexB);
    input.transformA = xfA;
    input.transformB = xfB;
    input.useRadii = true;
    
    b2SimplexCache cache;
    cache.count = 0;
    
    b2DistanceOutput output;
    
    b2Distance(&output, &cache, &input);
    
    return output.distance < 10.0f * b2_epsilon;
}

bool b2RaycastSegment(b2RayCastOutput* output, const b2RayCastInput& input, const b2Vec2& p1, const b2Vec2& p2)
{
    b2Vec2 s = input.p1;
    b2Vec2 r = input.p2 - s;
    b2Vec2 d = p2 - p1;
    b2Vec2 n = b2Cross(d, 1.0f);
    
    const float32 k_slop = 100.0f * b2_epsilon;
    float32 denom = -b2Dot(r, n);
    
    // Cull back facing collision and ignore parallel segments.
    if (denom > k_slop)
    {
        // Does the segment intersect the infinite line associated with this segment?
        b2Vec2 b = s - p1;
        float32 a = b2Dot(b, n);
        
        if (0.0f <= a && a <= input.maxFraction * denom)
        {
            float32 mu2 = -r.x * b.y + r.y * b.x;
            
            // Does the segment intersect this segment?
            if (-k_slop * denom <= mu2 && mu2 <= denom * (1.0f + k_slop))
            {
                a /= denom;
                n.Normalize();
                output->normal = n;
                output->fraction = a;
                return true;
            }
        }
    }
    
    return false;
}


static void FindNearestPointOnLineSegment(    b2Vec2* nearest, float32* parameter,
                                          const b2Vec2& A1, const b2Vec2& L, const b2Vec2& B,
                                          bool infinite_line )
{
    float32 D = L.LengthSquared();
    if (D < b2_epsilon * b2_epsilon)
    {
        *nearest = A1;
        return;
    }
    
    b2Vec2 AB = B - A1;
    *parameter = b2Dot(L, AB) / D;
    if (!infinite_line)
    {
        *parameter = b2Clamp(*parameter, 0.0f, 1.0f);
    }
    *nearest = A1 + *parameter * L;
}

// Based on Game Programming Gems 2
// Fast, Robust Intersection of 3D Line Segments
// Graham Rhodes, Applied Research Associates
b2Vec2 b2NearestPointOnLine(const b2Vec2& point, const b2Vec2& p1, const b2Vec2& p2)
{
    b2Vec2 result;
    float32 unused;
    FindNearestPointOnLineSegment(&result, &unused, p1, p2 - p1, point, false);
    return result;
}

static void FindNearestPointOfParallelLineSegments(    b2Vec2* x1, b2Vec2* x2,
                                                   const b2Vec2& A1, const b2Vec2& A2, const b2Vec2& La,
                                                   const b2Vec2& B1, const b2Vec2& B2, const b2Vec2& Lb)
{
    float32 s, t;
    FindNearestPointOnLineSegment(x1, &s, A1, La, B1, true);
    FindNearestPointOnLineSegment(x2, &t, A1, La, B2, true);
    
    if (s < 0.0f && t < 0.0f)
    {
        *x1 = A1;
        if (s < t)
            *x2 = B2;
        else
            *x2 = B1;
        return;
    }
    
    if (s > 1.0f && t > 1.0f)
    {
        *x1 = A2;
        if (s < t)
            *x2 = B1;
        else
            *x2 = B2;
        return;
    }
    
    float32 temp = 0.5f * (b2Clamp(s, 0.0f, 1.0f) + b2Clamp(t, 0.0f, 1.0f));
    *x1 = A1 + temp * La;
    
    float32 unused;
    FindNearestPointOnLineSegment(x2, &unused, B1, Lb, *x1, true);
}

static void AdjustNearestPoints(b2Vec2* x1, b2Vec2* x2,
                                const b2Vec2& A1, const b2Vec2& A2, const b2Vec2& La,
                                const b2Vec2& B1, const b2Vec2& B2, const b2Vec2& Lb,
                                float32 s, float32 t)
{
    if ((s < 0.0f || s > 1.0f) && (t < 0.0f || t > 1.0f))
    {
        s = b2Clamp(s, 0.0f, 1.0f);
        *x1 = A1 + s * La;
        FindNearestPointOnLineSegment(x2, &t, B1, Lb, *x1, true);
        if (t < 0.0f || t > 1.0f)
        {
            t = b2Clamp(t, 0.0f, 1.0f);
            *x2 = B1 + t * Lb;
            FindNearestPointOnLineSegment(x1, &s, A1, La, *x2, false);
            FindNearestPointOnLineSegment(x2, &t, B1, Lb, *x1, false);
        }
        return;
    }
    if (s < 0.0f || s > 1.0f)
    {
        s = b2Clamp(s, 0.0f, 1.0f);
        *x1 = A1 + s * La;
        FindNearestPointOnLineSegment(x2, &t, B1, Lb, *x1, false);
        return;
    }
    b2Assert(t < 0.0f || t > 1.0f);
    t = b2Clamp(t, 0.0f, 1.0f);
    *x2 = B1 + t * Lb;
    FindNearestPointOnLineSegment(x1, &s, A1, La, *x2, false);
}

static void IntersectLineSegments(    b2Vec2* x1, b2Vec2* x2,
                                  const b2Vec2& A1, const b2Vec2& A2,
                                  const b2Vec2& B1, const b2Vec2& B2)
{
    float32 unused;
    
    b2Vec2 La = A2 - A1;
    b2Vec2 Lb = B2 - B1;
    float32 L11 = La.LengthSquared();
    float32 L22 = Lb.LengthSquared();
    
    // Check for degenerate parameters
    if (L11 < b2_epsilon * b2_epsilon)
    {
        *x1 = A1;
        FindNearestPointOnLineSegment(x2, &unused, B1, Lb, A1, false);
        return;
    }
    if (L22 < b2_epsilon * b2_epsilon)
    {
        *x2 = B1;
        FindNearestPointOnLineSegment(x1, &unused, A1, La, B1, false);
        return;
    }
    
    b2Vec2 AB = B1 - A1;
    float32 L12 = -b2Dot(La, Lb);
    float32 DetL = L11 * L22 - L12 * L12;
    
    // parallel segments
    if (b2Abs(DetL) < b2_epsilon)
    {
        FindNearestPointOfParallelLineSegments(x1, x2, A1, A2, La, B1, B2, Lb);
        return;
    }
    
    float32 ra = b2Dot(La, AB);
    float32 rb = -b2Dot(Lb, AB);
    
    float32 t = (L11 * rb - ra * L12) / DetL;
    float32 s = (ra - L12 * t) / L11;
    
    // These tests can't quite stay within B2_FLT_EPSILON
    //b2Assert(b2Abs(s * L11 + t * L12 - ra) < .0001f);
    //b2Assert(b2Abs(s * L12 + t * L22 - rb) < .0001f);
    
    *x1 = A1 + s * La;
    *x2 = B1 + t * Lb;
    
    if (s < 0.0f || s > 1.0f || t < 0.0f || t > 1.0f)
    {
        AdjustNearestPoints(x1, x2, A1, A2, La, B1, B2, Lb, s, t);
    }
}

// Based on Game Programming Gems 2
// Fast, Robust Intersection of 3D Line Segments
// Graham Rhodes, Applied Research Associates
float32 b2DistanceBetweenLines(    b2Vec2* x1, b2Vec2* x2,
                               const b2Vec2& A1, const b2Vec2& A2,
                               const b2Vec2& B1, const b2Vec2& B2)
{
    IntersectLineSegments(x1, x2, A1, A2, B1, B2);
    return (*x2 - *x1).Length();
}
