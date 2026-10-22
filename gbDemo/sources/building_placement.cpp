#include "building_placement.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <limits>
#include <stdexcept>
namespace game {
namespace {
using Point = std::array<double, 2>;
Point local(float x, float z, float cx, float cz, float yaw) {
  double c = std::cos(double(yaw)), s = std::sin(double(yaw));
  const double dx = double(x) - cx, dz = double(z) - cz;
  return {{c * dx - s * dz, s * dx + c * dz}};
}
std::array<Point, 4> corners(const EditorBuilding &b, float extra) {
  auto &d = building_definition(b.type);
  std::array<Point, 4> p;
  double c = std::cos(b.yaw), s = std::sin(b.yaw);
  int i = 0;
  for (auto q :
       std::array<Point, 4>{{{{-d.half_width - extra, -d.half_depth - extra}},
                             {{d.half_width + extra, -d.half_depth - extra}},
                             {{d.half_width + extra, d.half_depth + extra}},
                             {{-d.half_width - extra, d.half_depth + extra}}}})
    p[i++] = {{b.position[0] + c * q[0] + s * q[1],
               b.position[2] - s * q[0] + c * q[1]}};
  return p;
}
double area(const std::vector<Point> &p) {
  double a = 0;
  for (size_t i = 0; i < p.size(); ++i) {
    auto &q = p[(i + 1) % p.size()];
    a += p[i][0] * q[1] - q[0] * p[i][1];
  }
  return std::abs(a) * .5;
}
std::vector<Point> clip(std::vector<Point> p, int axis, double boundary,
                        bool lower) {
  std::vector<Point> out;
  if (p.empty())
    return out;
  auto prev = p.back();
  bool pin = lower ? prev[axis] >= boundary : prev[axis] <= boundary;
  for (auto curr : p) {
    bool cin = lower ? curr[axis] >= boundary : curr[axis] <= boundary;
    if (cin != pin) {
      double t = (boundary - prev[axis]) / (curr[axis] - prev[axis]);
      out.push_back({{prev[0] + t * (curr[0] - prev[0]),
                      prev[1] + t * (curr[1] - prev[1])}});
    }
    if (cin)
      out.push_back(curr);
    prev = curr;
    pin = cin;
  }
  return out;
}
} // namespace
float normalized_building_yaw(float v) {
  if (!std::isfinite(v))
    return 0;
  v = std::fmod(v, 6.283185307179586f);
  if (v < 0)
    v += 6.283185307179586f;
  // Adding a tiny negative remainder can round back to exactly 2*pi.
  return v >= 6.283185307179586f ? 0.f : v;
}
bool point_inside_building(const EditorBuilding &b, float x, float z,
                           float extra) {
  auto p = local(x, z, b.position[0], b.position[2], b.yaw);
  auto &d = building_definition(b.type);
  return std::abs(p[0]) <= d.half_width + extra &&
         std::abs(p[1]) <= d.half_depth + extra;
}
bool buildings_overlap(const EditorBuilding &a, const EditorBuilding &b,
                       float extra) {
  auto pa = corners(a, extra), pb = corners(b, extra);
  for (float yaw : {a.yaw, b.yaw})
    for (int axis = 0; axis < 2; ++axis) {
      double nx = axis ? std::sin(yaw) : std::cos(yaw),
             nz = axis ? std::cos(yaw) : -std::sin(yaw);
      double amin = 1e30, amax = -1e30, bmin = 1e30, bmax = -1e30;
      for (auto p : pa) {
        double v = p[0] * nx + p[1] * nz;
        amin = std::min(amin, v);
        amax = std::max(amax, v);
      }
      for (auto p : pb) {
        double v = p[0] * nx + p[1] * nz;
        bmin = std::min(bmin, v);
        bmax = std::max(bmax, v);
      }
      if (amax <= bmin + 1e-6 || bmax <= amin + 1e-6)
        return false;
    }
  return true;
}
BuildingSurfaceSupport::BuildingSurfaceSupport(
    const std::vector<terrain_mesh_data> &parts, float size)
    : bucket_size_(size) {
  if (!std::isfinite(size) || size <= 0)
    throw std::invalid_argument("Invalid surface bucket size");
  for (auto &mesh : parts)
    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
      auto ia = mesh.indices[i], ib = mesh.indices[i + 1],
           ic = mesh.indices[i + 2];
      if (ia >= mesh.vertices.size() || ib >= mesh.vertices.size() ||
          ic >= mesh.vertices.size())
        continue;
      auto &a = mesh.vertices[ia];
      auto &b = mesh.vertices[ib];
      auto &c = mesh.vertices[ic];
      if (a.normal[1] < .99f || b.normal[1] < .99f || c.normal[1] < .99f ||
          std::abs(a.position[1] - b.position[1]) > 1e-4f ||
          std::abs(a.position[1] - c.position[1]) > 1e-4f)
        continue;
      Triangle t;
      t.height = a.position[1];
      t.p = {{{{a.position[0], a.position[2]}},
              {{b.position[0], b.position[2]}},
              {{c.position[0], c.position[2]}}}};
      float x0 = std::min({t.p[0][0], t.p[1][0], t.p[2][0]}),
            x1 = std::max({t.p[0][0], t.p[1][0], t.p[2][0]}),
            z0 = std::min({t.p[0][1], t.p[1][1], t.p[2][1]}),
            z1 = std::max({t.p[0][1], t.p[1][1], t.p[2][1]});
      size_t id = triangles_.size();
      triangles_.push_back(t);
      for (int z = int(std::floor(z0 / size)); z <= int(std::floor(z1 / size));
           ++z)
        for (int x = int(std::floor(x0 / size));
             x <= int(std::floor(x1 / size)); ++x)
          buckets_[{x, z}].push_back(id);
    }
}
bool BuildingSurfaceSupport::covers(float x, float z, float yaw, float w,
                                    float d, float height) const {
  if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(yaw) ||
      !std::isfinite(w) || !std::isfinite(d) || !std::isfinite(height) ||
      w <= 0 || d <= 0)
    return false;
  double c = std::abs(std::cos(yaw)), s = std::abs(std::sin(yaw)),
         rx = c * w + s * d, rz = s * w + c * d;
  std::set<size_t> ids;
  for (int bz = int(std::floor((z - rz) / bucket_size_));
       bz <= int(std::floor((z + rz) / bucket_size_)); ++bz)
    for (int bx = int(std::floor((x - rx) / bucket_size_));
         bx <= int(std::floor((x + rx) / bucket_size_)); ++bx) {
      auto it = buckets_.find({bx, bz});
      if (it != buckets_.end())
        ids.insert(it->second.begin(), it->second.end());
    }
  double covered = 0;
  for (auto id : ids) {
    auto &t = triangles_[id];
    std::vector<Point> p;
    for (auto q : t.p)
      p.push_back(local(q[0], q[1], x, z, yaw));
    p = clip(clip(clip(clip(p, 0, -w, true), 0, w, false), 1, -d, true), 1, d,
             false);
    double a = area(p);
    if (a > 1e-9 && std::abs(t.height - height) > 1e-4)
      return false;
    covered += a;
  }
  double wanted = 4. * w * d;
  // Terrain vertices are floats: independently clipped chunk boundaries can
  // overlap or separate by a few coordinate ULPs. Compare area within the
  // corresponding perimeter error, rather than rejecting a visually flat seam.
  // Real holes, overhangs and triangles at another height remain rejected.
  const double coordinate_error = 4. * std::numeric_limits<float>::epsilon() *
      std::max({1., std::abs(double(x)) + rx, std::abs(double(z)) + rz});
  const double area_error = 4. * (double(w) + d) * coordinate_error +
      4. * coordinate_error * coordinate_error;
  return std::abs(covered - wanted) <=
      std::max({1e-7, wanted * 1e-7, area_error});
}
bool BuildingSurfaceSupport::height_at(float x, float z, float &height) const {
  if (!std::isfinite(x) || !std::isfinite(z)) return false;
  auto bucket = buckets_.find({int(std::floor(x / bucket_size_)),
                              int(std::floor(z / bucket_size_))});
  if (bucket == buckets_.end()) return false;
  bool found = false;
  for (auto id : bucket->second) {
    const auto &t = triangles_[id];
    double signs[3];
    for (int i = 0; i < 3; ++i) {
      const auto &a = t.p[i], &b = t.p[(i + 1) % 3];
      signs[i] = double(b[0] - a[0]) * (z - a[1]) -
                 double(b[1] - a[1]) * (x - a[0]);
    }
    const double twice_area =
        double(t.p[1][0]-t.p[0][0])*(t.p[2][1]-t.p[0][1])-
        double(t.p[1][1]-t.p[0][1])*(t.p[2][0]-t.p[0][0]);
    if (std::abs(twice_area) < 1e-12) continue;
    if ((*std::min_element(signs, signs + 3) >= -1e-7 ||
         *std::max_element(signs, signs + 3) <= 1e-7)) {
      if (!found || t.height > height) height = t.height;
      found = true;
    }
  }
  return found;
}
std::vector<BuildingSurfaceSupport::SurfaceEdge>
BuildingSurfaceSupport::nearby_edges(float x, float z, float radius) const {
  std::vector<SurfaceEdge> result;
  if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(radius) || radius < 0)
    return result;
  std::set<size_t> ids;
  for (int bz=int(std::floor((z-radius)/bucket_size_));
       bz<=int(std::floor((z+radius)/bucket_size_)); ++bz)
    for (int bx=int(std::floor((x-radius)/bucket_size_));
         bx<=int(std::floor((x+radius)/bucket_size_)); ++bx) {
      auto it=buckets_.find({bx,bz});
      if(it!=buckets_.end()) ids.insert(it->second.begin(),it->second.end());
    }
  for(auto id:ids) {
    const auto &t=triangles_[id];
    for(int i=0;i<3;++i) {
      const auto &a=t.p[i], &b=t.p[(i+1)%3];
      const float dx=b[0]-a[0], dz=b[1]-a[1], length2=dx*dx+dz*dz;
      if(length2<1e-10f) continue;
      const float u=std::max(0.f,std::min(1.f,((x-a[0])*dx+(z-a[1])*dz)/length2));
      const float ex=a[0]+u*dx-x, ez=a[1]+u*dz-z;
      if(ex*ex+ez*ez<=radius*radius) result.push_back({a,b,t.height});
    }
  }
  return result;
}
} // namespace game
