#pragma once
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
namespace game {
// Zero disables outlines. IDs below 1000 are reserved for shared surfaces.
inline uint32_t outline_terrace_id(int logical_terrace) {
  return logical_terrace>=-9&&logical_terrace<=16 ? uint32_t(logical_terrace+11) : 0;
}
// Logical save IDs are arbitrary uint32 values. Never truncate or hash them
// into the transport range: distinct objects must retain distinct silhouettes.
// Inline local statics are shared across translation units. Both mesh workers
// and game-loop builders may call this registry, so allocation is synchronized.
inline uint32_t outline_object_id(uint32_t logical_id,uint32_t domain) {
  static std::mutex mutex;
  static std::unordered_map<uint64_t,uint32_t> ids;
  std::lock_guard<std::mutex> lock(mutex);
  const uint64_t key=(uint64_t(domain)<<32)|logical_id;
  const auto found=ids.find(key);
  if(found!=ids.end())return found->second;
  // This single bit pattern matches generic opaque vertices (zero spare
  // normal/tangent channels and alpha 255), so it must never be allocated.
  if(ids.size()>=(1u<<22)-1001)throw std::length_error("Object outline identity capacity exceeded");
  const uint32_t ordinal=1000+static_cast<uint32_t>(ids.size());
  const uint32_t id=ordinal+(ordinal>=0x3fc000u?1u:0u);
  ids.emplace(key,id);
  return id;
}
inline uint32_t outline_tree_id(uint32_t logical_id) {return outline_object_id(logical_id,0);}
inline uint32_t outline_stairs_id(uint32_t logical_id) {return outline_object_id(logical_id,1);}
}
